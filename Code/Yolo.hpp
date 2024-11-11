#ifndef Yolo_h
#define Yolo_h

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>

// opencv
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

/*objects identified in the image - 
bounding box of obj, class id - type of object, confidence - how close of a match to the class*/
typedef struct yolo_obj
{
    cv::Rect boundingBox;
    cv::String classID; // what type of obj was identified
    float confidence; 
}yolo_obj;

/**
 * @class yolo
 * @brief This class calls the yolo API to send images to the darknet (OPENCV) and receive a vector of yolo_obj 
 * 
 * The yolo object contains a boundingBox, type of obj and confidence. 
 * @authors Harkirat Bassi & Maanasa Pillai - Some parts of the code was taken from OpenSource 
 */
class yolo
{
    private: 
        int width, height; //dimensions of the image: this will be constant as when we process images we convert them into a constant dimension
        float confidence_threshold; //confidence threshold to accept object: also constant;   
        std::vector <cv::String> classes; // holds all class names from coco.names
        cv::dnn::Net net; //network (we forward blobs (pre processed images) to the network and it returns a whole bunch of data))
        std::vector <cv::String> unconnected_layers; //last layer - need this as param for forward() so that the netowrk uses all layers until the last layer
        std::vector <yolo_obj> final_objects_list; //stores the final yolo objects  post-processing

    public:
 
        yolo(const cv::String weightsFile, const cv::String classesFile, const cv::String configFile, const int width = 608, const int height = 608, const float confidenceThreshold = 0.5);
        
        ~yolo(); 

        std::vector<yolo_obj> processImage(cv::Mat img);

        std::vector<yolo_obj> getYoloObjs();





};


#endif 