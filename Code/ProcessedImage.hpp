#ifndef processedImg_h
#define processedImg_h
#include "Yolo.hpp"
#include "CongestionScore.hpp"
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>

#include "DateScorePair.hpp"



using namespace cv;
using namespace dnn;
using namespace std;
using namespace traffictrack;


    /** 
     * @class ProcessedImage.hpp
     *  @brief processes 4 images for each intersection to identify the number of cars going in each direction
     *  
     * Receives 4 images [north, south, east, west] through the constructor and uses the carCount function
     * to identify all objects in the photos and split the image into 3 parts [left lane, straight lane, right lane] 
     * and count the number of cars in each lane. 
     * @author Harkirat Bassi & Maanasa Pillai
     */
class ProcessedImage{
    private:
        vector<yolo_obj> result;
        Mat img;
    public:

        ProcessedImage(String nimage, String simage, String eimage, String wimage);


        DateScorePair carCount();
};

#endif