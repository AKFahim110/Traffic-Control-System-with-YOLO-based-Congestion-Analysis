//
//  MinutesConverter.hpp
//  TraffikTrak


#ifndef MinutesConverter_hpp
#define MinutesConverter_hpp

#include <chrono>
#include <ratio>
#include "AbstractDurationConverter.hpp"

/** @class MinutesConverter
 *  @brief converts minutes to seconds
 *  @author Matthew Lovick
 */
class MinutesConverter : public AbstractDurationConverter {
    
public:
    virtual ~MinutesConverter();
    virtual l_seconds convert(int time);
    
};

#endif /* MinutesConverter_hpp */
