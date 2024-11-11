#include "RandomPhotoTaker.hpp"
#include <time.h>


vector<String> fn;

/**
* @fn takePhoto()
* @brief returns a random image from the photos folder
* This function uses the rand function to randomly select photos from the photo folder to simulate taking 
* pictures in real life. This overrides the AbstractPhotoTaker class so that functions can be added in the future
* to take realtime pictures and process them instead.
* @return String - Returns the filename of the jpg that will be opened
*/

String RandomPhotoTaker::takePhoto(){
    glob("./photos/*.jpg", fn, false);
    int randNum = rand() % fn.size();
    return fn.at(randNum);


}
/**
 * @fn ~RandomPhotoTaker
 * @brief deconstructor
 */
RandomPhotoTaker::~RandomPhotoTaker() { }
