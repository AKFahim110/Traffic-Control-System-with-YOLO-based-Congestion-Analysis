//
//  NorthSouthState.hpp
//  TraffikTrak


#ifndef NorthSouthState_hpp
#define NorthSouthState_hpp

#include <chrono>
#include "AbstractIntersectionState.hpp"

class Intersection;

/** @class NorthSouthState
 *  @brief changes lights based on ratio for the northsouth states
 *  @author Matthew Lovick
 */
class NorthSouthState : public AbstractIntersectionState {
    
public:
    using AbstractIntersectionState::AbstractIntersectionState;
    virtual ~NorthSouthState();
    virtual void changeLights(Intersection* intersection);
    virtual float trafficRatio(int northSouthTraffic, int eastWestTraffic);

};

#endif /* NorthSouthState_hpp */
