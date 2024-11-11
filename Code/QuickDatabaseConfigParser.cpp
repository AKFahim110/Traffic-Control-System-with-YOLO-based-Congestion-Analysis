//
//  QuickDatabaseConfigParser.cpp
//  TraffikTrak


#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <ratio>
#include "QuickDatabaseConfigParser.hpp"
#include "IOException.hpp"
#include "TrafficDatabase.hpp"
#include "FormatException.hpp"
#include "Intersection.hpp"
#include "MillisecondsConverter.hpp"
#include "SecondsConverter.hpp"
#include "MinutesConverter.hpp"
#include "HoursConverter.hpp"
#include "AbstractDurationConverter.hpp"
#include "IntersectionID.h"
#include "TimeBlock.h"
#include "PredictionLevel.h"
#include "TempFile.h"
#include "DatabaseLocation.h"

using namespace std;
using namespace traffictrack;


/** @fn isPositiveInteger(std::string str_value) const
 *  @brief tests whether a string representation of an integer is a valid positive integer
 *  @param str_value the number to be tested as a string
 *  @return std::pair<bool, int> pair.first is whether or not the input was a positive integer, and pair.second is the interger value if successful
 */
std::pair<bool, int> QuickDatabaseConfigParser::isPositiveInteger(std::string str_value) const {
    
    //use stringstream to attempt to convert the string to an int and test for failure
    pair<bool, int> result;
    int value;
    istringstream ss(str_value);
    if (ss >> value) {
        if (value > 0)
            return pair<bool, int>(true, value);
    }
    return pair<bool, int>(false, NULL);
    
}


/** @fn isNonEmptyString(std::string input) const
 *  @brief tests whether or not a string is non empty
 *  @param input string to be tested
 *  @return bool whether or not the string was non empty
 */
bool QuickDatabaseConfigParser::isNonEmptyString(std::string input) const {
    return input != "";
}


/** @fn validateTimeUnit(std::string input)
 *  @brief tests whether a specified time unit given as a string is a valud std::chrono::duration given in the timeUnitConverter map
 *  @param input the input to be tested
 *  @return bool whether or not the input was a valid time unit
 */
bool QuickDatabaseConfigParser::validateTimeUnit(std::string input) {
    
    if (timeUnitConverters_.find(input) != timeUnitConverters_.end()) {
        timeUnit_ = input;
        return true;
    }
    else return false;
    
}


/** @fn validateTimeQuantity(std::string input)
 *  @brief tests whether the time quantity given is valid, meaning positive integer and can be converted to type l_seconds
 *  @param input the time unit to be converted
 *  @return bool whether the operation is successful
 */
bool QuickDatabaseConfigParser::validateTimeQuantity(std::string input) {
    
    pair<bool, int> result = isPositiveInteger(input);
    if (result.first) {
        
        l_seconds time;
        std::map<std::string, AbstractDurationConverter*>::iterator it = timeUnitConverters_.find(timeUnit_);
        if (it != timeUnitConverters_.end()) {
            time = it->second->convert(result.second);
            timeQuantity_ = time;
            return true;
        }
    }
    return false;

}


/** @fn validateTimeBlocks(std::string input)
 *  @brief tests whether or not a given time block is valid. in this case, the restriction is that it needs to be a positive integer
 *  @param input the time block to test
 *  @return bool whether the operation was successful or not
 */
bool QuickDatabaseConfigParser::validateTimeBlocks(std::string input) {
    
    pair<bool, int> result = isPositiveInteger(input);
    if (result.first) {
        numberOfTimeBlocks_ = result.second;
    }
    return result.first;
    
}


/** @fn validateTempFile(std::string input)
 *  @brief tests that the temp file name specified is not empty
 *  @param input the filename to be tested
 *  @return bool whether or not the input was valid
 */
bool QuickDatabaseConfigParser::validateTempFile(std::string input) {
    
    if (isNonEmptyString(input)) {
        tempFile_ = input;
        return true;
    }
    return false;
    
}


/** @fn validateDatabaseLocation(std::string input)
 *  @brief tests whether or not the specified database directory location is non empty
 *  @param input the directory to be tested
 *  @return bool whether or not the input was non empty
 */
bool QuickDatabaseConfigParser::validateDatabaseLocation(std::string input) {
    
    if (isNonEmptyString(input)) {
        databaseLocation_ = input;
        return true;
    }
    return false;

}


/** @fn validateNumberOfDataPoints(std::string input(
 *  @brief tests whether the number of data points specified is a positive integer or not
 *  @param input the string value to be tested
 *  @return bool whether or not the input was valid
 */
bool QuickDatabaseConfigParser::validateNumberOfDataPoints(std::string input) {
    
    pair<bool, int> result = isPositiveInteger(input);
    if (result.first) {
        numberOfDataPoints_ = result.second;
    }
    return result.first;
    
}


