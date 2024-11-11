//
//  QuickMapFileParser.cpp
//  TraffikTrak


#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include "QuickMapFileParser.hpp"
#include "Road.hpp"
#include "IOException.hpp"
#include "FormatException.hpp"
#include "IntersectionID.h"
#include "Intersection.hpp"

using namespace std;
using namespace traffictrack;


/** @fn ~QuickMapFileParser()
 *  @brief destructor that does nothing
 */
QuickMapFileParser::~QuickMapFileParser() { }


/** @fn parse(const std::string filename, std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections) const
 *  @brief parses the map file to create a map given the specified parameters
 *  @param filename the filename of the file that will be used to create the graph
 *  @param intersections the actual intersections that represent the graph
 */
void QuickMapFileParser::parse(const std::string filename, std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections) const {
    
    intersections.clear();
    
    ifstream inFile;
    inFile.open(filename);
    
    if (inFile.is_open()) {
        
        string str_number_of_intersections = "";
        int number_of_intersections = -1;
        
        getline(inFile, str_number_of_intersections);
        istringstream stream(str_number_of_intersections);
        
        stream >> number_of_intersections;
        
        //if negative number of intersections specified or the previous instruction failed (default value = -1) then throw exception
        if (number_of_intersections < 0) {
            throw FormatException("invalid number of intersections specified");
        }
        
        //create intersections with default ID 1 -> n
        for (int i = 0; i < number_of_intersections; i++) {
            IntersectionID ID(i);
            intersections.insert( { ID, new Intersection(ID) } );
            if (i == 0) {
                intersections[ID]->setHospital(true);
            }
        }
        
        string road_input = "";
        IntersectionID first(-1), second(-1);
        float distance = -1;
        char char_direction = ' ';
        string str_direction = " ";
        Direction direction = Direction::DEFAULT;
        
        //get the different components of the road
        while (getline(inFile, road_input, '\n')) {
            
            istringstream ss(road_input);
            
            ss >> first;
            ss >> second;
            ss >> distance;
            ss >> str_direction;
            
            char_direction = str_direction[0];
            char_direction = static_cast<char>(tolower(static_cast<unsigned char>(char_direction)));
            
            switch (char_direction) {
                case 'n':
                    direction = Direction::NORTH;
                    break;
                case 'e':
                    direction = Direction::EAST;
                    break;
                case 's':
                    direction = Direction::SOUTH;
                    break;
                case 'w':
                    direction = Direction::WEST;
                    break;
                default:
                    direction = Direction::DEFAULT;
            }
            
            if (intersections.find(first) == intersections.end() || intersections.find(second) == intersections.end() || distance < 0 || direction == Direction::DEFAULT) {
                inFile.close();
                throw FormatException("improper file format in " + filename);
            }
            
            intersections.at(first)->addRoad(intersections[first], intersections[second], distance, direction);
            
            int value = static_cast<int>(direction);
            value = (value+2)%4;
            Direction reverse_direction = static_cast<Direction>(value);
            
            intersections.at(second)->addRoad(intersections[second], intersections[first], distance, reverse_direction);
            
        }
        
        if (inFile.is_open()) {
            inFile.close();
        }
        
    } else {
        throw IOException("file " + filename + " not found");
    }
    
}
