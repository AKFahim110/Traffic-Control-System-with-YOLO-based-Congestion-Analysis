//
//  MillisecondsConverter.hpp
//  TraffikTrak

#ifndef MillisecondsConverter_hpp
#define MillisecondsConverter_hpp

#include <chrono>
#include <ratio>
#include "AbstractDurationConverter.hpp"

/** @class MillisecondsConverter
 *  @brief converts milliseconds to seconds
 *  @author Matthew Lovick
 */
class MillisecondsConverter : public AbstractDurationConverter {
    
public:
    virtual ~MillisecondsConverter();
    virtual l_seconds convert(int time);
    
};

#endif /* MillisecondsConverter_hpp */
