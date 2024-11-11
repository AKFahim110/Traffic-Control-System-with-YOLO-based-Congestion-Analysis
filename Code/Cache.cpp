//
//  Cache.cpp
//  TraffikTrak


#include <string>
#include <fstream>
#include <ctime>
#include <exception>
#include "Cache.hpp"
#include "CongestionScore.hpp"

using namespace std;
using namespace traffictrack;


/** @fn ~Cache()
 *  @brief destructor doesn nothing
 */
Cache::~Cache() { }


/** @fn add(traffictrack::DateScorePair date_score_pair, bool cached)
 *  @brief adds a date_score_pair to the vector
 *  @param date_score_pair the data to be logged
 *  @param cached whether or not the item has already been cached or not. distinguishes new data in the vector vs old data that has already been written
 */
void Cache::add(traffictrack::DateScorePair date_score_pair, bool cached) {
    
    data_.push_back(std::pair<traffictrack::DateScorePair, bool>(date_score_pair, cached));

}


/** @fn write(const std::string filename, const int min_cached_size)
 *  @brief writes the uncached items in the vector to the file
 *  @param filename the file to write to
 *  @param min_cache_size the min amount of items to keep in the cache so that the average can be calculated without reading from the external file
 *  @return int the number of IO errors while writing
 */
int Cache::write(const std::string filename, const int min_cache_size) {
    
    ofstream outFile;
    int ERROR_COUNT = 0;
    
    outFile.open(filename, ofstream::out | ofstream::app);
    if (outFile.is_open()) {
        
        vector<DateScorePair*> data_to_write;
        
        //start from the end since the uncached items will be appended to the end
        //break when you reach the first cached file, since you won't want to write it again
        for (auto it = data_.rbegin() ; it != data_.rend(); ++it) {
            
            bool& cached = it->second;
            if (cached)
                break;
            
            data_to_write.push_back(&it->first);
            it->second = true;
            
        }
        
        //write to the file
        for (auto it = data_to_write.crbegin(); it != data_to_write.crend(); ++it) {
            
            try {
                //print the date to the file
                string time = (*it)->first;
                outFile << time << endl;
                CongestionScore congestion_score = (*it)->second;
                
                //print the north bound traffic
                for (int score : congestion_score.getNorth()) {
                    outFile << score << " ";
                }
                //print the east bound traffic
                for (int score : congestion_score.getEast()) {
                    outFile << score << " ";
                }
                //print the south bound traffic
                for (int score : congestion_score.getSouth()) {
                    outFile << score << " ";
                }
                //print the west bound traffic
                for (int score : congestion_score.getWest()) {
                    outFile << score << " ";
                }
                //print a newline
                outFile << std::endl;
            }
            catch (exception& e) {
                ERROR_COUNT++;
            }
            
        }
        
    }
    
    if (outFile.is_open()) {
        outFile.close();
    }
    
    data_.erase(data_.begin(), data_.begin() + std::max(0, static_cast<int>(data_.size()) - min_cache_size));
    
    return ERROR_COUNT;
    
}


/** @fn data() const
 *  @brief getter method for the cache data
 *  @return const std::vector<std::pair<traffictrack::DateScorePair, bool>>& the vector of data to cache
 */
const std::vector<std::pair<traffictrack::DateScorePair, bool>>& Cache::data() const {
    return data_;
}

