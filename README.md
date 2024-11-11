# Traffic Control System with YOLO-Based Congestion Analysis

## Overview
This project aims to create a scalable, centralized traffic control system to manage intersections, traffic lights, and road congestion across a city. Using object detection technology (YOLO), the system continuously analyzes traffic congestion levels and updates traffic light operations accordingly. The project has been developed with scalability in mind, allowing cities to add intersections, adjust traffic flow, and prepare for real-time congestion monitoring.

## Key Features
- **Citywide Traffic Control**: Utilizes a centralized controller to manage a network of intersections, each represented as nodes connected by roads (edges).
- **Real-Time Congestion Monitoring**: YOLO-based video processing for congestion score calculation, which is then fed back into the system for real-time updates.
- **Data Storage & Prediction**: Databases store and analyze congestion data over time, allowing predictions to be made on traffic patterns and optimizing traffic light scheduling.
- **Emergency Traffic Flow**: The system is equipped to adapt in emergency scenarios, enabling smooth traffic redirection and priority changes.
- **Exception Handling**: Built-in exception classes ensure reliable error handling, especially given the critical nature of traffic control.

## Future Enhancements
The system is designed to be extensible with future goals including:
1. Integration of machine learning models to predict congestion and adjust traffic light timings dynamically.
2. Enhanced real-time visibility of congestion scores for city officials.
3. Security features to protect sensitive traffic data.

## Installation

### Prerequisites
1. **YOLO (You Only Look Once)**: The project utilizes YOLO for object detection in video streams. Make sure to install the necessary YOLO library and dependencies.
2. **Database Setup**: Configure the database to store congestion scores.
3. **C++ Compiler**: This project uses `make` for compilation, so ensure you have a C++ compiler installed.

### Steps
1. Clone the repository:
    ```bash
    git clone https://github.com/your-username/Traffic_Control_YOLO.git
    ```
2. Navigate to the project directory:
    ```bash
    cd Traffic_Control_YOLO
    ```
3. Compile the program using `make`:
    ```bash
    make
    ```
4. Run the program with command-line arguments:
    ```bash
    ./traffic_control <map_config> <additional_arguments>
    ```

## Usage
The system uses parsers to configure the map setup and manage command-line arguments. These parsers read a predefined city map, where intersections (nodes) and roads (edges) are defined along with initial traffic light configurations. 

Each traffic light node connects to a camera (to be implemented) that loops continuously, sending back real-time congestion scores. These scores are saved to a database, providing historical data and supporting future congestion prediction.

## Project Structure
- **Controller**: Manages overall traffic control and map configurations.
- **Parsers**: Parses map configurations and command-line arguments.
- **Intersection Nodes and Roads**: Defines nodes (intersections) and edges (roads) for the city's road network.
- **Traffic Lights**: Configures traffic lights, with plans to integrate camera inputs and YOLO-based congestion analysis.
- **Database**: Stores historical congestion data and prepares data for predictive analysis.
- **Exception Handling**: Ensures errors are handled efficiently to maintain system reliability.

## Contributing
Please feel free to submit issues, feature requests, or pull requests. Contributions that add to scalability, security, and efficiency are especially welcome!

## License
This project is licensed under the MIT License. See the LICENSE file for more details.

---

