//
//  EastWestState.cpp
//  TraffikTrak

#include <thread>
#include <mutex>
#include "EastWestState.hpp"
#include "Intersection.hpp"
#include "TrafficLight.hpp"


/** @fn ~NorthSouthState()
 *  @brief destructor does nothing
 */
EastWestState::~EastWestState() { }


/** @fn changeLights(Intersection* intersection)
 *  @brief changes the state of the intersection by changing the interval time and changing the lights
 *  @param intersection the intersection that owns this state object
 */
void EastWestState::changeLights(Intersection* intersection) {
    
    std::thread* eastLight;
    std::thread* westLight;
    
    std::lock_guard<std::mutex> lightsGuard(intersection->lightsMutex_);
    //change the green lights to red
    //since they turn yellow first and wait for a few seconds, use threads to do this in parallel
    eastLight = new std::thread(&TrafficLight::changeLight, intersection->lights_[1]);
    westLight = new std::thread(&TrafficLight::changeLight, intersection->lights_[3]);
    
    eastLight->join();
    westLight->join();
    
    delete eastLight;
    delete westLight;
    
    //now that the other lights are red, make the east/west lights green
    intersection->lights_[0]->changeLight();
    intersection->lights_[2]->changeLight();
    
    std::lock_guard<std::mutex> nextStateGuard(intersection->nextStateMutex_);
    intersection->nextState_ = new NorthSouthState(intersection->northSouthIntervalTime_);
    
}


/** @fn trafficRatio(int northSouthTraffic, int eastWestTraffic)
 *  @brief returns the ratio of traffic in perpendicular directions
 *  @param northSouthTraffic combined traffic going north and south
 *  @param eastWestTraffic combined traffic going east and west
 *  @return float the ratio
 */
float EastWestState::trafficRatio(int northSouthTraffic, int eastWestTraffic) {
    
    return static_cast<float>(northSouthTraffic)/static_cast<float>(eastWestTraffic);
    
}
