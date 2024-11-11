//
//  MinutesConverter.cpp
//  TraffikTrak


#include <chrono>
#include <ratio>
#include "MinutesConverter.hpp"

/** @fn ~MinutesConverter
 *  @brief deconstructor
 */
MinutesConverter::~MinutesConverter() { }


/** @fn convert
 *  @brief converts minutes to  seconds
 *  @param time - takes in time as a parameter in minutes format
 *  @return - returns converted time in long seconds
 */
MinutesConverter::l_seconds MinutesConverter::convert(int time) {
    return MinutesConverter::l_seconds((std::chrono::minutes(time)));
}
