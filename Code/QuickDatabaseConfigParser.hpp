//
//  QuickDatabaseConfigParser.hpp
//  TraffikTrak


#ifndef QuickDatabaseConfigParser_hpp
#define QuickDatabaseConfigParser_hpp

#include <string>
#include <map>
#include <unordered_map>
#include <utility>
#include "AbstractDatabaseConfigParser.hpp"
#include "TrafficDatabase.hpp"
#include "IntersectionID.h"
#include "TimeBlock.h"
#include "PredictionLevel.h"
#include "TempFile.h"
#include "DatabaseLocation.h"

class AbstractDurationConverter;
class Intersection;

/** @class QuickDatabaseConfigParser
 *  @brief reads from a config file to set up a database with the justified parameters
 *  @author Matthew Lovick
 */
class QuickDatabaseConfigParser : public AbstractDatabaseConfigParser {
    
protected:
    typedef bool (QuickDatabaseConfigParser::*validationFunction)(std::string);
    typedef TrafficDatabase::l_seconds l_seconds;
    std::map<std::string, validationFunction> validCommands_;
    std::map<std::string, AbstractDurationConverter*> timeUnitConverters_;
    std::string timeUnit_;
    l_seconds timeQuantity_;
    traffictrack::TimeBlock numberOfTimeBlocks_;
    traffictrack::PredictionLevel numberOfDataPoints_;
    traffictrack::TempFile tempFile_;
    traffictrack::DatabaseLocation databaseLocation_;
    
    std::string removeWhitespace(std::string str) const;
    std::pair<bool, int> isPositiveInteger(std::string str_value) const;
    bool isNonEmptyString(std::string input) const;
    
    bool validateTimeUnit(std::string input);
    bool validateTimeQuantity(std::string input);
    bool validateTimeBlocks(std::string input);
    bool validateTempFile(std::string input);
    bool validateDatabaseLocation(std::string input);
    bool validateNumberOfDataPoints(std::string input);
    
    bool validateCommand(std::string command, std::string value);
    
public:
    QuickDatabaseConfigParser();
    virtual ~QuickDatabaseConfigParser();
    virtual AbstractTrafficDatabase* parse(const std::string filename, const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections);
    
};

#endif /* QuickDatabaseConfigParser_hpp */
