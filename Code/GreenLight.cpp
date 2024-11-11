//
//  GreenLight.cpp
//  TraffikTrak

#include <thread>
#include <chrono>
#include <mutex>
#include "GreenLight.hpp"
#include "RedLight.hpp"
#include "TrafficLight.hpp"
#include "LightColour.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

/** @fn ~Greenlight()
 *  @brief default constructor does nothing
 */
GreenLight::~GreenLight() { }


/** @fn changeLight(TrafficLight* light)
 *  @brief changes the light from green light to red light in the given traffic light
 *  @param light the light whose state this is, that needs to be updated
 */
void GreenLight::changeLight(TrafficLight* light) {
    
    lock_guard<mutex> colourGuard(light->colourMutex_);
    light->colour_ = traffictrack::LightColour::YELLOW;
    sleep_for(seconds(3));
    light->colour_ = traffictrack::LightColour::RED;
    lock_guard<mutex> nextStateGuard(light->nextStateMutex_);
    light->nextState_ = new RedLight();
    
}
