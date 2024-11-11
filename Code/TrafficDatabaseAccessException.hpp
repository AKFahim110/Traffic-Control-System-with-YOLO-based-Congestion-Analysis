//
//  TrafficDatabaseAccessException.hpp
//  TraffikTrak


#ifndef TrafficDatabaseAccessException_hpp
#define TrafficDatabaseAccessException_hpp

#include <string>
#include "BaseException.hpp"

/** @class TrafficDatabaseAccessException
 *  @brief handles excception with regards to accessing the traffic database
 *  @author Matthew Lovick
 */
class TrafficDatabaseAccessException : public BaseException {

public:
    TrafficDatabaseAccessException();
    TrafficDatabaseAccessException(std::string message);
    
};

#endif /* TrafficDatabaseAccessException_hpp */
