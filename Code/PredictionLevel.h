//
//  PredictionLevel.h
//  TraffikTrak

#ifndef PredictionLevel_h
#define PredictionLevel_h

#include "StrongTypeDefModules.h"

namespace traffictrack {
    
    /** @class PredictionLevel
     *   @brief abstract class - strong type for PredictionLevel to prevent ambiguity
     *   @author Matthew Lovick
     */
    class PredictionLevel : public StrongTypes::StrongTypeDefBase<int>, StrongTypes::Comparable<PredictionLevel>, StrongTypes::Outputtable<PredictionLevel>, StrongTypes::Inputtable<PredictionLevel> {
        
    public:
        using StrongTypeDefBase::StrongTypeDefBase;
        void operator = (const int& val) {
            ref() = val;
        }
        
    };
    
}

#endif /* PredictionLevel_h */
