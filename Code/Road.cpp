//
//  Road.cpp
//  TraffikTrak


#include "Road.hpp"
#include "Intersection.hpp"
#include "Direction.h"

using traffictrack::Direction;


/** @fn Road(Intersection* first, Intersection* second, float distance, Direction direction)
 *  @brief creates a new road object with the given parameters
 *  @param first the first intersection connected to the road
 *  @param second the second intersection connected to the road
 *  @param distance the length of the road
 *  @param direction the direction of the road from first -> second
 */
Road::Road(Intersection* first, Intersection* second, float distance, Direction direction) : intersections_(first, second), distance_(distance), direction_(direction) { }


/** @fn ~Road()
 *  @brief destructor does nothing
 */
Road::~Road() { }


/** @fn firstIntersection() const
 *  @brief getter for the first intersection
 *  @return Intersecion*
 */
Intersection* Road::firstIntersection() const {
    return intersections_.first;
}


/** @fn secondIntersection() const
 *  @brief getter for the second intersection
 *  @return Intersection*
 */
Intersection* Road::secondIntersection() const {
    return intersections_.second;
}


/** @fn direction() const
 *  @brief getter for the road direction
 *  @return Direction
 */
traffictrack::Direction Road::direction() const {
    return direction_;
}


/** @fn distance() const
 *  @brief getter for the road distance
 *  @return float
 */
float Road::distance() const {
    return distance_;
}


/** @fn operator < (const Road& other) const
 *  @brief overloaded operator to make Roads comparable because they are stored in a set in intersections
 *  @param other the othe road to be compared against
 *  @return bool whether or not the first road is less than the second argument
 */
bool Road::operator < (const Road& other) const {
    return static_cast<int>(this->direction()) < static_cast<int>(other.direction());
}

