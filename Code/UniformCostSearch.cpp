//
//  UniformCostSearch.cpp
//  TraffikTrak

#include <vector>
#include <unordered_map>
#include <queue>
#include <map>
#include <utility>
#include "UniformCostSearch.hpp"
#include "Intersection.hpp"
#include "IntersectionID.h"

using namespace std;
using namespace traffictrack;


/** @fn ~UniformCostSearch()
 *  @brief destructor that does nothing
 */
UniformCostSearch::~UniformCostSearch() { }


/** @fn search(Intersection* startState, std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections)
 *  @brief performs a uniform cost search algorithm on the graph and returns the path
 *  @return std::pair<std::vector<Intersection*>, float> the path as a vector
 */
std::pair<std::vector<Intersection*>, float> UniformCostSearch::search(Intersection* startState, std::unordered_map<traffictrack::IntersectionID, Intersection*>& intersections) {
    
    //float is the distance from the starting node, and the Intersection* is a pointer to the predecessor along the shortest path
    typedef pair<float, Intersection*> node;
    
    //auxiliary data structures to help with the search
    vector<Intersection*> path; //will be used at the end to return the final path
    map<Intersection*, bool> expanded; //tracks whether a node has already been expanded
    map<Intersection*, node> predecessor; //tracks the current shortest path to a node and the parent node that got to the node
    priority_queue<node, vector<node>, std::greater<node>> queue; //min priority queue to pop nodes with the smallest paths
    
    bool found_goal_state = false; //tracks whether or not we have found the goal state or not
    Intersection* goal = nullptr; //pointer to the goal Intersection
    
    //start with the starting state in the queue
    predecessor[startState] = make_pair(0, nullptr); //starting state has no predecessor and is 0 distance from itself
    node start(0, startState);
    queue.push(start);
    
    //represents the current node being analyzed in the search
    node current;
    
    //loop until we have either found teh goal state or we have searched the entire graph and the goal state was not found
    while (!found_goal_state && !queue.empty()){
        
        //get the node that is currently the shortest distance from the starting node
        current = queue.top();
        queue.pop();
        float currentDistance = predecessor[current.second].first;
        
        //if we find the goal state, quit
        if (current.second->hospital()) {
            found_goal_state = true;
            goal = current.second;
            continue;
        }
        
        //only expand a node if it hasn't been expanded already
        //due to the order of expansion, a node will only be dequeued when its distance from the start state is the smallest of all paths being searched
        //therefore, we only need to expand each node once
        if (expanded.find(current.second) == expanded.end()) {
            
            //expand the node by getting analyzing each of the neighbors
            for (Road r : current.second->neighbors()) {
                
                int roadLength = r.distance(); //arc cost
                Intersection* neighbor = r.secondIntersection(); //pointer to the neighbor being analyzed
                
                //the neighbor might already be in the queue so we need to test whether or not to update its references
                //if the neighbor is not in predecessor, then it has not been seen before, so we can add it to the queue
                //if the neighbor has been seen before but the current path to the node is shorter than the existing path, update the precessor to
                //      reflect the new shortest path and add the new shortest path information to the predecessor
                if ((predecessor.find(neighbor) == predecessor.end()) || ((currentDistance + roadLength) < predecessor.find(neighbor)->second.first)) {
                    
                    predecessor[neighbor] = make_pair(currentDistance + roadLength, current.second); //set current shortest distance, and parent=current node
                    node newNode(currentDistance + roadLength, neighbor); //create a new node and add it to the queue
                    queue.push(newNode);
                    
                }
                
            }
            
            //signal that the node was expanded so it will not be expanded in the future
            expanded[current.second]= true;
            
        }
        
    }
    
    //if we found the goal state then rebuild the path and return it
    if (found_goal_state){
        
        float distance = predecessor.at(goal).first; //distance from start to goal
        Intersection* current = goal; //start at the goal and work backwards using the predecessor pointer to the parent
        
        //build the path from the goal state until we reach the starting state
        //the predecessor of the starting state is null, so we stop once we have processed the starting state
        while(current != nullptr) {
            path.insert(path.begin(), current); //insert at the beginning so that the order is reversed (since we are starting at the end)
            current = predecessor[current].second; //go to the predecessor and repeat
        }
        return make_pair(path, distance); //return the path and the total path distance
    }
    else { //else return an empty path
        return make_pair(path, -1);
    }
    
    
    
}

