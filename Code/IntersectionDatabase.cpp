//
//  IntersectionDatabase.cpp
//  TraffikTrak


#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <map>
#include <exception>
#include <iostream>
#include <mutex>
#include <sstream>
#include <future>
#include "IntersectionDatabase.hpp"
#include "DateScorePair.hpp"
#include "DayTimeBlockPair.hpp"
#include "TrafficDatabaseAccessException.hpp"
#include "IOException.hpp"
#include "FormatException.hpp"
#include "Cache.hpp"
#include "AverageCongestionScores.hpp"
#include "CongestionScore.hpp"

using namespace std;
using namespace traffictrack;


//initialize static variables
std::unordered_map<int, IntersectionDatabase*> IntersectionDatabase::instance_;
std::mutex IntersectionDatabase::staticMutex_;


/** @fn IntersectionDatabase(traffictrack::IntersectionID intersection_ID, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level)
 *  @brief private constructor sets the database parameters and loads the caches with data from the saved files
 *  @param intersection_ID the id of the intersection that the database is for
 *  @param number_of_time_blocks the number of blocks that the day is being split into
 *  @param tmp_filename the name of the temp file being used to manipulate file data
 *  @param database_directory the directory that the database files will be stored in
 *  @param prediction_level the number of data points that will be used to calculate the average
 */
IntersectionDatabase::IntersectionDatabase(traffictrack::IntersectionID intersection_ID, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level) : ID_(intersection_ID), numberOfTimeBlocks_(number_of_time_blocks), numberOfHoursPerBlock_(24/number_of_time_blocks), tmpfilename_(tmp_filename), predictionLevel_(prediction_level), databaseDirectory_(getCurrentDirectory() + "/" + static_cast<std::string>(database_directory)), daysOfTheWeek_({ "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" }) {
    
    //set up the caches - one for each valid DayTimeBlockPair
    for (std::string day : daysOfTheWeek_) {
        for (int i = 0; i < numberOfTimeBlocks_; i++) {
            DayTimeBlockPair p(day, i);
            data_.insert( { p, Cache() } );
        }
    }
    
    //set up the caches with initial values from the files so that averages can be calculated quickly without IO
    setupCaches();
    
}


/** @fn setupCaches()
 *  @brief reads from the written data files and loads the last prediction_level number of data points so that averages can be easily calculated without IO
 */
void IntersectionDatabase::setupCaches() {
    
    //iterate through all valid DayTimeBlockPairs, since there is one file for each pair
    for (auto& it : data_) {
        
        string filename = generateFilename(it.first);
        filename = "\"" + appendPath(databaseDirectory_, filename) + "\"";
        string temp = appendPath(databaseDirectory_, tmpfilename_);
        int lines_to_read = predictionLevel_*2; //each entry is split into 2 lines: timestamp and the data
        
        ifstream inFile;
        try {
            
            //put the last values of the file into a temp file so it can be read start to finish
            string command = "tail -n " + std::to_string(lines_to_read) + " " + filename + " >> " + "\"" + temp + "\"";
            system(command.c_str());
            
            inFile.open(temp);
            
            if (inFile.is_open()) {
                
                //read the file in groups of 2 to get the date and the congestion scores
                
                string date;
                string congestion_scores;
                while (getline(inFile, date)) {
                    
                    if (getline(inFile, congestion_scores)) {
                        
                        istringstream ss(congestion_scores);
                        
                        //separate the data line into each of its 12 componenets and recreate the CongestionScore
                        
                        CongestionScore score;
                        
                        int left_turning = 0;
                        int straight = 0;
                        int right_turning = 0;
                        
                        //read into north bound traffic
                        if ((ss >> left_turning) && (ss >> straight) && (ss >> right_turning)) {
                            score.setNorth(left_turning, straight, right_turning);
                        }
                        
                        //read into east bound traffic
                        if ((ss >> left_turning) && (ss >> straight) && (ss >> right_turning)) {
                            score.setEast(left_turning, straight, right_turning);
                        }
                        
                        //read into south bound traffic
                        if ((ss >> left_turning) && (ss >> straight) && (ss >> right_turning)) {
                            score.setSouth(left_turning, straight, right_turning);
                        }
                        
                        //read into west bound traffic
                        if ((ss >> left_turning) && (ss >> straight) && (ss >> right_turning)) {
                            score.setWest(left_turning, straight, right_turning);
                        }
                        
                        it.second.add(DateScorePair(date, score), true);
                        
                    }
                    
                }
                
            }
            else {
                cerr << "file " << filename << " not found" << endl;
            }
            
        }
        catch (IOException& e) {
            cerr << e.what() << endl;
        }
        
        if (inFile.is_open()) {
            inFile.close();
        }
        if (remove(temp.c_str()) != 0) {
            cerr << temp << " not deleted" << endl;
        }
        
    }
    
}


