//
//  FormatException.cpp
//  TraffikTrak


#include <string>
#include "FormatException.hpp"


/** @fn FormatException
 *  @brief constructor
 */
FormatException::FormatException() : BaseException() { };


/** @fn FormatException
 *  @brief constructor
 *  @param message - a message that will display formatexception
 */
FormatException::FormatException(const std::string& message) : BaseException(message, "FormatException") { }
