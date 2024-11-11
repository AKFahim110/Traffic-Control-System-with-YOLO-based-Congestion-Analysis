//
//  CongestionScore.cpp
//  TraffikTrak


#include <vector>
#include "CongestionScore.hpp"

using std::vector;


/**
 * @fn congestionScore()
 * @brief default constructor for congestionScore
 * This creates an object that stores the congestion levels at all four side of an intersection
 */
traffictrack::CongestionScore::CongestionScore() { }


/**
 * @fn getNorth()
 * @brief Getter method to get the number of cars at the north side of the intsersection
 * This function is used to access the number of cars turning left, right, or going straight at the intersection
 * @return const std::vector<int>& - returns a vector storing the cars turning left in index 0, cars going straight
 * in index 1, and cars turning left in index 2
 */

const std::vector<int>& traffictrack::CongestionScore::getNorth() const {
    return north;
}


/**
 * @fn getSouth()
 * @brief Getter method to get the number of cars at the south side of the intsersection
 * This function is used to access the number of cars turning left, right, or going straight at the intersection
 * @return const std::vector<int>& - returns a vector storing the cars turning left in index 0, cars going straight
 * in index 1, and cars turning left in index 2
 */
const std::vector<int>& traffictrack::CongestionScore::getSouth() const {
    return south;
}


/**
 * @fn getEast()
 * @brief Getter method to get the number of cars at the east side of the intsersection
 * This function is used to access the number of cars turning left, right, or going straight at the intersection
 * @return const std::vector<int>& - returns a vector storing the cars turning left in index 0, cars going straight
 * in index 1, and cars turning left in index 2
 */
const std::vector<int>& traffictrack::CongestionScore::getEast() const {
    return east;
}


/**
 * @fn getWest()
 * @brief Getter method to get the number of cars at the west side of the intsersection
 * This function is used to access the number of cars turning left, right, or going straight at the intersection
 * @return const std::vector<int>& - returns a vector storing the cars turning left in index 0, cars going straight
 * in index 1, and cars turning left in index 2
 */
const std::vector<int>& traffictrack::CongestionScore::getWest() const {
    return west;
}


/**
 * @fn setNorth
 * @brief setter method for the North traffic light
 * @param left - the #cars in the left lane
 * @param straight - #cars in the straight lanes
 * @param right - #cars in the right lane
 */
void traffictrack::CongestionScore::setNorth(int left, int straight , int right) {
    north.clear();
    north.push_back(left);
    north.push_back(straight);
    north.push_back(right);
}


/**
 * @fn setSouth
 * @brief setter method for the south traffic light
 * @param left - the #cars in the left lane
 * @param straight - #cars in the straight lanes
 * @param right - #cars in the right lane
 */
void traffictrack::CongestionScore::setSouth(int left, int straight , int right) {
    south.clear();
    south.push_back(left);
    south.push_back(straight);
    south.push_back(right);
}


/**
 * @fn setEast
 * @brief setter method for the east traffic light
 * @param left - the #cars in the left lane
 * @param straight - #cars in the straight lanes
 * @param right - #cars in the right lane
 */
void traffictrack::CongestionScore::setEast(int left, int straight , int right) {
    east.clear();
    east.push_back(left);
    east.push_back(straight);
    east.push_back(right);
}


/**
 * @fn setWest
 * @brief setter method for the west traffic light
 * @param left - the #cars in the left lane
 * @param straight - #cars in the straight lanes
 * @param right - #cars in the right lane
 */
void traffictrack::CongestionScore::setWest(int left, int straight , int right) {
    west.clear();
    west.push_back(left);
    west.push_back(straight);
    west.push_back(right);
}




