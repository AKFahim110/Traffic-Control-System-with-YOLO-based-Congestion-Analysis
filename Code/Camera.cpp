//
//  Camera.cpp
//  TraffikTrak


#include <string>
#include "Camera.hpp"
#include "AbstractPhotoTaker.hpp"
#include "RandomPhotoTaker.hpp"


/** @fn Camera()
 *  @brief initializes default values
 */
Camera::Camera() {
    
    streamIsOpen_ = false;
    photoTaker_ = new RandomPhotoTaker();
    
}


/** @fn ~Camera()
 *  @brief frees the photoTaker memory
 */
Camera::~Camera() {
    
    if (photoTaker_ != nullptr) {
        delete photoTaker_;
    }
    
}


/** @fn openVideoStream()
 *  @brief opens the video stream. this is a placeholder and doesn't do anything in this implementation
 *  @return bool returns whether the operation was successful
 */
bool Camera::openVideoStream() {
    if (!streamIsOpen_) {
        streamIsOpen_ = true;
    }
    return true;
}


/** @fn closeVideoStream()
 *  @brief closes the video stream. as with openVideoStream(), it is a placeholder
 *  @return bool returns whether the operation was successful
 */
bool Camera::closeVideoStream() {
    if (streamIsOpen_) {
        streamIsOpen_ = false;
    }
    return true;
}


/** @fn takePhoto()
 *  @brief takes a photo using delegation with the photoTaker
 *  @return string returns the file name of the photo to be processed
 */
std::string Camera::takePhoto() {
    
    return photoTaker_->takePhoto();
    
}


