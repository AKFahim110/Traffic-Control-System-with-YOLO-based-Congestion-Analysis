
#include "ProcessedImage.hpp"

using namespace cv;
using namespace dnn;
using namespace std;
using namespace traffictrack;

vector<yolo_obj> northResult;
vector<yolo_obj> southResult;
vector<yolo_obj> eastResult;
vector<yolo_obj> westResult;
//vector<vector<yolo_obj>> results;
Mat img1;
Mat img2;
Mat img3;
Mat img4;
String priority;


/**
* @fn ProcessedImage()
* @brief constructor 
* @param nimage - image file name for the north traffic light
* @param simage - image file name for the south traffic light
* @param eimage - image file name for the east traffic light
* @param wimage - image file name for the west traffic light
* @returns void - nothing 
*/
ProcessedImage::ProcessedImage(String northImage, String southImage, String eastImage, String westImage){
    yolo new_yolo = yolo("yolov3.weights","coco.names","yolov3.cfg",416,416, 0.30);
    img1 = imread(northImage);
    northResult = new_yolo.processImage(img1);
    img2 = imread(southImage);
    southResult = new_yolo.processImage(img2);
    img3 = imread(eastImage);
    eastResult = new_yolo.processImage(img3);
    img4 = imread(westImage);
    westResult = new_yolo.processImage(img4);

}

/** @fn carCount()
*  @brief counts the number of cars turning left, right, or going straight 
*  
* This function uses the yolov3 models to count the cars in the left turning lanes, right turning lanes, 
* and the through lanes. It creates a congestionScore object to store this information and then 
* returns the object alongside the time as a DateScorePair.
* @return - returns a DateScorePair that stores the time and congestionScore
*/
DateScorePair ProcessedImage::carCount(){
     CongestionScore congestion;
    //north
    static int count[3] = {0,0,0};
   
    for(int i = 0; i < northResult.size(); i++){
        if(northResult[i].classID == "car" || northResult[i].classID == "bus" || northResult[i].classID == "truck" || northResult[i].classID == "motorbike"){
            if(northResult[i].boundingBox.y <= 333){
                count[0] = count[0]+1;
            }
            else if(northResult[i].boundingBox.y <=1000){
                count[1] = count[1]+1;
            }
            else{
                count[2] = count[2]+1;
            }
        }
    }
    congestion.setNorth(count[0],count[1],count[2]);


    count[0]=0;
    count[1]=0;
    count[2]=0;
    for(int i = 0; i < southResult.size(); i++){
        if(northResult[i].classID == "car"|| northResult[i].classID == "bus" || northResult[i].classID == "truck" ){
            if(southResult[i].boundingBox.y <= 333){
                count[0] = count[0]+1;
            }
            else if(southResult[i].boundingBox.y <=000){
                count[1] = count[1]+1;
            }
            else{
                count[2] = count[2]+1;
            }
        }
    }
    congestion.setSouth(count[0],count[1],count[2]);




    count[0]=0;
    count[1]=0;
    count[2]=0;
    for(int i = 0; i < eastResult.size(); i++){
        if(northResult[i].classID == "car" || northResult[i].classID == "bus" || northResult[i].classID == "truck"){
            if(eastResult[i].boundingBox.y <= 333){
                count[0] = count[0]+1;
            }
            else if(eastResult[i].boundingBox.y <=1000){
                count[1] = count[1]+1;
            }
            else{
                count[2] = count[2]+1;
            }
        }
    }
    congestion.setEast(count[0],count[1],count[2]);




    count[0]=0;
    count[1]=0;
    count[2]=0;
    for(int i = 0; i < westResult.size(); i++){
        if(northResult[i].classID == "car" || northResult[i].classID == "bus" || northResult[i].classID == "truck"){
            if(westResult[i].boundingBox.y <=333){
                count[0] = count[0]+1;
            }
            else if(westResult[i].boundingBox.y <= 1000){
                count[1] = count[1]+1;
            }
            else{
                count[2] = count[2]+1;
            }
        }
    }
    congestion.setWest(count[0],count[1],count[2]);

DateScorePair databaseResult;
time_t now = std::time(NULL);
string time = std::ctime(&now);
databaseResult.first = time;
databaseResult.second = congestion;
return databaseResult;


}



