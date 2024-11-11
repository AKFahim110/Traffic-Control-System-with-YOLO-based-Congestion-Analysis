//
//  HoursConverter.cpp
//  TraffikTrak

#include <chrono>
#include <ratio>
#include "HoursConverter.hpp"

/** @fn ~HoursConverter
 *  @brief deconstructor
 */
HoursConverter::~HoursConverter() { }


/** @fn convert
 *  @brief converts hours to seconds
 *  @param time - takes in time as a parameter in hours format
 *  @return - returns converted time in seconds
 */
HoursConverter::l_seconds HoursConverter::convert(int time) {
    return HoursConverter::l_seconds((std::chrono::hours(time)));
}

