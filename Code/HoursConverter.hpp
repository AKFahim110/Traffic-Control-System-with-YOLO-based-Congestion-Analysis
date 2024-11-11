//
//  HoursConverter.hpp
//  TraffikTrak


#ifndef HoursConverter_hpp
#define HoursConverter_hpp

#include <chrono>
#include <ratio>
#include "AbstractDurationConverter.hpp"

/** @class HoursConverter
 *  @brief converts hours into seconds
 *  @author Matthew Lovick
 */
class HoursConverter : public AbstractDurationConverter {
    
public:
    virtual ~HoursConverter();
    virtual l_seconds convert(int time);
    
};

#endif /* HoursConverter_hpp */
