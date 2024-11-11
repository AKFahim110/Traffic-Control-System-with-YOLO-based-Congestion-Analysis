//
//  Intersection.cpp
//  TraffikTrak


#include <set>
#include <mutex>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <time.h>
#include "Intersection.hpp"
#include "Road.hpp"
#include "Direction.h"
#include "IntersectionID.h"
#include "RedLight.hpp"
#include "GreenLight.hpp"
#include "LightColour.h"
#include "AbstractIntersectionState.hpp"
#include "NorthSouthState.hpp"
#include "EastWestState.hpp"
#include "DefaultTrafficLightScheduler.hpp"
#include "Controller.hpp"
#include "ProcessedImage.hpp"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace std::chrono;
using namespace traffictrack;


/** @fn countTimeSinceLastChange()
 *  @brief looping thread that counts the number of seconds since the last time the lights were changed
 */
void Intersection::countTimeSinceLastChange() {
    
    while(!stopRequested()) {
        
        waitFor(seconds(1));
        lock_guard<mutex> guard(timeCounterMutex_);
        timeSinceLastChange_++;
        
    }
    
}


/** @fn monitorIntersection()
 *  @brief looping thread that gathers real time data from the intersection using the traffic light cameras, and decides how to handle the information
 */
void Intersection::monitorIntersection() {
    
    //create the thread that counts the time since the last light change
    unique_lock<mutex> timeCounterThreadLock(timeCounterThreadMutex_);
    if (timeCounterThread_ == nullptr) {
        timeCounterThread_ = new thread(&Intersection::countTimeSinceLastChange, this);
    }
    timeCounterThreadLock.unlock();
    
    //loop to take photos and implement any logic associated with analyzing the real time data
    while (!stopRequested()) {
        
        {
            unique_lock<mutex> timeCounterLock(timeCounterMutex_);
            if (timeSinceLastChange_ >= state_->intervalTime()) {
                timeCounterLock.unlock();
                this->changeLights();
            }
        }
        

        //take one photo for each traffic light   
        cv::String img1 = lights_.at(0)->takePhoto();
        cv::String img2 = lights_.at(1)->takePhoto();
        cv::String img3 = lights_.at(2)->takePhoto();
        cv::String img4 = lights_.at(3)->takePhoto();

        ProcessedImage data = ProcessedImage(img1,img2 ,img3, img4);//process the photos
        DateScorePair scores = data.carCount();
        analyzer_ -> analyze(scores.second, this);//change the traffic light based on real time data
        Controller::instance()->logScore(this->ID_, scores);
            
            
        
        waitFor(std::chrono::seconds(5));
        
    }
    
    //join the counting thread when stopping the internal thread
    timeCounterThreadLock.lock();
    if (timeCounterThread_->joinable()) {
        timeCounterThread_->join();
        delete timeCounterThread_;
        timeCounterThread_ = nullptr;
    }
    
}


/** @fn Intersection(traffictrack::IntersectionID ID)
 *  @brief constructor sets default values, and sets two lights to green and the opposing lights to red, and initializes the internal state
 *  @param ID the id of the intersection being created
 */
Intersection::Intersection(traffictrack::IntersectionID ID) : ID_(ID), hospital_(false) {
    
    northSouthIntervalTime_ = seconds(5);
    eastWestIntervalTime_ = seconds(5);
    timeSinceLastChange_ = seconds(0);
    
    lights_ = {
        new TrafficLight(LightColour::GREEN, new GreenLight()),
        new TrafficLight(LightColour::RED, new RedLight()),
        new TrafficLight(LightColour::GREEN, new GreenLight()),
        new TrafficLight(LightColour::RED, new RedLight())
    };
    
    state_ = new NorthSouthState(this->northSouthIntervalTime_);
    nextState_ = nullptr;
    timeCounterThread_ = nullptr;
    analyzer_ = new DefaultCongestionScoreAnalyzer();
    
}