/** @fn validateCommand(std::string command, std::string value)
 *  @brief tests whether the command parameter given is a valid argument type
 *  @param command the parameter type given
 *  @param value the value of that parameter
 *  @return bool whether or not the parameter was valid
 */
bool QuickDatabaseConfigParser::validateCommand(std::string command, std::string value) {
    
    map<string, validationFunction>::const_iterator it = validCommands_.find(command);
    if (it != validCommands_.end()) {
        validationFunction f = it->second;
        return (this->*f)(value);
    }
    else return false;
    
}


/** @fn removeWhitespace(std::string str) const
 *  @brief removes excess whitespace from inputs
 *  @param str the string to format
 *  @return std::string the formatted string
 */
std::string QuickDatabaseConfigParser::removeWhitespace(std::string str) const {
    
    //split the input into whitespace separated tokens and then rejoin the input with 1 space between tokens
    istringstream cleaner(str);
    string result = "";
    if (cleaner >> result) {
        string token;
        while (cleaner >> token) {
            result = result +  " " + token;
        }
    }
    return result;
    
}


/** @fn QuickDatabaseConfigParser()
 *  @brief initializes the values of the object, including the valid parameter options and all function pointer mapping
 */
QuickDatabaseConfigParser::QuickDatabaseConfigParser()   {
    
    timeUnit_ = "";
    timeQuantity_ = std::chrono::duration<double, std::ratio<1,1>>(-1);
    numberOfTimeBlocks_ = -1;
    numberOfDataPoints_ = -1;
    tempFile_ = "";
    databaseLocation_ = "";
    
    validCommands_ = {
        { "Unit of Time", &QuickDatabaseConfigParser::validateTimeUnit },
        { "Length of Time", &QuickDatabaseConfigParser::validateTimeQuantity },
        { "Number of Time Blocks", &QuickDatabaseConfigParser::validateTimeBlocks },
        { "Name of Temp File", &QuickDatabaseConfigParser::validateTempFile},
        { "Database Folder Name", &QuickDatabaseConfigParser::validateDatabaseLocation },
        { "Number of Data Points For Prediction", &QuickDatabaseConfigParser::validateNumberOfDataPoints }
    };
    
    timeUnitConverters_ = {
        {"milliseconds", new MillisecondsConverter()},
        {"seconds", new SecondsConverter()},
        {"minutes", new MinutesConverter()},
        {"hours", new HoursConverter()}
    };
    
}


/** @fn ~QuickDatabaseConfigParser()
 *  @brief destructor frees dynamic memory
 */
QuickDatabaseConfigParser::~QuickDatabaseConfigParser() {
    
    for (std::map<string, AbstractDurationConverter*>::iterator it = timeUnitConverters_.begin(); it != timeUnitConverters_.end(); ++it) {
        delete it->second;
    }
    
}


/** @fn parse(const std::string filename, const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections)
 *  @brief parses the config file and extracts the database parameters from it.
 *  @param filename the config file with the database parameters specified
 *  @param intersections the intersections that need to be sent to the database to initialize it
 *  @return AbstractTrafficDatabase* returns a pointer to a newly created database
 */
AbstractTrafficDatabase* QuickDatabaseConfigParser::parse(const std::string filename, const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections) {
    
    //read parameters line by line, where the parameter and the value are separated by a colon
    //validate the parameter and set the value accordingly
    
    ifstream inFile;
    inFile.open(filename);
    
    if (inFile.is_open()) {
        
        string line;
        while(getline(inFile, line)) {
            
            istringstream ss(line);
            
            string command = "";
            string value = "";
            
            if (!getline(ss, command, ':')) { //all lines must have a colon to separate the parameter from the value
                throw FormatException("invalid entry in config file");
            }
            if (!getline(ss, value, '\n')) { //nothing after the colon, so no value given
                throw FormatException("no value given for parameter: " + command);
            }
            
            //format the input
            command = removeWhitespace(command);
            value = removeWhitespace(value);
            
            //validate the parameter
            if (!validateCommand(command, value)) {
                throw FormatException("Invalid parameter: " + command + " " + value);
            }
            
        }
        
        //check that all parameters have been set
        if (timeUnit_ == "" || static_cast<string>(tempFile_) == "" || static_cast<string>(databaseLocation_) == "" || timeQuantity_.count() == -1 || numberOfTimeBlocks_ == -1 || numberOfDataPoints_ == -1) {
            throw FormatException("not all parameters have been set");
        }
        
        //create the database and return it
        AbstractTrafficDatabase* db = TrafficDatabase::create(intersections, timeQuantity_, numberOfTimeBlocks_, tempFile_, databaseLocation_, numberOfDataPoints_);
        if (inFile.is_open()) {
            inFile.close();
        }
        return db;
        
    }
    else {
        throw IOException("file " + filename + " not found");
    }
    
    return nullptr;
}
