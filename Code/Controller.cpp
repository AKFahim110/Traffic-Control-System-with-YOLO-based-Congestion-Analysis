//
//  Controller.cpp
//  TraffikTrak

#include <map>
#include <string>
#include <ctime>
#include <utility>
#include <mutex>
#include <thread>
#include "Controller.hpp"
#include "ArgumentInterpreter.hpp"
#include "Intersection.hpp"
#include "AbstractMapFileParser.hpp"
#include "AbstractDatabaseConfigParser.hpp"
#include "QuickMapFileParser.hpp"
#include "ExplicitMapFileParser.hpp"
#include "QuickDatabaseConfigParser.hpp"
#include "IOException.hpp"
#include "FormatException.hpp"
#include "TrafficDatabaseAccessException.hpp"
#include "DateScorePair.hpp"
#include "IntersectionID.h"
#include "UniformCostSearch.hpp"
#include "LightSchedule.hpp"
#include "AbstractTrafficLightScheduler.hpp"
#include "DefaultTrafficLightScheduler.hpp"
#include "Direction.h"
#include "LightColour.h"
#include "AbstractTrafficDatabase.hpp"
#include "DayTimeBlockPair.hpp"
#include "AbstractPathFinder.hpp"
#include "UniformCostSearch.hpp"
#include "AverageCongestionScores.hpp"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <ProcessedImage.hpp>

using namespace std;
using namespace traffictrack;
using namespace std::chrono;


Controller* Controller::instance_ = nullptr;


/** @fn Controller()
 *  @brief default constructor initializes
 */
Controller::Controller() {
    
    database_ = nullptr;
    pathFinder_ = nullptr;
    timeBetweenScheduleUpdates_ = seconds(60); //aribtrary value for proof of concept
    timeSinceLastUpdate_ = seconds(60); //arbitrary value
    timeCounterThread_ = nullptr;
    scheduler = new DefaultTrafficLightScheduler();
    
}


/** @fn handleDataLogRequest(traffictrack::IntersectionID ID, traffictrack::DateScorePair score)
 *  @brief method that handles when intersection wants to log congestion score in the database
 *  @param ID the ID of the intersection that the score is for
 *  @param score the information that will be logged in the database
 */
void Controller::handleDataLogRequest(traffictrack::IntersectionID ID, traffictrack::DateScorePair score) {
    
    if (logThreads_.size() >= maxLogThreads_) {
        clearLogThreads();
    }
    
    thread* my_thread = new thread(&AbstractTrafficDatabase::log, database_, ID, score);
    logThreads_.push_back(my_thread);
    
}


/** @fn handleEmergencyAlert(ID)
 *  @brief method that handles when an emergency vehicle is detected
 *  @param ID the id of the intersection that the emergency vehicle was detected at
 */
void Controller::handleEmergencyAlert(traffictrack::IntersectionID ID) {
    
    pair<vector<Intersection*>, float> path = pathFinder_->search(intersections_[ID], intersections_);
    
    if (path.second > 0) {
        
        Intersection* previous = path.first[0];
        for (auto it = path.first.begin()+1; it != path.first.end(); ++it) {
            
            Direction direction = Direction::DEFAULT;
            
            for (Road r : previous->neighbors()) {
                
                if (r.secondIntersection() == (*it)) {
                    direction = r.direction();
                    break;
                }
                
            }
            
            if (direction != Direction::DEFAULT) {
                
                LightColour colour;
                if (direction == Direction::NORTH || direction == Direction::SOUTH) {
                    colour = (*it)->northSouthColour();
                }
                else {
                    colour = (*it)->eastWestColour();
                }
                
                if (colour != LightColour::GREEN) {
                    (*it)->changeLights();
                }
                
            }
            
            previous = (*it);
            
        }
        
    }
    
}


/** @fn clearLogThreads()
 *  @brief joins all the current threads running in the database and resets the vector to be empty, since we need to keep track of loose threads for safety
 */
