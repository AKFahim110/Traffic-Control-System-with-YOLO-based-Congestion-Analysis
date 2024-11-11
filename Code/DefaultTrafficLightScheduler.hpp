//
//  DefaultTrafficLightScheduler.hpp
//  TraffikTrak


#ifndef DefaultTrafficLightScheduler_hpp
#define DefaultTrafficLightScheduler_hpp

#include "AbstractTrafficLightScheduler.hpp"

/** @class DefaultTrafficLightScheduler
 *  @brief schedules traffic lights based on average scores
 *  @author Matthew Lovick
 */
class DefaultTrafficLightScheduler : public AbstractTrafficLightScheduler {
    
public:
    virtual ~DefaultTrafficLightScheduler();
    virtual IntersectionSchedules schedule(AverageScores average_scores);
    
};

#endif /* DefaultTrafficLightScheduler_hpp */
