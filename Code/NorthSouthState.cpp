
//  NorthSouthState.cpp
//  TraffikTrak


#include <thread>
#include <mutex>
#include "NorthSouthState.hpp"
#include "Intersection.hpp"
#include "TrafficLight.hpp"
#include "EastWestState.hpp"


/** @fn ~NorthSouthState()
 *  @brief destructor does nothing
 */
NorthSouthState::~NorthSouthState() { }


/** @fn changeLights(Intersection* intersection)
 *  @brief changes the state of the intersection by changing the interval time and changing the lights
 *  @param intersection the intersection that owns this state object
 */
void NorthSouthState::changeLights(Intersection* intersection) {
    
    std::thread* northLight;
    std::thread* southLight;
    
    std::lock_guard<std::mutex> lightsGuard(intersection->lightsMutex_);
    //change the green lights to red
    //since they turn yellow first and wait for a few seconds, use threads to do this in parallel
    northLight = new std::thread(&TrafficLight::changeLight, intersection->lights_[0]);
    southLight = new std::thread(&TrafficLight::changeLight, intersection->lights_[2]);
    
    northLight->join();
    southLight->join();
    
    delete northLight;
    delete southLight;

    //now that the other lights are red, make the east/west lights green
    intersection->lights_[1]->changeLight();
    intersection->lights_[3]->changeLight();
    
    std::lock_guard<std::mutex> nextStateGuard(intersection->nextStateMutex_);
    intersection->nextState_ = new EastWestState(intersection->eastWestIntervalTime_);
    
}


/** @fn trafficRatio(int northSouthTraffic, int eastWestTraffic)
 *  @brief returns the ratio of traffic in perpendicular directions
 *  @param northSouthTraffic combined traffic going north and south
 *  @param eastWestTraffic combined traffic going east and west
 *  @return float the ratio
 */
float NorthSouthState::trafficRatio(int northSouthTraffic, int eastWestTraffic) {
    
    return static_cast<float>(eastWestTraffic)/static_cast<float>(northSouthTraffic);
    
}