/** @fn ~Intersection()
 *  @brief stops the internal looping thread if it is not already stopped and releases any dynamic memory not needed anymore
 */
Intersection::~Intersection() {
    
    stop();
    
    lock_guard<mutex> lightsGuard(lightsMutex_);
    for (TrafficLight* light : lights_) {
        if (light != nullptr)
            delete light;
    }
    
    lock_guard<mutex> stateGuard(stateMutex_);
    if (state_ != nullptr) {
        delete state_;
    }
    
}


/** @fn setHospital(bool val)
 *  @brief sets the value of the hospital, representing that the hospital is at a given intersection
 *  @param val the value of of whether the hospital is at the location or not
 */
void Intersection::setHospital(bool val) {
    hospital_ = val;
}


/** @fn setAnalyzer(AbstractCongestionScoreAnalyzer* analyzer)
 *  @brief set the analyzer strategy object for the intersection that implements the logic for how to deal with real time congestion data
 *  @param analyzer the analyzer object to use
 */
void Intersection::setAnalyzer(AbstractCongestionScoreAnalyzer* analyzer) {
    
    //free the old analyzer if it exists to prevent memory leak
    if (analyzer_ != nullptr) {
        delete analyzer_;
    }
    
    analyzer_ = analyzer;
    
}


/** @fn ID() const
 *  @brief getter for the intersection ID
 *  @return traffictrack::IntersectionID the id of the intersection
 */
traffictrack::IntersectionID Intersection::ID() const {
    return ID_;
}


/** @fn hospital() const
 *  @brief getter for the value of whether the intersection is a hospital of not
 *  @return bool whether or not the location is a hospital
 */
bool Intersection::hospital() const {
    return hospital_;
}


/** @fn northSouthcolour() const
 *  @brief returns the colour of the north and south facing lights
 *  @return traffictrack::LightColour the colour of the north/south lights
 */
traffictrack::LightColour Intersection::northSouthColour() const {
    return lights_[0]->colour();
}


/** @fn eastWestColour() const
 *  @brief returns the colour of the east and west facing lights
 *  @return traffictrack::LightColour the colour of the east/west lights
 */
traffictrack::LightColour Intersection::eastWestColour() const {
    return lights_[1]->colour();
}


/** @fn neighbors() const
 *  @brief returns the set of neighboring intersections that the intersection is connected to
 *  @return const std::set<Road>&
 */
const std::set<Road>& Intersection::neighbors() const {
    return neighbors_;
}


/** @fn addRoad(Road road)
 *  @brief adds a road to the neighbors of the intersection
 *  @param road the road to be added
 *  @return bool whether or not the road is valid or not. The road must start at the intersection that it is being added to!
 */
bool Intersection::addRoad(Road road) {
    
    if (road.firstIntersection() == this) {
        lock_guard<mutex> guard(neighborsMutex_);
        return neighbors_.insert(road).second;
    }
    else return false;
    
}


/** @fn addRoad(Intersection* first, Intersection* second, float distance, Direction direction)
 *  @brief builds a road from its parts and attempts to add it to the set of neighbors
 *  @param first the first intersection of the road
 *  @param second the second intersection connected to the road
 *  @param distance the length of the road
 *  @param direction the direction the road is traveling from first -> second
 *  @return bool whether or not the road was valid and was inserted into the set
 */
bool Intersection::addRoad(Intersection* first, Intersection* second, float distance, Direction direction) {
    
    if (first == this) {
        lock_guard<mutex> guard(neighborsMutex_);
        return neighbors_.insert(Road(first, second, distance, direction)).second;
    }
    else return false;
    
}


/** @fn changeLights()
 *  @brief changes the colour and state of the traffic light itself by delegating to its current state who will change the lights next state
 */
