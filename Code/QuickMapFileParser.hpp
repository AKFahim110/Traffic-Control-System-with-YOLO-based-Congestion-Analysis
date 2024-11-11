//
//  QuickMapFileParser.hpp
//  TraffikTrak

#ifndef QuickMapFileParser_hpp
#define QuickMapFileParser_hpp

#include <string>
#include <unordered_map>
#include "AbstractMapFileParser.hpp"
#include "IntersectionID.h"

class Intersection;

/** @class QuickMapFileParser
 *  @brief reads from a config file to set up a map
 *  @author Matthew Lovick
 */
class QuickMapFileParser : public AbstractMapFileParser {
    
public:
    virtual ~QuickMapFileParser();
    virtual void parse(const std::string filename, std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections) const;
    
};

#endif /* QuickMapFileParser_hpp */