/** @fn generateFilename(DayTimeBlockPair p) const
 *  @brief puts together the different pieces of information to generate the name of the file where the data is saved
 *  @param p the DayTimeBlockPair that is used to extract the information necessary to generate the filename
 *  @return std::string the name of the file
 */
std::string IntersectionDatabase::generateFilename(DayTimeBlockPair p) const {
    
    string day = p.first;
    int time_block = p.second;
    string filename = std::to_string(ID_) + " " + day + " " + std::to_string(time_block);
    return filename;
    
}


/** @fn appendPath(std::string first, std::string second) const
 *  @brief appends two strings together formatted as a path
 *  @param first the first part of the path
 *  @param second the second part of the path to be concatenated to the first part
 *  @return std::string the full path
 */
std::string IntersectionDatabase::appendPath(std::string first, std::string second) const {
    
    string path = first + "/" + second;
    return path;
    
}


/** @fn convertToDayTimeBlockPair(traffic::DateScorePair date_score_pair) const
 *  @brief extracts information from the timestamp in the DateScorePair and creates a DayTimeBlockPair
 *  @param date_score_pair the pair to extract the information from
 *  @return traffictrack::DayTimeBlockPair
 */
traffictrack::DayTimeBlockPair IntersectionDatabase::convertToDayTimeBlockPair(traffictrack::DateScorePair date_score_pair) const {
    
    //extract the first 3 characters as they are a three character shorthand for the day of the week
    string date = date_score_pair.first;
    string day = "";
    
    for (int i = 0; i < 3; i++) {
        day += date[i];
    }
    
    string discard;
    string str_time = "";
    istringstream ss(date);

    //discard information that is not needed and get to the timestamp part
    if (ss >> discard && ss >> discard && ss >> discard && ss >> str_time) {
        
        string str_time_block = "";
        
        //extract the time up until the first colon
        for (int i = 0; i < str_time.length(); i++) {
            if (str_time[i] == ':') {
                break;
            }
            else {
                str_time_block += str_time[i];
            }
        }
        
        //convert the time into an integer and determine which block of the day it is in
        int time;
        istringstream ss1(str_time_block);
        if (!(ss1 >> time)) {
            time = -1;
        }
        else {
            time /= numberOfHoursPerBlock_;
        }
        
        return DayTimeBlockPair(day, time);
        
    }
    
    return DayTimeBlockPair(day, -1);
    
}


/** @fn getCurrentDirectory() const
 *  @brief returns the current directory the program is working in
 *  @return std::string current directory
 */
std::string IntersectionDatabase::getCurrentDirectory() const {
    
    char buff[FILENAME_MAX];
    getcwd( buff, FILENAME_MAX );
    string current_working_dir(buff);
    return current_working_dir;
    
}


/** @fn create(traffictrack::IntersectionID intersection_ID, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level)
 *  @brief public static method to create the singleton. creates an instance if not created, returns null if a singleton is already created
 *  @param intersection_ID the id of the intersection that the database is for
 *  @param number_of_time_blocks the number of blocks that the day is being split into
 *  @param tmp_filename the name of the temp file being used to manipulate file data
 *  @param database_directory the directory that the database files will be stored in
 *  @param prediction_level the number of data points that will be used to calculate the average
 *  @return IntersectionDatabase* pointer to the created database
 */
IntersectionDatabase* IntersectionDatabase::create(traffictrack::IntersectionID intersection_ID, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level) {
    
    std::lock_guard<std::mutex> guard(staticMutex_);
    
    if (instance_.find(intersection_ID) == instance_.end()) {
        instance_[intersection_ID] = new IntersectionDatabase(intersection_ID, number_of_time_blocks, tmp_filename, database_directory, prediction_level);
        return instance_[intersection_ID];
    }
    else return nullptr;
    
}


/** @fn instance(int Intersection_ID)
 *  @brief returns the instance of the database for the given intersection ID
 *  @param intersection_ID the id of the intersection database you want to get
 *  @return IntersectionDatabase* pointer to the correct intersection database
 */
IntersectionDatabase* IntersectionDatabase::instance(int intersection_ID) {
    
    lock_guard<mutex> guard(staticMutex_);
    
    if (instance_.find(intersection_ID) != instance_.end()) {
        return instance_[intersection_ID];
    }
    else return nullptr;
    
}


