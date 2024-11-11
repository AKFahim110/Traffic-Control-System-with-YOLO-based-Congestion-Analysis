#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include "AbstractPhotoTaker.hpp"

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


using namespace std;
using namespace cv;

/**
 * @class randomImg
 * @brief this class inherits from the AbstractPhotoTaker class to take a random photo from a file of photos
 * 
 * In real life this class would take a photo using the camera, however Since hardware is not being used for the sake of the project
 * this class will pull a random picture from a folder of traffic pictures and return the image file name
 * @authors Harkirat Bassi & Maanasa Pillai
 */

class RandomPhotoTaker : public AbstractPhotoTaker{
    
    vector<String> fn; //file names

    public:

    virtual String takePhoto();
    virtual ~RandomPhotoTaker();


};