//
//  FormatException.hpp
//  TraffikTrak


#ifndef FormatException_hpp
#define FormatException_hpp

#include <string>
#include "BaseException.hpp"

/** @class FormatException
 *  @brief formats exception with a message
 *  @author Matthew Lovick
 */
class FormatException : public BaseException {
    
public:
    FormatException();
    FormatException(const std::string& message);
    
};

#endif /* FormatException_hpp */
