//
//  DefaultCongestionScoreAnalyzer.cpp
//  TraffikTrak


#include <vector>
#include "DefaultCongestionScoreAnalyzer.hpp"
#include "Intersection.hpp"

using std::vector;


#include <iostream>
using std::cout;
using std::endl;


/** @fn DefaultCongestionScoreAnalyzer()
 *  @brief constructor sets the default critical ratio to 2
 */
DefaultCongestionScoreAnalyzer::DefaultCongestionScoreAnalyzer() {
    criticalRatio = 2;
}


/** @fn ~DefaultCongestionScoreAnalyzer()
 *  @brief destructor does nothing
 */
DefaultCongestionScoreAnalyzer::~DefaultCongestionScoreAnalyzer() { }


/** @fn analyze(traffictrack:CongestionScore score, Intersection* intersection)
 *  @brief analyzes a congestion score at an intersection and decides whether or not to change the traffic light based on the level of traffic
 *  @param score the real time congestion score
 *  @param intersection the intersection that is analyzing the congestion score
 */
void DefaultCongestionScoreAnalyzer::analyze(traffictrack::CongestionScore score, Intersection* intersection) {
    
    int north_traffic = 0;
    int east_traffic = 0;
    int south_traffic = 0;
    int west_traffic = 0;
    
    const vector<int>& north = score.getNorth();
    const vector<int>& east = score.getEast();
    const vector<int>& south = score.getSouth();
    const vector<int>& west = score.getWest();
    
    for (int traffic : north) {
        north_traffic += traffic;
    }
    
    for (int traffic : east) {
      east_traffic += traffic;
    }
    
    for (int traffic : south) {
        south_traffic += traffic;
    }
    
    for (int traffic : west) {
        west_traffic += traffic;
    }
    
    int northSouthTraffic = north_traffic + south_traffic;
    int eastWestTraffic = east_traffic + west_traffic;
    
    float ratio = intersection->state_->trafficRatio(northSouthTraffic, eastWestTraffic);
    
    
    if (ratio >= criticalRatio) {
        intersection->changeLights();
    }
    
}