void Controller::clearLogThreads() {
    
    for (thread* t : logThreads_) {
        if (t->joinable()) {
            t->join();
            delete t;
        }
    }
    logThreads_.clear();
    
}


/** @fn manageRequests()
 *  @brief internal looping thread that pulls information from the queue and handles events accordingly
 */
void Controller::manageRequests() {
    
    unique_lock<mutex> threadLock(timeCounterThreadMutex_);
    if (timeCounterThread_ == nullptr) {
        timeCounterThread_ = new thread (&Controller::countTimeSinceLastUpdate, this);
    }
    threadLock.unlock();
    
    while (!stopRequested()) {
        
        {
            unique_lock<mutex> timeCounterLock(timeCounterMutex_);
            if (timeSinceLastUpdate_ >= timeBetweenScheduleUpdates_) {
                timeCounterLock.unlock();
                updateSchedules();
            }
        }
        
        unique_lock<mutex> emergencyLock(emergencyAlertMutex_);
        if (!emergencyVehicleAlerts_.empty()) {
            
            IntersectionID ID = emergencyVehicleAlerts_.front();
            emergencyVehicleAlerts_.pop();
            emergencyLock.unlock();
            
            handleEmergencyAlert(ID);
            
        }
        else {
            
            emergencyLock.unlock();
            unique_lock<mutex> dataRequestsLock(dataRequestsMutex_);
            if (!dataRequests_.empty()) {
                
                pair<IntersectionID, DateScorePair> data = dataRequests_.front();
                dataRequests_.pop();
                dataRequestsLock.unlock();
                
                handleDataLogRequest(data.first, data.second);
                
            }
            
        }
        
    }

        
    threadLock.lock();
    if (timeCounterThread_->joinable()) {
        timeCounterThread_->join();
        delete timeCounterThread_;
        timeCounterThread_ = nullptr;
    }
    
}


/** @fn countTimeSinceLastUpdate()
 *  @brief counts the number of seconds since the last time that the traffic light schedules
 */
void Controller::countTimeSinceLastUpdate() {
    
    while(!stopRequested()) {
        
        waitFor(seconds(1));
        lock_guard<mutex> guard(timeCounterMutex_);
        timeSinceLastUpdate_++;
        
    }
    
}


/** @fn updateSchedules()
 *  @brief calculates new schedules for each intersection and updates their interval times based on the historical data
 */
void Controller::updateSchedules() {
    
    AbstractTrafficLightScheduler::IntersectionSchedules schedules = scheduler->schedule(database_->calculateAverages());
    for (auto it = schedules.begin(); it != schedules.end(); ++it) {
        
        IntersectionID id = it->first;
        LightSchedule intersection_schedule = it->second;
        DayTimeBlockPair p("Mon", 1);
        intersections_[id]->updateLightSchedule(intersection_schedule[p].first, intersection_schedule[p].second);
        
    }
    
    lock_guard<mutex> guard(timeCounterMutex_);
    timeSinceLastUpdate_ = seconds(0);
    
}


/** @fn instance()
 *  @brief returns the singleton instance of the controller class
 *  @return Controller* pointer to the sole instance of the class
 */
Controller* Controller::instance() {
    if (instance_ == nullptr) {
        instance_ = new Controller();
    }
    return instance_;
}


/** @fn ~Controller()
 *  @brief stops all threads running in the intersections and database, and frees all memory
 */
Controller::~Controller() {
    
    for (auto it = intersections_.begin(); it != intersections_.end(); ++it) {
        it->second->stop();
    }
    
    for (auto it = intersections_.begin(); it != intersections_.end(); ++it) {
        delete it->second;
    }
    
    if (database_ != nullptr) {
        database_->close();
        delete database_;
    }
    
}


