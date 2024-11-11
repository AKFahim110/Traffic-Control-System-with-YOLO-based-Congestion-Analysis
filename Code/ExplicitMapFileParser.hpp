//
//  ExplicitMapFileParser.hpp
//  TraffikTrak


#ifndef ExplicitMapFileParser_hpp
#define ExplicitMapFileParser_hpp

#include <string>
#include <unordered_map>
#include "AbstractMapFileParser.hpp"
#include "IntersectionID.h"

class Intersection;

/** @class ExplicitMapFileParser : public AbstractMapFileParser
 *  @brief parses a file and builds the intersections and roads accordingly.
 *      Intersection IDs are explicitly given
 *  @author Matthew Lovick
 */
class ExplicitMapFileParser : public AbstractMapFileParser {
    
public:
    virtual ~ExplicitMapFileParser();
    virtual void parse(const std::string filename, std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections) const;
    
};

#endif /* ExplicitMapFileParser_hpp */
