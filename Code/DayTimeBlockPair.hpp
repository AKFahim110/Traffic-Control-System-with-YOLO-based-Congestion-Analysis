//
//  DayTimeBlockPair.hpp
//  TraffikTrak


#ifndef DayTimeBlockPair_hpp
#define DayTimeBlockPair_hpp

#include <string>
#include <utility>

namespace traffictrack {
    
    /** @class DayTimeBlockPair
     *  @brief typedef foro std::pair<std::string, int>
     *  @author Matthew Lovick
     */
    class DayTimeBlockPair : public std::pair<std::string, int> {
        
    public:
        using pair<std::string, int>::pair;
        virtual ~DayTimeBlockPair() { };
        
    };
    
}

#endif /* DayTimeBlockPair_hpp */