/** @fn initialize(int argc, const char* argv[])
 *  @brief initializes the controller class strategies based on command line arguments
 *  @param argc the number of arguments
 *  @param argv the arguments as strings
 *  @return bool whether of not the initialization was successful or threw an error
 */
bool Controller::initialize(int argc, const char* argv[]) {
    
    bool successful_initialization = true;
    
    std::map<string, AbstractMapFileParser*> mapParser = {
        {"QuickMapFileParser", new QuickMapFileParser()},
        {"ExplicitMapFileParser", new ExplicitMapFileParser()}
    };
    
    std::map<string, AbstractDatabaseConfigParser*> configParser = {
        {"QuickDatabaseConfigParser", new QuickDatabaseConfigParser()}
    };
    
    std::map<string, AbstractPathFinder*> pathFinders = {
        {"UniformCostSearch", new UniformCostSearch()}
    };
    
    try {
        ArgumentInterpreter interpreter;
        interpreter.interpret(argc, argv);
        
        mapParser[interpreter.mapParser()]->parse(interpreter.map(), intersections_);
        database_ = configParser[interpreter.configParser()]->parse(interpreter.config(), intersections_);
        pathFinder_ = pathFinders[interpreter.searchAlgorithm()];
        
        database_->run();
        
    }
    catch (IOException& e) {
        successful_initialization = false;
        std::cerr << e.what() << std::endl;
    }
    catch (FormatException& e) {
        successful_initialization = false;
        std::cerr << e.what() << std::endl;
    }
    catch (TrafficDatabaseAccessException& e) {
        successful_initialization = false;
        std::cerr << e.what() << std::endl;
    }
    
    delete mapParser["QuickMapFileParser"];
    delete mapParser["ExplicitMapFileParser"];
    delete configParser["QuickDatabaseConfigParser"];
    
    return successful_initialization;
    
}


/** @fn logScore(traffictrack::IntersectionID ID, traffictrack::DateScorePair score)
 *  @brief puts a request to log a congestion score in the queue
 *  @param ID the id of the intersection requesting the operation
 *  @param score the data to be logged in the database
 */
void Controller::logScore(traffictrack::IntersectionID ID, traffictrack::DateScorePair score) {
    
    lock_guard<mutex> requestsGuard(dataRequestsMutex_);
    dataRequests_.push(make_pair(ID, score));
    
}


/** @fn alertEmergencyVehicle(traffictrack::IntersectionID ID)
 *  @brief puts an emergency vehicle alert in the queue to be processed
 *  @param ID the location of the intersection where the emergency vehicle is located
 */
void Controller::alertEmergencyVehicle(traffictrack::IntersectionID ID) {
    
    lock_guard<mutex> alertGuard(emergencyAlertMutex_);
    emergencyVehicleAlerts_.push(ID);
    
}


/** @fn run()
 *  @brief initializes the thread that will control the object
 *  @return bool returns whether the run was successful. fails if the thread is already running
 */
bool Controller::run() {
    
    lock_guard<mutex> threadGuard(threadMutex_);
    
    if (thread_ == nullptr) {
        thread_ = new thread(&Controller::manageRequests, this);
        running_ = true;
        
        for (auto it = intersections_.begin(); it != intersections_.end(); ++it) {
            it->second->run();
        }
        
        return true;
    }
    
    return false;
    
}



/**
 * @fn testController2
 * @brief this method checks if the intersections are able to retreive and process images with computer Vision
 * 
 * This method iterates through the intersections, retrieves and processes 4 images (for each traffic light) and returns the congestion score
 * for each traffic light and also displays the image that was processed for each. 
 * @returns void
 */
void Controller::testController() {
    cout << "This test program processes multiple images and returns a congestion score for each one" << endl;
    srand(time(NULL));
    cv::String img1;
    cv::String img2;
    cv::String img3;
    cv::String img4;
    for (auto it = intersections_.begin(); it != intersections_.end(); ++it) {
        it->second->processImage();
    }
}