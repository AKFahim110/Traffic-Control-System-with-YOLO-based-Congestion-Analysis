//
//  IOException.cpp
//  TraffikTrak


#include <string>
#include "IOException.hpp"

/** @fn IOException
 *  @brief constructor
 */
IOException::IOException() : BaseException() { }


/** @fn IOException
 *  @brief constructor but it sets custom message
 *  @param message - a message that will be displayed with the error saying "IOException"
 */
IOException::IOException(std::string message) : BaseException(message, "IOException") { }
