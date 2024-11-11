//
//  Road.hpp
//  TraffikTrak


#ifndef Road_hpp
#define Road_hpp

#include <utility>
#include "Direction.h"

class Intersection;

/** @class Road
 *  @brief holds the values of the intersections the road is between as well as the distance, connects intersections in the map accordingly
 *  @author Matthew Lovick
 */
class Road {
    
private:
    const std::pair<Intersection*, Intersection*> intersections_;
    const traffictrack::Direction direction_;
    const float distance_;
    
public:
    Road(Intersection* first, Intersection* second, float distance, traffictrack::Direction direction);
    virtual ~Road();
    Intersection* firstIntersection() const;
    Intersection* secondIntersection() const;
    traffictrack::Direction direction() const;
    float distance() const;
    bool operator < (const Road& other) const;
    
};

#endif /* Road_hpp */
