//
//  IntersectionID.h
//  TraffikTrak


#ifndef IntersectionID_h
#define IntersectionID_h

#include "StrongTypeDefModules.h"

namespace traffictrack {
    
    /** @class IntersectionID
     *  @brief abstract class - strong type for intersectionID to prevent ambiguity
     *  @author Matthew Lovick
     */
    class IntersectionID : public StrongTypes::StrongTypeDefBase<int>, StrongTypes::Comparable<IntersectionID>, StrongTypes::Outputtable<IntersectionID>, StrongTypes::Inputtable<IntersectionID> {
        
    public:
        using StrongTypeDefBase::StrongTypeDefBase;
        void operator = (const int& val) {
            ref() = val;
        }
        
    };
    
}

namespace std {
    
    template<>
    struct hash<traffictrack::IntersectionID> {
        size_t operator () (const traffictrack::IntersectionID& key) const {
            return std::hash<int>()(static_cast<int>(key));
        }
    };
    
}

#endif /* IntersectionID_h */
