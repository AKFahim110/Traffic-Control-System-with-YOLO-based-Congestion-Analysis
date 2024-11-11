//
//  IOException.hpp
//  TraffikTrak


#ifndef IOException_hpp
#define IOException_hpp

#include <string>
#include "BaseException.hpp"

/** @class IOException
 *  @brief handles input exception formatting and messaging accordingly
 *  @author Matthew Lovick
 */
class IOException : public BaseException {
    
public:
    IOException();
    IOException(std::string message);
    
};

#endif /* IOException_hpp */
