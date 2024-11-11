//
//  ArgumentInterpreter.cpp
//  TraffikTrak



#include <string>
#include <sstream>
#include <map>
#include "ArgumentInterpreter.hpp"
#include "FormatException.hpp"

using std::string;
using std::istringstream;
using std::map;


/** @fn verifyConfigFileParser() const
 *  @brief verifies that the requested parser exists and it available
 *  @return bool whether or not the parser is valid
 */
bool ArgumentInterpreter::verifyConfigFileParser() const {
    
    if (validConfigFileParsers_.find(configParser_) == validConfigFileParsers_.end()) {
        return false;
    }
    else return true;
    
}


/** @fn verifyMapFileParser() const
 *  @brief verifies that the requested parser exists and it available
 *  @return bool whether or not the parser is valid
 */
bool ArgumentInterpreter::verifyMapFileParser() const {
    
    if (validMapFileParsers_.find(mapParser_) == validMapFileParsers_.end()) {
        return false;
    }
    else return true;
    
}


/** @fn verifySearchAlgorithm() const
 *  @brief verifies that the specified search algorithm exists as a real and valid implementation
 *  @return bool whether or not the algorithm is valid
 */
bool ArgumentInterpreter::verifySearchAlgorithm() const {
    
    if (validSearchAlgorithms_.find(searchAlgorithm_) == validSearchAlgorithms_.end()) {
        return false;
    }
    else return true;
    
}


/** @fn ArgumentInterpreter()
 *  @brief default instructor initializes all string members to empty and describes which implementations of various parameters are valid
 */
ArgumentInterpreter::ArgumentInterpreter() {
    
    expectedArguments = 5;
    config_ = "";
    configParser_ = "";
    map_ = "";
    mapParser_ = "";
    searchAlgorithm_ = "";
    
    validConfigFileParsers_ = {
        "QuickDatabaseConfigParser"
    };
    
    validMapFileParsers_ = {
        "QuickMapFileParser", "ExplicitMapFileParer"
    };
    
    validSearchAlgorithms_ = {
        "UniformCostSearch"
    };
    
    validArgFlags_ = {
        { "-c", config_ },
        { "-config", config_ },
        { "-cp", configParser_ },
        { "-configparser", configParser_ },
        { "-m", map_ },
        { "-map", map_ },
        { "-mp", mapParser_ },
        { "-mapparser", mapParser_ },
        { "-sa", searchAlgorithm_ },
        { "-searchalgorithm", searchAlgorithm_ }
    };
    
}


/** @fn ~ArgumentInterpreter()
 *  @brief default destructor does nothing
 */
ArgumentInterpreter::~ArgumentInterpreter() { }


/** @fn config() const
 *  @brief getter for the config file argument
 *  @return std::string the name of the config file for the database
 */
std::string ArgumentInterpreter::config() const {
    return config_;
}


/** @fn configParser() const
 *  @brief getter for the config parser type
 *  @return std::string the name of the parser for the database config file
 */
std::string ArgumentInterpreter::configParser() const {
    return configParser_;
}


/** @fn map() const
 *  @brief getter for the map file argument
 *  @return std::string the name of the map file used to create the map
 */
std::string ArgumentInterpreter::map() const {
    return map_;
}


/** @fn mapParser() const
 *  @brief getter for the map parser type
 *  @return std::string the name of the parser to read the map file
 */
std::string ArgumentInterpreter::mapParser() const {
    return mapParser_;
}


/** @fn searchAlgorithm() const
 *  @brief getter for the name of the search algorithm argument
 *  @return std::string the name of the search algorithm
 */
std::string ArgumentInterpreter::searchAlgorithm() const {
    return searchAlgorithm_;
}


/** @fn interpret(int argc, const char* argv[])
 *  @brief reads the command line arguments and parses for the valid arguments
 *  @param argc the number of arguments specified
 *  @param argv the arguments as strings
 */
void ArgumentInterpreter::interpret(int argc, const char* argv[]) {
    
    //expect program name + flags + argument
    if (argc != expectedArguments*2+1) {
        throw FormatException("arguments expected: " + std::to_string(expectedArguments));
    }
    
    //put all arguments into one string so it can be tokenized by stringstream
    string args = string(argv[1]);
    for (int i = 2; i < argc; i++) {
        args = args + " " + string(argv[i]);
    }
    
    istringstream ss(args);
    string flag;
    string argument;
    
    /*
     get tokens in pairs. first one is the flag (i.e. -config) and the second one is the argument
     if the variable has already been set, then it is a duplicate flag so throw an error
     */
    while (ss >> flag) {
        ss >> argument;
        std::map<string, string&>::iterator arg = validArgFlags_.find(flag);
        if (arg == validArgFlags_.end()) {
            throw FormatException("invalid argument type: " + flag);
        }
        else if (arg->second != "") {
            throw FormatException("duplicate argument type: " + flag);
        }
        arg->second = argument;
    }
    
    //verify that the inputs were valid
    if (!verifyConfigFileParser()) {
        throw FormatException("invalid config parser");
    }
    if (!verifyMapFileParser()) {
        throw FormatException("invalid map parser");
    }
    if (!verifySearchAlgorithm()) {
        throw FormatException("invalid search algorithm");
    }
    
}
