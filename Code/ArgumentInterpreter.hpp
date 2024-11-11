//
//  ArgumentInterpreter.hpp
//  TraffikTrak


#ifndef ArgumentInterpreter_hpp
#define ArgumentInterpreter_hpp

#include <string>
#include <set>
#include <map>

/** @class ArgumentInterpreter
 *  @brief parses and validates command line arguments that are retrievable with getters
 *  @author Matthew Lovick
 */
class ArgumentInterpreter {
    
protected:
    int expectedArguments; /**< the expected number of arguments by the program */
    std::string config_; /**< configuration file for the database settings */
    std::string configParser_; /**< the name of the parser to read the config file */
    std::string map_; /**< file that will be used to initialize the nodes and edges of the graph */
    std::string mapParser_; /**< name of parser class to read the map file */
    std::string searchAlgorithm_; /**< name of the search algorithm that will be used to calculate the path of the emergency vehicle */
    std::set<std::string> validConfigFileParsers_; /**< contains a list of valid parser nemes to compare config_ against to see if it is valid */
    std::set<std::string> validMapFileParsers_; /**< contains a list of valid parser nemes to compare map_ against to see if it is valid */
    std::map<std::string, std::string&> validArgFlags_; /**< contains valid argument flags that represent the different program arguments. i.e -config links the following argument to the config_ data member */
    std::set<std::string> validSearchAlgorithms_;
    
    bool verifyConfigFileParser() const;
    bool verifyMapFileParser() const;
    bool verifySearchAlgorithm() const;
    
public:
    ArgumentInterpreter();
    virtual ~ArgumentInterpreter();
    std::string config() const;
    std::string configParser() const;
    std::string map() const;
    std::string mapParser() const;
    std::string searchAlgorithm() const;
    virtual void interpret(int argc, const char* argv[]);
    
};

#endif /* ArgumentInterpreter_hpp */
