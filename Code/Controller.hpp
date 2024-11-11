//
//  Controller.hpp
//  TraffikTrak


#ifndef Controller_hpp
#define Controller_hpp

#include <unordered_map>
#include <queue>
#include <utility>
#include <mutex>
#include <thread>
#include <chrono>
#include "Intersection.hpp"
#include "AbstractTrafficDatabase.hpp"
#include "IntersectionID.h"
#include "AbstractPathFinder.hpp"
#include "DateScorePair.hpp"
#include "AbstractStoppableThread.hpp"
#include "AbstractTrafficLightScheduler.hpp"

class AbstractTrafficLightScheduler;

/** @class Controller
 *  @brief class controls the entire program, setting up the database and controlling
 *      the flow of information between intersections
 *  @author Matthew Lovick
 */
class Controller : public AbstractStoppableThread {
    
private:
    Controller();
    static Controller* instance_; /**< static instance for singleton */
    
protected:
    std::mutex dataRequestsMutex_;
    std::mutex emergencyAlertMutex_;
    std::mutex timeCounterMutex_;
    std::mutex timeCounterThreadMutex_;
    std::unordered_map<traffictrack::IntersectionID, Intersection*> intersections_; /**< Intersections (nodes) for the graph */
    AbstractTrafficDatabase* database_; /**< database that stores the congestion scores of the intersections */
    AbstractPathFinder* pathFinder_;
    std::queue<std::pair<traffictrack::IntersectionID, traffictrack::DateScorePair>> dataRequests_;
    std::queue<traffictrack::IntersectionID> emergencyVehicleAlerts_;
    std::vector<std::thread*> logThreads_;
    int maxLogThreads_ = 10;
    std::chrono::seconds timeBetweenScheduleUpdates_;
    std::chrono::seconds timeSinceLastUpdate_;
    std::thread* timeCounterThread_;
    AbstractTrafficLightScheduler* scheduler;
    
    void handleDataLogRequest(traffictrack::IntersectionID ID, traffictrack::DateScorePair score);
    void handleEmergencyAlert(traffictrack::IntersectionID ID);
    void clearLogThreads();
    void manageRequests();
    void countTimeSinceLastUpdate();
    void updateSchedules();
    
public:
    static Controller* instance();
    virtual ~Controller();
    bool initialize(int argc, const char* argv[]);
    void logScore(traffictrack::IntersectionID ID, traffictrack::DateScorePair score);
    void alertEmergencyVehicle(traffictrack::IntersectionID ID);
    virtual bool run();
    void testController();
    
};

#endif /* Controller_hpp */
