#include "Yolo.hpp"

using namespace cv;
using namespace dnn;
using namespace std;

/**
* @fn yolo()
* @brief constructor 
* @param weightsFile - yolov3.weights or yolov3-tiny.weights
* @param classesFile - coco.names
* @param configFile - yolov3.cfg or yolov3-tiny.cfg
* @param width - width of the image - default is 608
* @param height - length of image - default is 608
* @param confidenceThreshold - the probability that the identification is right - default 50%
* @returns void 
*/
yolo::yolo(const String weightsFile, const String classesFile, const String configFile, const int width, const int height, const float confidenceThreshold){
    
    this-> width = width;
    this-> height = height;
    this-> confidence_threshold = confidenceThreshold;
    
    ifstream file; //file to read
    String line; //variable to store strings
    file.open("coco.names"); //open classes file coco.names

    //read every line in file and store in vector classes
    while(getline(file,line)){
        this->classes.push_back(line);
    }

    // create the network
    this->net = readNetFromDarknet(configFile, weightsFile);
    this->net.setPreferableBackend(DNN_BACKEND_DEFAULT);
    this->net.setPreferableTarget(DNN_TARGET_CPU);

    /* The forward function in OpenCV’s Net class needs the ending layer till which it should run in the network. 
    Since we want to run through the whole network, we need to identify the last layer of the network. 
    We do that by using the function getUnconnectedOutLayers() that gives the names of the unconnected output layers, 
    which are essentially the last layers of the network. */
   this-> unconnected_layers = this -> net.getUnconnectedOutLayersNames();

}


/**
* @fn ~yolo()
* @brief deconstructor 
*/
yolo::~yolo(){}



/**
* @fn processImage()
* @brief processes the image to return identify all objects in the image
* @param img - a jpg image
* @returns A list of yolo_obj (refer to struct above) that were identified from the image)
*/
std::vector<yolo_obj> yolo::processImage(Mat img){
    Mat blob; //variable to store image converted to blob
    blobFromImage(img, blob, 1/255.0, Size(this->width, this->height),Scalar(0,0,0), true, false ); //convert image to blob
    net.setInput(blob);
    std::vector<cv::Mat> net_output; //vector to store predictions
    net.forward(net_output,unconnected_layers); //run netowrk

    //vectors to store information
    vector <float> confidences;
    vector <int> classIDs;
    vector <cv::Rect> boundingBoxes;

    //iterate through predictions in net_output to store predictions with confidence >confidence threshold
    for(int i = 0; i < net_output.size(); i++) {
        float* data = (float*)net_output[i].data;
        for(int j = 0; j < net_output[i].rows; j++) {
            float objectConfidence= data[4];
            if (objectConfidence > this->confidence_threshold){
                Mat classPredictions = net_output[i].row(j).colRange(5,net_output[i].cols);
                Point maxPoint;
                double maxVal;
                minMaxLoc(classPredictions, 0, &maxVal, 0, &maxPoint);

                int centerX = (int)(data[0] * img.cols);
                int centerY = (int)(data[1] * img.rows);
                int boxWidth = (int)(data[2] * img.cols);
                int boxHeight = (int)(data[3] * img.rows);

                confidences.push_back(maxVal);
                classIDs.push_back(maxPoint.x);
                boundingBoxes.push_back(Rect(centerX, centerY, boxWidth, boxHeight));

            }
            data += net_output[i].cols; 
    }
}

  // remove the bounding boxes indicate the same object using NMS
    std::vector<int> indices;
    NMSBoxes(boundingBoxes, confidences, this->confidence_threshold, 0.3, indices);
    // save bounding boxes
    this->final_objects_list.resize(indices.size());
    for(int i = 0; i < indices.size(); i++) {
        int idx = indices[i];
        CV_Assert(classIDs[idx] < this->classes.size());
		yolo_obj object;
		object.boundingBox = boundingBoxes[idx];
		object.classID = this->classes[classIDs[idx]];
		object.confidence = confidences[idx];
        
        this->final_objects_list[i] = object;
    }
    return this->final_objects_list;
}



/**
* @fn processImage()
* @brief getter method: extra method so you dont have to process image every time you want to access the list;
* @returns A list of yolo_obj (refer to struct above) that were identified from the image)
*/
std::vector<yolo_obj> yolo::getYoloObjs(){
    return this->final_objects_list;
}




