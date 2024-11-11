//
//  TrafficDatabaseAccessException.cpp
//  TraffikTrak


#include <string>
#include "TrafficDatabaseAccessException.hpp"

/** @fn TrafficDatabaseAccesssException
 *  @brief constructor
 */
TrafficDatabaseAccessException::TrafficDatabaseAccessException() : BaseException() { }


/** @fn TrafficDatabaseAccessException
 *  @brief constructor
 *  @param message for traffic database access exception
 */
TrafficDatabaseAccessException::TrafficDatabaseAccessException(std::string message) : BaseException(message, "TrafficDatabaseAccessException") { }

