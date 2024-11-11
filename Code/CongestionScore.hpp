//
//  CongestionScore.hpp
//  TraffikTrak

#ifndef CongestionScore_hpp
#define CongestionScore_hpp

#include <vector>

namespace traffictrack {
    
    /**
     * @class CongestionScore
     * @brief Creates an object to store congestion scores for an intersection
     *
     * This class creates a congestionScore object associated with each intersection
     * each object has the # of vehicles in each lane (left, straight, right) facing the north, south, east and west traffic light
     * @author Harkirat Bassi & Maanasa Pillai
     */
    
    class CongestionScore {
        
    protected:
        std::vector<int> north;
        std::vector<int> south;
        std::vector<int> east;
        std::vector<int> west;
        
    public:
        CongestionScore();
        const std::vector<int>& getNorth() const;
        const std::vector<int>& getSouth() const;
        const std::vector<int>& getEast() const;
        const std::vector<int>& getWest() const;
        void setNorth(int left, int straight , int right);
        void setSouth(int left, int straight , int right);
        void setEast(int left, int straight , int right);
        void setWest(int left, int straight , int right);
        
    };
    
}

#endif /* CongestionScore_hpp */
