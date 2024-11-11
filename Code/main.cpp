//
//  main.cpp
//  TraffikTrak

//program setup
#include <iostream>
#include <sstream>

//Test case 1
#include <unordered_map>
#include <ctime>
#include "AbstractTrafficDatabase.hpp"
#include "TrafficDatabase.hpp"
#include "IntersectionID.h"
#include "TimeBlock.h"
#include "PredictionLevel.h"
#include "TempFile.h"
#include "DatabaseLocation.h"
#include "CongestionScore.hpp"
#include "Intersection.hpp"
#include "DateScorePair.hpp"
#include "AverageCongestionScores.hpp"
#include "AbstractTrafficLightScheduler.hpp"
#include "DefaultTrafficLightScheduler.hpp"
#include "LightSchedule.hpp"

//Test case 2
#include "AbstractMapFileParser.hpp"
#include "QuickMapFileParser.hpp"
#include "AbstractPathFinder.hpp"
#include "UniformCostSearch.hpp"

//Test case 3
#include "Controller.hpp"

//Test case 4
#include "Yolo.hpp"
#include "ProcessedImage.hpp"
#include "RandomPhotoTaker.hpp"
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <time.h>


using namespace cv;
using namespace dnn;
using namespace std;
using namespace std::chrono;
using namespace traffictrack;


