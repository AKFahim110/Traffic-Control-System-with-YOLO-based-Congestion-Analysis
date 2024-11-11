//
//  TrafficDatabase.hpp
//  TraffikTrak
#ifndef TrafficDatabase_hpp
#define TrafficDatabase_hpp

#include <mutex>
#include <unordered_map>
#include <string>
#include <chrono>
#include <ratio>
#include <atomic>
#include "AbstractTrafficDatabase.hpp"
#include "DateScorePair.hpp"
#include "AverageCongestionScores.hpp"
#include "TrafficDatabaseAccessException.hpp"
#include "IntersectionID.h"
#include "TimeBlock.h"
#include "TempFile.h"
#include "DatabaseLocation.h"
#include "PredictionLevel.h"

class Intersection;
class IntersectionDatabase;

/** @class TrafficDatabase
 *  @brief holds traffic data for each intersection
 *  @author Matthew Lovick
 */
class TrafficDatabase : public AbstractTrafficDatabase {
    
protected:
    static TrafficDatabase* instance_;
    static std::mutex staticMutex_;
    std::mutex databaseMutex_;
    std::atomic_bool active_;
    const l_seconds timeBetweenSaves_;
    std::unordered_map<traffictrack::IntersectionID, IntersectionDatabase*> intersectionDatabases_;
    TrafficDatabase(const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections, l_seconds time_between_saves, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level);
    void deactivate();
    std::string getCurrentDirectory() const;
    void periodicallyWriteCache();
    
public:
    static TrafficDatabase* create(const std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections, l_seconds time_between_saves, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level);
    static TrafficDatabase* instance();
    virtual ~TrafficDatabase();
    virtual bool log(traffictrack::IntersectionID intersection, traffictrack::DateScorePair date_score_pair);
    virtual const std::unordered_map<traffictrack::IntersectionID, traffictrack::AverageCongestionScores> calculateAverages();
    bool active() const;
    virtual void close();
    virtual bool run();
    
};

#endif /* TrafficDatabase_hpp */
