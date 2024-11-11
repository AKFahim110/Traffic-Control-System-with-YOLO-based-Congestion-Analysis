//
//  RedLight.hpp
//  TraffikTrak


#ifndef RedLight_hpp
#define RedLight_hpp

#include "AbstractTrafficLightState.hpp"

class TrafficLight;

/** @class RedLight
 *  @brief changes light to the red state accordingly
 *  @author Matthew Lovick
 */
class RedLight : public AbstractTrafficLightState {
    
public:
    virtual ~RedLight();
    virtual void changeLight(TrafficLight* light);
};

#endif /* RedLight_hpp */