/** @fn ~IntersectionDatabase()
 *  @brief destructor writes the data to files before destructing
 */
IntersectionDatabase::~IntersectionDatabase() {
    
    writeCache();
    
}


/** @fn log(traffictrack::DateScorePair date_score_pair)
 *  @brief logs the data in the cache
 *  @param date_score_pair the data to be stored
 *  @return bool whether or not the data was logged or not
 */
bool IntersectionDatabase::log(traffictrack::DateScorePair date_score_pair) {
    
    bool success = false;
    DayTimeBlockPair key = convertToDayTimeBlockPair(date_score_pair);
    
    lock_guard<mutex> guard(dataMutex_);
    auto it = data_.find(key);
    if (it != data_.end()) {
        it->second.add(date_score_pair);
        success = true;
    }
    
    return success;
    
}


/** @fn calculateAverages(std::promise<traffictrafk::AverageCongestionScores> p)
 *  @brief calculates the average congestion scores for each day time block
 *  @param p the promise that is used to return the value since this method will be used with a thread that can't return values
 */
void IntersectionDatabase::calculateAverages(std::promise<traffictrack::AverageCongestionScores> p) {
    
    lock_guard<mutex> guard(fileMutex_);
    
    AverageCongestionScores light_average;
    
    //go through each day time block pair
    for (auto it = data_.cbegin(); it != data_.cend(); ++it) {
        
        const auto& cache = it->second.data();
        
        CongestionScore averages;
        
        //default averages are 0
        vector<int> north_sum = { 0, 0, 0 };
        vector<int> east_sum = { 0, 0, 0 };
        vector<int> south_sum = { 0, 0, 0 };
        vector<int> west_sum = { 0, 0, 0 };
        
        int datapoints = 0;
        
        //go through the last prediction_level number of entries and add their values
        for (auto it1 = cache.crbegin(); it1 != cache.crbegin() + std::min(static_cast<int>(cache.size()), static_cast<int>(predictionLevel_)); ++it1) {
            
            const vector<int>& north_traffic = it1->first.second.getNorth();
            for (int i = 0; i < north_traffic.size(); i++) {
                north_sum[i] += north_traffic[i];
            }
            
            const vector<int>& east_traffic = it1->first.second.getEast();
            for (int i = 0; i < east_traffic.size(); i++) {
                east_sum[i] += east_traffic[i];
            }
            
            const vector<int>& south_traffic = it1->first.second.getSouth();
            for (int i = 0; i < south_traffic.size(); i++) {
                south_sum[i] += south_traffic[i];
            }
            
            const vector<int>& west_traffic = it1->first.second.getWest();
            for (int i = 0; i < west_traffic.size(); i++) {
                west_sum[i] += west_traffic[i];
            }
            
            datapoints++;
            
        }
        
        //average the values
        if (datapoints > 0) {
            
            for (int i = 0; i < north_sum.size(); i++) {
                north_sum[i] /= datapoints;
            }
            
            for (int i = 0; i < east_sum.size(); i++) {
                east_sum[i] /= datapoints;
            }
            
            for (int i = 0; i < south_sum.size(); i++) {
                south_sum[i] /= datapoints;
            }
            
            for (int i = 0; i < west_sum.size(); i++) {
                west_sum[i] /= datapoints;
            }
            
        }
        
        //set the values in a congestion score object and add it to the list of averages
        averages.setNorth(north_sum[0], north_sum[1], north_sum[2]);
        averages.setEast(east_sum[0], east_sum[1], east_sum[2]);
        averages.setSouth(south_sum[0], south_sum[1], south_sum[2]);
        averages.setWest(west_sum[0], west_sum[1], west_sum[2]);
        
        light_average[it->first] = averages;
        
    }
    
    p.set_value(light_average);
    
}


/** @fn writeCache()
 *  @brief writes the data in the caches that haven't been written to a file already
 *  @return int the number of IO errors
 */
int IntersectionDatabase::writeCache() {
    
    lock_guard<mutex> guard1(fileMutex_);
    lock_guard<mutex> guard2(dataMutex_);
    
    int ERROR_COUNT = 0;
    
    for (auto& element : data_) {
        
        string filename = generateFilename(element.first);
        filename = appendPath(databaseDirectory_, filename);
        
        ERROR_COUNT += element.second.write(filename, predictionLevel_);
        
    }
    
    return ERROR_COUNT;
    
}

