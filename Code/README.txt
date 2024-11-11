READ ME - TrafficTrak

These classes cover the logic on how traffic lights and intersections are set up and represented on a grander scale.
Following a centralized system, an overall controller controls a map. 
Parsers have been used to set up the map, and reading command line arguments. 
The map stores the intersections of a city. This would allow for a city planner to have intersections to communicate with each other, for general traffic flow and in the case of an emergency. 
The intersections(represented as nodes), are connected to roads(represented as edges), as well as a traffic lights. 
While the general set-up is given, for the future, each traffic light will have reference to a camera which continuously loops and sends back congestion scores using a video processor, which will send it back to the traffic light.This would allow a city official to view real time levels of congestion to be reflected accurately.
Databases are used to store congestion scores for each intersection. It can use average scores to predict scores for the future with the future goal of scheduling traffic lights accordingly. For future purposes, this will help city planners use predictions to switch traffic data at optimal intervals. 
Given the sensitive nature of traffic data and public infrastructure, the team has decided to also include exception classes to catch any errors and handle them appropriately. 
In order to run, use make to compile the program. 

cd into the directory containing the files, run make to compile the program, and make test
to run the program