int main(int argc, const char * argv[]) {
    
    
    if (argc != 2) {
        cout << "invalid arguments" << endl;
        exit(1);
    }
    
    string testCase = argv[1];
    istringstream ss(testCase);
    int testCaseNumber = 0;
    
    if (!(ss >> testCaseNumber)) {
        cout << "invalid test case" << endl;
        exit(2);
    }
    
    /*  Test 1: test the database and traffic light scheduling algorithm
     *  create a database, log information to the database, then calculate the averages
     *  print the results to the screen
     *
     *  create a single intersection for illustration purposes
     */
    if (testCaseNumber == 1) {
        
        cout << "====================================================" << endl;
        cout << "   Test Case 1: Database and Schedule Prediction" << endl;
        cout << "====================================================" << endl;
        
        /*
         Expected Results:
            One time block for each day
            the north/south and east/west time blocks will be the same, because the given traffic levels are specified to be equal
            Default days will have a time of 150 seconds for each interval.
            Some days, including the day of testing will have intervals of 60 seconds. This is because the specified score is large enough to trigger
                "high" congestion. This results in a different switching time. Times are based on real system time, so the data will be logged to the
                file based on the day. This is why some days will be default and some will use the data that was logged during the test.
            The output files are visible in the project directory in the directory "TestDatabase", where you can see the data that was used to calculate
                the scores.
            The prediction level is set to 1, so the "average" will only be calculated off of the last input into the file.
         
         Example output:
         
         Schedule for intersection 123:
         Day: Fri | TimeBlock: 0 | North/South Interval Time: 150 | East/West Interval Time: 150
         Day: Mon | TimeBlock: 0 | North/South Interval Time: 150 | East/West Interval Time: 150
         Day: Sat | TimeBlock: 0 | North/South Interval Time: 150 | East/West Interval Time: 150
         Day: Sun | TimeBlock: 0 | North/South Interval Time: 150 | East/West Interval Time: 150
         Day: Thu | TimeBlock: 0 | North/South Interval Time: 150 | East/West Interval Time: 150
         Day: Tue | TimeBlock: 0 | North/South Interval Time: 150 | East/West Interval Time: 150
         Day: Wed | TimeBlock: 0 | North/South Interval Time: 60 | East/West Interval Time: 60
         
         Note: this test was run on wednesday, so the CongestionScore below was recorded in the wednesday file under time block 0, for interseciton 123.
               This is why the time is 60 seconds, while the others (with no available data) are 150 seconds (assumes low congestion)
         
         */
        
        //create a map of intersections with only 1 intersection with ID 123
        unordered_map<IntersectionID, Intersection*> intersections;
        Intersection* testIntersection = new Intersection(IntersectionID(123));
        intersections[testIntersection->ID()] = testIntersection;
        
        AbstractTrafficDatabase::l_seconds timeBetweenSaves = seconds(20);
        TimeBlock numberOfTimeBlocks(1);
        TempFile temp("temp_file");
        DatabaseLocation location("TestDatabase");
        PredictionLevel level(1);
        
        AbstractTrafficDatabase* database = TrafficDatabase::create(intersections, timeBetweenSaves, numberOfTimeBlocks, temp, location, level);
        
        CongestionScore score;
        score.setNorth(100, 100, 100);
        score.setEast(100, 100, 100);
        score.setSouth(100, 100, 100);
        score.setWest(100, 100, 100);
        
        time_t now = time(NULL);
        string time = ctime(&now);
        
        DateScorePair p(time, score);
        
        database->log(testIntersection->ID(), p);
        
        auto averageScores = database->calculateAverages();
        
        AbstractTrafficLightScheduler* scheduler = new DefaultTrafficLightScheduler();
        
        auto schedules = scheduler->schedule(averageScores);
        
        for (auto it = schedules.begin(); it != schedules.end(); ++it) {
            
            IntersectionID id = it->first;
            LightSchedule schedule = it->second;
            
            cout << "Schedule for intersection " << id << ":" << endl;
            
            for (auto info : schedule) {
                
                string day = info.first.first;
                int timeblock = info.first.second;
                seconds northSouthIntervalTime = info.second.first;
                seconds eastWestIntervalTime = info.second.second;
                
                cout << "   Day: " << day << " | TimeBlock: " << timeblock << " | North/South Interval Time: " << northSouthIntervalTime.count() << " | East/West Interval Time: " << eastWestIntervalTime.count() << endl;
                
            }
            
        }
        
        delete database;
        delete testIntersection;
        delete scheduler;
        
    }
    
    /*  Test 2: create a map and then run the shortest path algorithm on the map. use Map.txt as the input file
     *          and test the algorithm from node 3 to the default final state = intersection 0
     *
     *  prints the final path and the path length
     */
    else if (testCaseNumber == 2) {
        
        cout << "====================================================" << endl;
        cout << "        Test Case 2: Emergency Vehicle Route" << endl;
        cout << "====================================================" << endl;
        
        /*
         Expected Results:
            the expected output is the final path: 3 -> 4 -> 1 -> 0
            the total path distance = 30
         
         This assumes that we are calculating the shortest path from intersection 3 to intersection 0
         
         sample output:
         Path: 3 -> 4 -> 1 -> 0
         Total Path Distance: 30
         
         */
        
        
        unordered_map<IntersectionID, Intersection*> intersections;
        
        AbstractMapFileParser* parser = new QuickMapFileParser();
        parser->parse("map.txt", intersections);
        
        AbstractPathFinder* pathFinder = new UniformCostSearch();
        auto path = pathFinder->search(intersections[IntersectionID(3)], intersections);
        float pathDistance = path.second;
        
        cout << "Path: ";
        
        for (auto it = path.first.begin(); it != path.first.end(); ++it) {
            
            IntersectionID id = (*it)->ID();
            cout << id;
            
            if (it+1 != path.first.end()) {
                cout << " -> ";
            }
            
        }
        cout << endl;
        cout << "Total Path Distance: " << pathDistance << endl;
        
        for (auto& element : intersections) {
            delete element.second;
        }
        
        delete parser;
        delete pathFinder;
        
    }
    
    /*  Test 3: sets up and runs the program independently. press enter to end the program
     *
     */
    else if (testCaseNumber == 3) {
        
        cout << "====================================================" << endl;
        cout << "             Test Case 3: Run Program" << endl;
        cout << "====================================================" << endl;
        
        /*
         Expected output
            the program runs in a loop and doesn't print much to the console to show what is happening. There would be a lot to print
            and it would be difficult to read and comprehend. However, it does show when the database writes to the files.
         
         */
        
        
        Controller* controller = Controller::instance();
        if (controller != nullptr) {
            
            const char** args = (const char**) malloc(sizeof(char*)*11);
            args[0] = strdup(argv[0]);
            args[1] = strdup("-c");
            args[2] = strdup("config.txt");
            args[3] = strdup("-m");
            args[4] = strdup("map.txt");
            args[5] = strdup("-cp");
            args[6] = strdup("QuickDatabaseConfigParser");
            args[7] = strdup("-mp");
            args[8] = strdup("QuickMapFileParser");
            args[9] = strdup("-sa");
            args[10] = strdup("UniformCostSearch");
            
            if (controller->initialize(11, args)) {
                controller->run();
                getchar();
                controller->stop();
            }
            
            delete controller;
            for (int i = 0; i < 11; i++) {
                delete args[i];
            }
            delete args;
            
        }/**/
        
    }


    else if (testCaseNumber == 4) {
        cout << "====================================================" << endl;
        cout << "             Test Case 4: Linking Computer Vision with Controller" << endl;
        cout << "====================================================" << endl;
        
        const char** args = (const char**) malloc(sizeof(char*)*11);
        args[0] = strdup(argv[0]);
        args[1] = strdup("-c");
        args[2] = strdup("config.txt");
        args[3] = strdup("-m");
        args[4] = strdup("map.txt");
        args[5] = strdup("-cp");
        args[6] = strdup("QuickDatabaseConfigParser");
        args[7] = strdup("-mp");
        args[8] = strdup("QuickMapFileParser");
        args[9] = strdup("-sa");
        args[10] = strdup("UniformCostSearch");

        Controller* controller = Controller::instance();
        if (controller != nullptr) {
    
            if (controller->initialize(11, args)) {
                controller->testController();
            }
            delete controller; 
        }
        cout << "end of test" << endl;
        for (int i = 0; i < 11; i++) {
            delete args[i];
        }
        delete args;
            
    }

    else if (testCaseNumber == 5) {
        cout << "====================================================" << endl;
        cout << "             Test Case 5: Testing Computer Vision" << endl;
        cout << "====================================================" << endl;
        
        Mat img1;
        Mat img2;
        Mat img3;
        Mat img4;

        cout << "This test program processes multiple images and returns a congestion score for each one" << endl;
        srand(time(NULL));
        RandomPhotoTaker img;
        String northImage = img.takePhoto(); //retrieves random image
        String southImage = img.takePhoto();
        String eastImage = img.takePhoto();
        String westImage = img.takePhoto();
        img1 = imread(northImage); //read file
        img2 = imread(southImage);
        img3 = imread(eastImage);
        img4 = imread(westImage);
        
        ProcessedImage data = ProcessedImage(northImage,southImage ,eastImage, westImage); //process images with computer vision
        DateScorePair scores = data.carCount(); //get congestion scores and date

        // DISPLAY REUSULTS AS OUTPUT

        cout << "Date stamp" << scores.first <<endl; //DATE STAMP
        cout << endl;

        //CONGESTION SCORES
        cout << "North :left lane: " << scores.second.getNorth().at(0)<<endl;
        cout << "North: straight lane: " << scores.second.getNorth().at(1) <<endl;
        cout << "North :right lane: " << scores.second.getNorth().at(2) <<endl;
        imshow("Display NorthImage", img1);
        int k = waitKey(0);
        destroyWindow("Display NorthImage");

        cout << "South :left lane: " << scores.second.getSouth().at(0)<<endl;
        cout << "South: straight lane: " << scores.second.getSouth().at(1) <<endl;
        cout << "South :right lane: " << scores.second.getSouth().at(2) <<endl;
        imshow("Display SouthImage", img2);
        k = waitKey(0);
        destroyWindow("Display SouthImage");

        cout << "East :left lane: " << scores.second.getEast().at(0)<<endl;
        cout << "East: straight lane: " << scores.second.getEast().at(1) <<endl;
        cout << "East :right lane: " << scores.second.getEast().at(2) <<endl;
        imshow("Display EastImage", img3);
        k = waitKey(0);
        destroyWindow("Display EastImage");

        cout << "West :left lane: " << scores.second.getWest().at(0)<<endl;
        cout << "West: straight lane: " << scores.second.getWest().at(1) <<endl;
        cout << "West :right lane: " << scores.second.getWest().at(2) <<endl;
        imshow("Display WestImage", img4);
        k = waitKey(0);
        destroyWindow("Display WestImage");
    }
        
    return 0;
    
}/**/
