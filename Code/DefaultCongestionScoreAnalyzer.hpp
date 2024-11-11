//
//  DefaultCongestionScoreAnalyzer.hpp
//  TraffikTrak

#ifndef DefaultCongestionScoreAnalyzer_hpp
#define DefaultCongestionScoreAnalyzer_hpp

#include "AbstractCongestionScoreAnalyzer.hpp"

class Intersection;

/** @class DefaultCongestion
 *  @brief analyzes default congestion scores based on the congestion scores and their respective intersections
 *  @author Matthew Lovick
 */
class DefaultCongestionScoreAnalyzer : public AbstractCongestionScoreAnalyzer {
    
protected:
    float criticalRatio;
    
public:
    DefaultCongestionScoreAnalyzer();
    virtual ~DefaultCongestionScoreAnalyzer();
    virtual void analyze(traffictrack::CongestionScore score, Intersection* intersection);
    
};

#endif /* DefaultCongestionScoreAnalyzer_hpp */
