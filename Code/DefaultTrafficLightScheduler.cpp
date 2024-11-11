//
//  DefaultTrafficLightScheduler.cpp
//  TraffikTrak

#include <utility>
#include <chrono>
#include "DefaultTrafficLightScheduler.hpp"
#include "AverageCongestionScores.hpp"
#include "CongestionScore.hpp"
#include "DayTimeBlockPair.hpp"
#include "IntersectionID.h"
#include "LightSchedule.hpp"

using namespace std;
using namespace std::chrono;
using namespace traffictrack;


/** @fn ~DefaultTrafficLightScheduler()
 *  @brief destructor does nothing
 */
DefaultTrafficLightScheduler::~DefaultTrafficLightScheduler() { }


/** @fn schedule(AbstractTrafficLightScheduler::AverageScores average_scores)
 *  @brief creates the traffic light schedule for the intersections based on how heavy the congestion is, and the ratio of north/south traffic to east/west traffic
 *  @param average_scores object containing all the average congestion scores
 *  @return AbstractTrafficLightScheduler::IntersectionSchedules structure containing all the time intervals for each light at every time block
 */
AbstractTrafficLightScheduler::IntersectionSchedules DefaultTrafficLightScheduler::schedule(AbstractTrafficLightScheduler::AverageScores average_scores) {
    
    IntersectionSchedules intersection_schedules;
    
    //iterator through all intersections
    for (auto it = average_scores.begin(); it != average_scores.end(); ++it) {
        
        IntersectionID id = it->first;
        LightSchedule schedule;
        
        //iterate through all time blocks for each intersection
        AverageCongestionScores light_scores = it->second;
        for (auto it1 = light_scores.begin(); it1 != light_scores.end(); ++it1) {
            
            DayTimeBlockPair daytime = it1->first;
            CongestionScore score = it1->second;
            
            int north_traffic = 0, east_traffic = 0, south_traffic = 0, west_traffic = 0;
            
            //sum up all the traffic in a given direction (merging left-turning, straight, right-turning)
            for (int i : score.getNorth()) {
                north_traffic += i;
            }
            for (int i : score.getEast()) {
                east_traffic += i;
            }
            for (int i : score.getSouth()) {
                south_traffic += i;
            }
            for (int i : score.getWest()) {
                west_traffic += i;
            }
            
            //combine parallel traffic
            int north_south_traffic = north_traffic + south_traffic;
            int east_west_traffic = east_traffic + west_traffic;
            
            float north_south_percentage = 0;
            float east_west_percentage = 0;
            
            //convert traffic as a percentage of the whole
            int total_traffic = north_south_traffic + east_west_traffic;
            if (total_traffic > 0) {

                north_south_percentage = static_cast<float>(north_south_traffic)/static_cast<float>(total_traffic);
                east_west_percentage = static_cast<float>(east_west_traffic)/static_cast<float>(total_traffic);
                
            }
            else {
                
                north_south_percentage = 0.50;
                east_west_percentage = 0.50;
                
            }
            
            //get the cycle time based on how heavy the traffic is
            int cycle_time = 0;
            if (total_traffic < 10) {
                cycle_time = 5*60;
            }
            else if (total_traffic >= 10 && total_traffic < 20) {
                cycle_time = 3*60;
            }
            else {
                cycle_time = 2*60;
            }
            
            //convert to actual times
            int north_south_time = north_south_percentage*cycle_time;
            int east_west_time = east_west_percentage*cycle_time;
            
            //log the intervals as std::chrono::seconds
            schedule[daytime] = make_pair(seconds(north_south_time), seconds(east_west_time));
            
        }
        
        intersection_schedules[id] = schedule;
        
    }
    
    return intersection_schedules;
    
}
