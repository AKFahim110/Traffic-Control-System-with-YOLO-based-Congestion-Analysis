//
//  TrafficLight.cpp
//  TraffikTrak


#include <string>
#include <mutex>
#include "TrafficLight.hpp"
#include "LightColour.h"
#include "Camera.hpp"
#include "AbstractTrafficLightState.hpp"

using namespace std;


/** @fn TrafficLight(traffictrack::LightColour colour, AbstractTrafficLightState* initialState)
 *  @brief constructs the traffic light with the given initial starting state, as either red or green, and sets up the camera
 *  @param colour the colour of the light
 *  @param initialState the initial state of the light (RedLight or GreenLight)
 */
TrafficLight::TrafficLight(traffictrack::LightColour colour, AbstractTrafficLightState* initialState) {
    
    camera_ = new Camera();
    camera_->openVideoStream();
    this->colour_ = colour;
    state_ = initialState;
    nextState_ = nullptr;
    
}


/** @fn ~TrafficLight()
 *  @brief destructor that frees the dynamic memory in the class
 */
TrafficLight::~TrafficLight() {
    
    if (camera_ != nullptr) {
        camera_->closeVideoStream();
        delete camera_;
    }
    if (state_ != nullptr) {
        delete state_;
    }
    if (nextState_ != nullptr) {
        delete nextState_;
    }
    
}


/** @fn colour() const
 *  @brief returns the colour of the traffic light
 *  @return traffictrack::LightColour
 */
traffictrack::LightColour TrafficLight::colour() const {
    return colour_;
}


/** @fn takePhoto()
 *  @brief uses the camera object to take a photo, returning the file name of the photo that was taken
 *  @return string the file name of the photo
 */
std::string TrafficLight::takePhoto() {
    
    return camera_->takePhoto();
    
}


/** @fn changeLight()
 *  @brief changes the lights and the states by delegating to the state object
 */
void TrafficLight::changeLight() {
    
    lock_guard<mutex> stateGuard(stateMutex_);
    state_->changeLight(this);
    delete state_;
    lock_guard<mutex> nextStateGuard(nextStateMutex_);
    state_ = nextState_;
    nextState_= nullptr;
    
}

