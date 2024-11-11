//
//  GreenLight.hpp
//  TraffikTrak

#ifndef GreenLight_hpp
#define GreenLight_hpp

#include "AbstractTrafficLightState.hpp"

/** @class GreenLight
 *  @brief class for the green traffic light state, changes traffic light accordingly
 *  @author Matthew Lovick
 */
class GreenLight : public AbstractTrafficLightState {
    
public:
    virtual ~GreenLight();
    virtual void changeLight(TrafficLight* light);

};

#endif /* GreenLight_hpp */
