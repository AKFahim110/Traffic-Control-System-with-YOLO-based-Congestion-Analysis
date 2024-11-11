//
//  TrafficDatabase.cpp
//  TraffikTrak

#include <string>
#include <unordered_map>
#include <mutex>
#include <unistd.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <future>
#include <utility>
#include <sstream>
#include "IntersectionDatabase.hpp"
#include "TrafficDatabase.hpp"
#include "TrafficDatabaseAccessException.hpp"
#include "AverageCongestionScores.hpp"
#include "IntersectionID.h"
#include "DateScorePair.hpp"

using namespace std;
using namespace traffictrack;


#include <iostream>
using std::endl;
using std::cout;

#include <map>
using std::map;
#include "CongestionScore.hpp"
using traffictrack::CongestionScore;


//initialize static variables
TrafficDatabase* TrafficDatabase::instance_ = nullptr;
std::mutex TrafficDatabase::staticMutex_;


/** @fn TrafficDatabase(const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections, l_seconds time_between_saves, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level)
 *  @brief private constructor that initializes the internal parameters
 *  @param intersections the intersections, such that each one needs its own intersection database
 *  @param time_between_saves the time inteveral between when the database automatically saves the caches to files
 *  @param number_of_time_blocks the number of time blocks that the day is separated into, and each one will have its own history data
 *  @param tmp_filename the name of the temp file that will be used for intermediate IO manipulation
 *  @param database_directory the location of the database
 *  @param prediction_level the number of data points that will be used to calculate the averages
 */
TrafficDatabase::TrafficDatabase(const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections, l_seconds time_between_saves, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level) : active_(true), timeBetweenSaves_(time_between_saves) {
    
    std::string current_directory = "\"" + getCurrentDirectory() + "\"";
    system(("mkdir " + current_directory + "/" + "\"" + static_cast<std::string>(database_directory) + "\"").c_str());
    
    for (std::unordered_map<traffictrack::IntersectionID, Intersection*>::const_iterator it = intersections.begin(); it != intersections.end(); ++it) {
        IntersectionDatabase* database = IntersectionDatabase::create(it->first, number_of_time_blocks, tmp_filename + traffictrack::TempFile(std::to_string(it->first)), database_directory, prediction_level);
        if (database != nullptr) {
            intersectionDatabases_.insert( { it->first, database } );
        }
        else {
            std::cerr << "failure to create database " << it->first << std::endl;
        }
    }
    
}


/** @fn deactivate()
 *  @brief deactivates the database so no more logging can happen
 */
void TrafficDatabase::deactivate() {
    active_ = false;
}


/** @fn getCurrentDirectory() const
 *  @brief gets the current working directory
 *  @return std::string working directory
 */
std::string TrafficDatabase::getCurrentDirectory() const {
    
    char buff[FILENAME_MAX];
    getcwd( buff, FILENAME_MAX );
    string current_working_dir(buff);
    return current_working_dir;
    
}


/** @fn periodicallyWriteCache()
 *  @brief looping internal thread that writes to cache to files every so often
 */
void TrafficDatabase::periodicallyWriteCache() {
    
    waitFor(timeBetweenSaves_);
    
    unique_lock<mutex> lock(databaseMutex_);
    
    while (stopRequested() == false) {
        
        //write to all files simultaneously using threads
        vector<thread*> threads;
        
        for (unordered_map<IntersectionID, IntersectionDatabase*>::iterator it = intersectionDatabases_.begin(); it != intersectionDatabases_.end(); ++it) {
            thread* thread_1 = new thread(&IntersectionDatabase::writeCache, it->second);
            threads.push_back(thread_1);
        }
        
        //join the threads
        for (thread* thread_1 : threads) {
            if (thread_1->joinable()) {
                thread_1->join();
                delete thread_1;
            }
        }
        
        //release the lock and wait
        lock.unlock();
        waitFor(timeBetweenSaves_);
        lock.lock();
        
    }
        
}


/** @fn create(const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections, l_seconds time_between_saves, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level)
 *  @brief public static method to create the singleton method
 *  @param intersections the intersections, such that each one needs its own intersection database
 *  @param time_between_saves the time inteveral between when the database automatically saves the caches to files
 *  @param number_of_time_blocks the number of time blocks that the day is separated into, and each one will have its own history data
 *  @param tmp_filename the name of the temp file that will be used for intermediate IO manipulation
 *  @param database_directory the location of the database
 *  @param prediction_level the number of data points that will be used to calculate the averages
 *  @return TrafficDatabase* returns pointer to the singleton
 */
