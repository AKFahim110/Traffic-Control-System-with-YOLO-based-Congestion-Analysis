Testing for the computer vision:
The test file contains multiple images that program will process and return a congesetion score for. 

Please download the "install" folder from bitbucket 

Replace the following two lines in the CMakeLists.txt... :

set(OPENCV_DIR /Users/maan/Desktop/install/lib/cmake/opencv4)
set(CMAKE_PREFIX_PATH /Users/maan/Desktop/install/lib/cmake/opencv4)

....WITH:

set(OPENCV_DIR *path to where you store the install folder*...install/lib/cmake/opencv4)
set(CMAKE_PREFIX_PATH *path to where you store the install folder*...install/lib/cmake/opencv4install/lib/cmake/opencv4)


To build and run the program start inside the build folder. 
Run the following commands
    "cmake ./"
    "make"
    "./computerVision"

The expected outputs for the images are as follows
car.jpg -> Low congestion score
popcars.jpg -> Medium congestion score
intersectionCars.jpg -> High congestion score
crazy.jpg -> High congestion score ** This will not produce correct output **

The final test on crazy.jpg outlines the limitations of using yolo-tiny as opposed to yolo
We need to keep this under consideration when sending images to be processed becuase the rasberry Pi can only support yolo-tiny so clear images must be taken where the cars are not too small!

Feel free to add images of cars you would like to process. Add the images to the build folder and then edit any processedImg function.
processedImg("YOUR_IMAGE_HERE.jpg")
