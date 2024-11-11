//
//  RedLight.cpp
//  TraffikTrak

#include "RedLight.hpp"
#include "GreenLight.hpp"
#include "TrafficLight.hpp"
#include "LightColour.h"

using namespace std;


/** @fn ~RedLight()
 *  @brief default constructor does nothing
 */
RedLight::~RedLight() { }


/** @fn changeLight(TrafficLight* light)
 *  @brief changes the light from being a red light to being a green light
 *  @param light the traffic light whose state this is, whose state needs to be changed
 */
void RedLight::changeLight(TrafficLight* light) {
    
    lock_guard<mutex> colourGuard(light->colourMutex_);
    light->colour_ = traffictrack::LightColour::GREEN;
    lock_guard<mutex> nextStateGuard(light->nextStateMutex_);
    light->nextState_ = new GreenLight();
    
}