void Intersection::changeLights() {
    
    lock_guard<mutex> stateGuard(stateMutex_);
    state_->changeLights(this);
    delete state_;
    lock_guard<mutex> nextStateGuard(nextStateMutex_);
    state_ = nextState_;
    nextState_ = nullptr;
    lock_guard<mutex> timeCounterGuard(timeCounterMutex_);
    timeSinceLastChange_ = seconds(0);
    
}


/** @fn updateLightSchedule(std::chrono::seconds northSouthTime, std::chrono::seconds eastWestTime)
 *  @brief updates the interval times that the lights switche after
 *  @param northSouthTime the time in seconds that the north/south lights will stay green for during regular operations
 *  @param eastWestTime the time in seconds that the east/west lights will wstay green for during regular operations
 */
void Intersection::updateLightSchedule(std::chrono::seconds northSouthTime, std::chrono::seconds eastWestTime) {
    
    lock_guard<mutex> guard(intervalMutex_);
    northSouthIntervalTime_ = northSouthTime;
    eastWestIntervalTime_ = eastWestTime;
    
}


/** @fn run()
 *  @brief starts the internal thread that will automatically change the traffic lights and collect real time data
 *  @return bool whether or not the thread was started. fails if the thread was already running
 */
bool Intersection::run() {
    
    lock_guard<mutex> guard(threadMutex_);
    
    if (thread_ == nullptr) {
        thread_ = new thread(&Intersection::monitorIntersection, this);
        running_ = true;
        return true;
    }
    
    return false;
    
}

/**
 * @fn processImage
 * @brief processes the image of each traffic light of the intersection for testing purposes
 * @returns DateScorePair containing the date and the congestion score of the image
 */
traffictrack::DateScorePair Intersection::processImage(){ //verification method
    cv::String img1 = lights_.at(0)->takePhoto();
    cv::String img2 = lights_.at(1)->takePhoto();
    cv::String img3 = lights_.at(2)->takePhoto();
    cv::String img4 = lights_.at(3)->takePhoto();

    Mat i1;
    Mat i2;
    Mat i3;
    Mat i4;
    i1 = imread(img1);
    i2 = imread(img2);
    i3 = imread(img3);
    i4 = imread(img4);
    ProcessedImage data = ProcessedImage(img1,img2 ,img3, img4);//process the photos
    DateScorePair scores = data.carCount();
    cout << "North :left lane: " << scores.first <<endl;
    
    cout << "North :left lane: " << scores.second.getNorth().at(0)<<endl;
    cout << "North: straight lane: " << scores.second.getNorth().at(1) <<endl;
    cout << "North :right lane: " << scores.second.getNorth().at(2) <<endl;
    cv::imshow("Display NorthImage", i1);
    int k = waitKey(0);
    destroyWindow("Display NorthImage");

    cout << "South :left lane: " << scores.second.getSouth().at(0)<<endl;
    cout << "South: straight lane: " << scores.second.getSouth().at(1) <<endl;
    cout << "South :right lane: " << scores.second.getSouth().at(2) <<endl;
    cv::imshow("Display SouthImage", i2);
    k = waitKey(0);
    destroyWindow("Display SouthImage");

    cout << "East :left lane: " << scores.second.getEast().at(0)<<endl;
    cout << "East: straight lane: " << scores.second.getEast().at(1) <<endl;
    cout << "East :right lane: " << scores.second.getEast().at(2) <<endl;
    cv::imshow("Display EastImage", i3);
    k = waitKey(0);
    destroyWindow("Display EastImage");

    cout << "West :left lane: " << scores.second.getWest().at(0)<<endl;
    cout << "West: straight lane: " << scores.second.getWest().at(1) <<endl;
    cout << "West :right lane: " << scores.second.getWest().at(2) <<endl;
    cv::imshow("Display WestImage", i4);
    k = waitKey(0);
    destroyWindow("Display WestImage");
    return scores;

}

/**
 * @fn getLights
 * @brief getter for lights
 * @returns list of TrafficLights
 */
std::vector<TrafficLight*> Intersection::getLights(){
    return lights_;
}

