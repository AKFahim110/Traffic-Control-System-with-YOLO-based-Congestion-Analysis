//
//  ExplicitMapFileParser.cpp
//  TraffikTrak


#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include "ExplicitMapFileParser.hpp"
#include "Road.hpp"
#include "IOException.hpp"
#include "FormatException.hpp"
#include "IntersectionID.h"
#include "Intersection.hpp"

using namespace std;
using namespace traffictrack;


/** @fn ~ExplicitMapFileParser()
 *  @brief destructor does nothing
 */
ExplicitMapFileParser::~ExplicitMapFileParser() { }


/** @fn parse(const std::string filename, std::vector<Intersection*>& intersections) const
 *  @brief reads from the file filename to configure the graph
 *  @param filename the file that will be used to create the graph
 *  @param intersections the intersections that actually represent the graph
 */
void ExplicitMapFileParser::parse(const std::string filename, std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections) const {
    
    //same as QuickMapFileParser except that it reads Intersection IDs instead of giving default IDs
    
    intersections.clear();
    
    ifstream inFile;
    inFile.open(filename);
    
    if (inFile.is_open()) {
        
        int number_of_intersections;
        inFile >> number_of_intersections;
        inFile.ignore();
        
        string intersection_ID;
        IntersectionID ID;
        
        for (int i = 0; i < number_of_intersections; i++) {
            
            if (!getline(inFile, intersection_ID)) {
                throw FormatException("invalid number of intersection IDs specified");
            }
            
            istringstream ss(intersection_ID);
            
            if (!(ss >> ID)) {
                throw FormatException("invalid intersection ID");
            }
            
            intersections.insert( { ID, new Intersection(ID) } );
            
            if (i == 0) {
                intersections[ID]->setHospital(true);
            }
            
        }
        
        
        string road_input;
        IntersectionID first(-1), second(-1);
        float distance = -1;
        char char_direction = ' ';
        string str_direction = " ";
        Direction direction = Direction::DEFAULT;
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
                throw FormatException("improper file formate in " + filename);
            }
            
            intersections.at(first)->addRoad(intersections[first], intersections[second], distance, direction);
            
            int value = static_cast<int>(direction);
            value = (value+2)%4;
            Direction reverse_direction = static_cast<Direction>(value);
            
            intersections.at(second)->addRoad(intersections[second], intersections[first], distance, reverse_direction);
            
        }
        
    } else {
        throw IOException("file " + filename + " not found");
    }
    
}
