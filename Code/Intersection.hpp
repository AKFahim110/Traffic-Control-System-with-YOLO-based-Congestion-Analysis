//
//  Intersection.hpp
//  TraffikTrak


#ifndef Intersection_hpp
#define Intersection_hpp

#include <set>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include "Road.hpp"
#include "Direction.h"
#include "IntersectionID.h"
#include "TrafficLight.hpp"
#include "AbstractStoppableThread.hpp"
#include "AbstractIntersectionState.hpp"
#include "NorthSouthState.hpp"
#include "EastWestState.hpp"
#include "AbstractCongestionScoreAnalyzer.hpp"
#include "DefaultCongestionScoreAnalyzer.hpp"
#include "LightColour.h"
#include "DateScorePair.hpp"

class NorthSouthState;
class EastWestState;
class DefaultCongestionScoreAnalyzer;
class AbstractCongestionScoreAnalyzer;

/** @class Intersection
 *  @brief intersection which manages 4 traffic lights and changing the lights based on a given schedule
 *  @author Matthew Lovick
 */
class Intersection : public AbstractStoppableThread {

protected:
    std::mutex timeCounterMutex_;
    std::mutex timeCounterThreadMutex_;
    std::mutex intervalMutex_;
    std::mutex stateMutex_;
    std::mutex nextStateMutex_;
    std::mutex neighborsMutex_;
    std::mutex intervalsMutex_;
    std::mutex lightsMutex_;
    std::thread* timeCounterThread_;
    const traffictrack::IntersectionID ID_;
    std::atomic_bool hospital_;
    std::set<Road> neighbors_;
    std::vector<TrafficLight*> lights_;
    std::chrono::seconds northSouthIntervalTime_;
    std::chrono::seconds eastWestIntervalTime_;
    std::chrono::seconds timeSinceLastChange_;
    AbstractIntersectionState* state_;
    AbstractIntersectionState* nextState_;
    AbstractCongestionScoreAnalyzer* analyzer_;
    
    void countTimeSinceLastChange();
    void monitorIntersection();
    
public:
    Intersection(traffictrack::IntersectionID ID);
    virtual ~Intersection();
    void setHospital(bool val);
    void setAnalyzer(AbstractCongestionScoreAnalyzer* analyzer);
    traffictrack::IntersectionID ID() const;
    bool hospital() const;
    traffictrack::LightColour northSouthColour() const;
    traffictrack::LightColour eastWestColour() const;
    const std::set<Road>& neighbors() const;
    bool addRoad(Road road);
    bool addRoad(Intersection* first, Intersection* second, float distance, traffictrack::Direction direction);
    void changeLights();
    void updateLightSchedule(std::chrono::seconds northSouthTime, std::chrono::seconds eastWestTime);
    virtual bool run();
    traffictrack::DateScorePair processImage();
    std::vector<TrafficLight*> getLights();
    friend class NorthSouthState;
    friend class EastWestState;
    friend class DefaultCongestionScoreAnalyzer;
    
};

#endif /* Intersection_hpp */
