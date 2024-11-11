//
//  IntersectionDatabase.hpp
//  TraffikTrak


#ifndef IntersectionDatabase_hpp
#define IntersectionDatabase_hpp

#include <map>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <string>
#include <future>
#include "DayTimeBlockPair.hpp"
#include "DateScorePair.hpp"
#include "AverageCongestionScores.hpp"
#include "IntersectionID.h"
#include "TimeBlock.h"
#include "TempFile.h"
#include "DatabaseLocation.h"
#include "PredictionLevel.h"
#include "Cache.hpp"

/** @class IntersectionDatabase
 *  @brief manages congestion scores for intersections, returns an average congestion score over a period of time
 *  @author Matthew Lovick
 */
class IntersectionDatabase {
    
protected:
    static std::unordered_map<int, IntersectionDatabase*> instance_;
    static std::mutex staticMutex_;
    std::mutex dataMutex_;
    std::mutex fileMutex_;
    const traffictrack::IntersectionID ID_;
    const traffictrack::TimeBlock numberOfTimeBlocks_;
    const traffictrack::TimeBlock numberOfHoursPerBlock_;
    const traffictrack::TempFile tmpfilename_;
    const traffictrack::DatabaseLocation databaseDirectory_;
    const traffictrack::PredictionLevel predictionLevel_;
    const std::vector<std::string> daysOfTheWeek_;
    std::map<traffictrack::DayTimeBlockPair, Cache> data_;
    
    IntersectionDatabase(traffictrack::IntersectionID intersection_ID, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level);
    void setupCaches();
    std::string generateFilename(traffictrack::DayTimeBlockPair p) const;
    std::string appendPath(std::string first, std::string second) const;
    traffictrack::DayTimeBlockPair convertToDayTimeBlockPair(traffictrack::DateScorePair) const;
    std::string getCurrentDirectory() const;
    
public:
    static IntersectionDatabase* create(traffictrack::IntersectionID intersection_ID, traffictrack::TimeBlock number_of_time_blocks, traffictrack::TempFile tmp_filename, traffictrack::DatabaseLocation database_directory, traffictrack::PredictionLevel prediction_level);
    static IntersectionDatabase* instance(int intersection_ID);
    virtual ~IntersectionDatabase();
    bool log(traffictrack::DateScorePair date_score_pair);
    void calculateAverages(std::promise<traffictrack::AverageCongestionScores> p);
    int writeCache();
    
};

#endif /* IntersectionDatabase_hpp */