TrafficDatabase* TrafficDatabase::create(const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections, l_seconds time_between_saves, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level) {
    
    std::lock_guard<std::mutex> guard(staticMutex_);
    
    if (instance_ == nullptr) {
        instance_ = new TrafficDatabase(intersections, time_between_saves, number_of_time_blocks, tmp_filename, database_directory, prediction_level);
        return instance_;
    }
    else return nullptr;
    
}


/** @fn instance()
 *  @brief returns the pointer to the singleton
 *  @return TrafficDatabase* pointer to singleton
 */
TrafficDatabase* TrafficDatabase::instance() {
    
    lock_guard<mutex> guard(staticMutex_);
    return instance_;
    
}


/** @fn ~TrafficDatabase()
 *  @brief closes the data base and frees all the individual intersection databases
 */
TrafficDatabase::~TrafficDatabase() {
    
    close();
    
    lock_guard<mutex> guard(databaseMutex_);
        
    for (auto it = intersectionDatabases_.begin(); it != intersectionDatabases_.end(); ++it) {
        
        if (it->second != nullptr) {
            delete it->second;
        }
        
    }
    
}


/** @fn log(traffictrack::IntersectionID intersection_ID, traffictrack::DateScorePair date_score_pair)
 *  @brief logs the data to the specified intersection database
 *  @param intersection_ID the ID of the intersection logging the data
 *  @param date_score_pair the data to log
 *  @return bool whether or not the log was successful
 */
bool TrafficDatabase::log(traffictrack::IntersectionID intersection_ID, traffictrack::DateScorePair date_score_pair) {
    
    if (!active()) {
        throw TrafficDatabaseAccessException("called log on inactive database");
    }
    
    string date = date_score_pair.first;
    istringstream ss(date);
    string formatted_date;
    getline(ss, formatted_date);
    
    DateScorePair formatted_pair(formatted_date, date_score_pair.second);
    
    lock_guard<mutex> guard(databaseMutex_);
    
    unordered_map<IntersectionID, IntersectionDatabase*>::iterator node = intersectionDatabases_.find(intersection_ID);
    
    bool success = false;
    
    if (node != intersectionDatabases_.end()) {
        if (node->second->log(formatted_pair)) {
            success = true;
        }
    }
    
    return success;
    
}


/** @fn calculateAverages()
 *  @brief calculates the average congestion scores for each time block across all intersections
 *  @return const std::unordered_map<traffictrack::IntersectionID, traffictrack::AverageCongestionScores> the average congestion scores
 */
const std::unordered_map<traffictrack::IntersectionID, traffictrack::AverageCongestionScores> TrafficDatabase::calculateAverages() {
    
    if (!active()) {
        throw TrafficDatabaseAccessException("calculating averages on inactive database");
    }
    
    lock_guard<mutex> guard(databaseMutex_);
        
    vector<thread*> threads;
    unordered_map<IntersectionID, future<AverageCongestionScores>> return_values;
    
    //calls calculateAverage() for each intersection database asychronously
    for (unordered_map<IntersectionID, IntersectionDatabase*>::const_iterator it = intersectionDatabases_.begin(); it != intersectionDatabases_.end(); ++it) {
        
        promise<AverageCongestionScores> p;
        future<AverageCongestionScores> f = p.get_future();
        
        return_values.insert( { it->first, std::move(f) } );
        
        thread* thread_1 = new thread(&IntersectionDatabase::calculateAverages, it->second, std::move(p));
        threads.push_back(thread_1);
        
        thread_1->join();
        
    }
    
    //join the threads
    for (thread* thread_1 : threads) {
        
        if (thread_1->joinable()) {
            thread_1->join();
            delete thread_1;
        }
        
    }
    
    //move the return values from the threads into an object and return it
    unordered_map<IntersectionID, AverageCongestionScores> averages;
    
    for (auto it = return_values.begin(); it != return_values.end(); ++it) {
        
        AverageCongestionScores average = it->second.get();
        averages.insert( { it->first, average } );
        
    }
    
    return averages;
    
}


/** @fn active() const
 *  @brief getter for active member.
 *  @return bool whether or not the database is active or not
 */
bool TrafficDatabase::active() const {
    return active_;
}


/** @fn close()
 *  @brief stops the internal thread and handles any releasing of memory required to close the database
 */
void TrafficDatabase::close() {
    deactivate();
    stop();
}


/** @fn run()
 *  @brief starts the internal looping thread
 *  @return bool whether or not the thread was started. fails if the thread was already running
 */
bool TrafficDatabase::run() {
    
    lock_guard<mutex> guard(threadMutex_);
    
    if (thread_ != nullptr || !active()) {
        return false;
    }
    
    thread_ = new thread(&TrafficDatabase::periodicallyWriteCache, this);
    
    running_ = true;
    
    return true;
    
}


