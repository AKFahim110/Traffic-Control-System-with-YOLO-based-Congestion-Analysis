//
//  TrafficLight.hpp
//  TraffikTrak


#ifndef TrafficLight_hpp
#define TrafficLight_hpp

#include <string>
#include <mutex>
#include "LightColour.h"

class Camera;
class AbstractTrafficLightState;
class RedLight;
class GreenLight;

/** @class TrafficLight
 *  @brief takes photo and changes lights accordingly
 *  @author Matthew Lovick
 */
class TrafficLight {
    
protected:
    Camera* camera_;
    traffictrack::LightColour colour_;
    AbstractTrafficLightState* state_;
    AbstractTrafficLightState* nextState_;
    std::mutex colourMutex_;
    std::mutex stateMutex_;
    std::mutex nextStateMutex_;
    
public:
    TrafficLight(traffictrack::LightColour colour, AbstractTrafficLightState* initialState);
    virtual ~TrafficLight();
    traffictrack::LightColour colour() const;
    virtual std::string takePhoto();
    virtual void changeLight();
    friend class RedLight;
    friend class GreenLight;
    
};

#endif /* TrafficLight_hpp */
