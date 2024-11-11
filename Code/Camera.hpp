//
//  Camera.hpp
//  TraffikTrak

#ifndef Camera_hpp
#define Camera_hpp

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


class AbstractPhotoTaker;

/** @class Camera
 *  @brief opens a video stream, takes a photo using abstract photo taker
 *  @author Matthew Lovick
 */
class Camera {
    
protected:
    bool streamIsOpen_;
    AbstractPhotoTaker* photoTaker_;
    
public:
    Camera();
    virtual ~Camera(); //photoTaker should be send in as parameter from constructor
    bool isOpen();
    virtual bool openVideoStream();
    virtual bool closeVideoStream();
    virtual cv::String takePhoto();
    
};

#endif /* Camera_hpp */
