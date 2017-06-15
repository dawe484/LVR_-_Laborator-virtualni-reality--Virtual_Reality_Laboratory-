//
//  Cars.cpp
//  CarTracking
//
//  Created by David Krénar on 26/10/16.
//  Copyright © 2016 David Krénar. All rights reserved.
//

#include "Cars.hpp"

// global variables
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

Cars::Cars(std::string _videoFilename) {
    
    leftCounter = 0; // initialize counter for cars that go in left lane
    rightCounter = 0; // initialize counter for cars that go in right lane
    
    photoCounter = 0; // initialize image counter
    
    //for (unsigned int i = 1; i < 20; ++i) {
    //	std::string file = (std::string("auta\\") + std::to_string(i) + ".jpg");
    //	//wchar_t number = i;
    //	char* cfile = new char[file.length()];
    //	for (int j = 0; j < file.length(); ++j) {
    //		cfile[j] = file[j];
    //	}
    //	std::cout << cfile << std::endl;
    //	//std::remove(cfile);
    ////	//std::remove("");
    //}
    
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); // specify the compression technique
    
    compression_params.push_back(90); // specify the compression quality
    
    video = cv::VideoCapture(_videoFilename); // initialize video file
    videoFilename = _videoFilename;
}

void Cars::run() {
    
    video.open(videoFilename); // open video file, http://docs.opencv.org/3.1.0/d8/dfe/classcv_1_1VideoCapture.html#a473055e77dd7faa4d26d686226b292c1
    
    if (!video.isOpened()) {  // check if we succeeded
        std::cout << "Error reading video file." << std::endl;
        return;
    }
    
    video.read(videoFrame1); // grabs, decodes and returns next video frame = combine VideoCapture::grab and VideoCapture::retrieve in one call.
    video.read(videoFrame2);
    
    int frameCount = 2; // indicate how many frames was read
    
    // skutecne projeta auta 8/9 (vpravo prvni nepocitam) - 94,117647
    int linePosition = 256; // vpravo se prvni auto nepocita, neprojede zatim pres caru ->
    // 244-9/9; 246-11/10; 248-8/8; 249-8/8;
    // 250-8/8; 251-9/9; 252-10/10; 253-9/9; 254-9/9; 255-9/9; 256-8/8; 257-8/8; 258-10/10;
    // vpravo uz auto projede carou a zapocita se
    // 260-12/12; 262-12/12; 264-12/12; 266-11/11; 268-13/13;
    // 270-12/12; 272-15/15; 274-14/14; 276-14/14; 278-17/16;
    // 280-16/16
    
    crossingLeftLine[0].x = 234;
    crossingLeftLine[0].y = linePosition;
    
    crossingLeftLine[1].x = 302;
    crossingLeftLine[1].y = linePosition;
    
    crossingRightLine[0].x = 322;
    crossingRightLine[0].y = linePosition;
    
    crossingRightLine[1].x = 392;
    crossingRightLine[1].y = linePosition;
    
    bool firstFrame = true;
    
    char checkEsc = 0;
    
    int elementSize = 4;
    
    while (video.isOpened() && checkEsc != 27) {
        
        std::vector<CarDetector> currentFrameCars; // cars that are in current frame of videofile
        
        cv::Mat videoFrame1Copy = videoFrame1.clone(); // not allowed to modify tthe image, must copy and then do whatever you want
        cv::Mat videoFrame2Copy = videoFrame2.clone();
        
        cv::Mat imgDifference;
        cv::Mat imgThresh;
        
        // threshhold /////////////////////////////////////////////////////////
        cv::cvtColor(videoFrame1Copy, videoFrame1Copy, CV_BGR2GRAY); // rgb image to gray image
        cv::cvtColor(videoFrame2Copy, videoFrame2Copy, CV_BGR2GRAY);
        
        // http://docs.opencv.org/3.1.0/d4/d13/tutorial_py_filtering.html
        cv::GaussianBlur(videoFrame1Copy, videoFrame1Copy, cv::Size(5, 5), 0); // eliminate noises in an image
        cv::GaussianBlur(videoFrame2Copy, videoFrame2Copy, cv::Size(5, 5), 0);
        
        cv::absdiff(videoFrame1Copy, videoFrame2Copy, imgDifference); //Calculates the per-element absolute difference between two arrays.
        
        cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);
        
        cv::imshow("imgThresh", imgThresh); // show threshold image
        
        for (unsigned int i = 0; i < 2; ++i) {
            // closing
            cv::dilate(imgThresh, imgThresh, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(elementSize, elementSize)));
            cv::dilate(imgThresh, imgThresh, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(elementSize, elementSize)));
            cv::erode(imgThresh, imgThresh, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(elementSize, elementSize)));
        }
        
        cv::imshow("closing imgThresh", imgThresh);
        
        cv::Mat imgThreshCopy = imgThresh.clone();
        
        // contours ///////////////////////////////////////////////////////////
        // http://docs.opencv.org/3.1.0/d3/dc0/group__imgproc__shape.html#ga17ed9f5d79ae97bd4c7cf18403e1689a
        cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        showContours(imgThresh.size(), contours, "imgContours");
        
        // convexHulls ////////////////////////////////////////////////////////
        // http://docs.opencv.org/3.1.0/d3/dc0/group__imgproc__shape.html#ga014b28e56cb8854c0de4a211cb2be656
        std::vector<std::vector<cv::Point>> convexHulls(contours.size());
        
        for (unsigned int i = 0; i < contours.size(); ++i) {
            cv::convexHull(contours[i], convexHulls[i]); // finds the convex hull of a point set
            // https://cs.wikipedia.org/wiki/Konvexn%C3%AD_mno%C5%BEina
        }
        
        showContours(imgThresh.size(), convexHulls, "imgConvexHulls");
        
        for (auto &convexHull : convexHulls) {
            
            CarDetector possibleCar(convexHull);
            
            // define what is car and what's not
            if (possibleCar.currentBoundingRect.area() > 400 &&
                possibleCar.currentBoundingRect.width > 30 &&
                possibleCar.currentBoundingRect.height > 30 &&
                possibleCar.currentDiagonalSize > 60.0
                ) {
                
                currentFrameCars.push_back(possibleCar);
            }
        }
        
        drawCarContours(imgThresh.size(), currentFrameCars, "currentFrameCars");
        
        if (firstFrame == true) {
            for (auto &currentFrameCar : currentFrameCars) {
                cars.push_back(currentFrameCar);
            }
        }
        else {
            matchCurrentCarsToExisting(cars, currentFrameCars);
        }
        
        drawCarContours(imgThresh.size(), cars, "imgCars");
        
        videoFrame2Copy = videoFrame2.clone(); // get another copy of frame 2 since we changed the previous frame 2 copy in the processing above
        
        drawCarInfo(cars, videoFrame2Copy);
        
        bool leastOneCarCrossedLeftLine = checkIfCarCrossedLeftLine(cars, linePosition, leftCounter);
        
        if (leastOneCarCrossedLeftLine == true) {
            cv::line(videoFrame2Copy, crossingLeftLine[0], crossingLeftLine[1], SCALAR_BLUE, 2);
            
            photoCounter++;
            std::cout << std::to_string(photoCounter) << std::endl;
            imagename = "auta/" + std::to_string(photoCounter) + ".jpg";
            std::cout << imagename << std::endl;
            cv::imwrite(imagename, videoFrame2Copy, compression_params);
            std::cout << "file saved." << std::endl;
            
        }
        else {
            cv::line(videoFrame2Copy, crossingLeftLine[0], crossingLeftLine[1], SCALAR_RED, 2);
        }
        
        bool leastOneCarCrossedRightLine = checkIfCarCrossedRightLine(cars, linePosition, rightCounter);
        
        if (leastOneCarCrossedRightLine == true) {
            cv::line(videoFrame2Copy, crossingRightLine[0], crossingRightLine[1], SCALAR_BLUE, 2);
            
            photoCounter++;
            std::cout << std::to_string(photoCounter) << std::endl;
            imagename = "auta/" + std::to_string(photoCounter) + ".jpg";
            std::cout << imagename << std::endl;
            cv::imwrite(imagename, videoFrame2Copy, compression_params);
            std::cout << "file saved." << std::endl;
        }
        else {
            cv::line(videoFrame2Copy, crossingRightLine[0], crossingRightLine[1], SCALAR_RED, 2);
        }
        
        drawCarLeftCounter(leftCounter, videoFrame2Copy);
        drawCarRightCounter(rightCounter, videoFrame2Copy);
        
        cv::rectangle(videoFrame2Copy, cv::Point(130, 170), cv::Point(510, 430), SCALAR_RED, 5);
        cv::imshow("imgFrame2Copy", videoFrame2Copy);
        
        currentFrameCars.clear();
        
        videoFrame1 = videoFrame2.clone(); // move videoFrame1 up to where videoFrame2 is
        
        if ((video.get(CV_CAP_PROP_POS_FRAMES) + 1) < video.get(CV_CAP_PROP_FRAME_COUNT)) {
            video.read(videoFrame2);
        }
        else {
            std::cout << "end of video\n" << std::endl;
            break;
        }
        
        firstFrame = false;
        frameCount++;
        checkEsc = cv::waitKey(1);
    }
    
    if (checkEsc != 27) {	// if the user did not press esc
        cv::waitKey(0);
    }
}

void Cars::matchCurrentCarsToExisting(std::vector<CarDetector> &existingCars, std::vector<CarDetector> &currentCars) {
    
    for (auto &existingCar : existingCars) {
        
        existingCar.currentMatchFoundOrNewCar = false;
        
        existingCar.predictNextPosition();
    }
    
    for (auto &currentCar : currentCars) {
        
        int indexOfLeastDistance = 0;
        double leastDistance = INFINITY;
        
        for (unsigned int i = 0; i < existingCars.size(); ++i) {
            
            if (existingCars[i].stillBeingTracked == true) {
                
                double distance = distanceBetweenPoints(currentCar.centerPositions.back(), existingCars[i].predictedNextPosition);
                
                if (distance < leastDistance) {
                    leastDistance = distance;
                    indexOfLeastDistance = i;
                }
            }
        }
        
        if (leastDistance < currentCar.currentDiagonalSize * 0.5) {
            addCarToExisting(currentCar, existingCars, indexOfLeastDistance);
        }
        else {
            addNewCar(currentCar, existingCars);
        }
    }
    
    for (auto &existingCar: existingCars) {
        
        if (existingCar.currentMatchFoundOrNewCar == false) {
            existingCar.numOfConsecutiveFramesWithoutAMatch++;
        }
        
        if (existingCar.numOfConsecutiveFramesWithoutAMatch >= 5) {
            existingCar.stillBeingTracked = false;
        }
    }
}

void Cars::addCarToExisting(CarDetector &currentCar, std::vector<CarDetector> &existingCars, int &index) {
    
    existingCars[index].currentContour = currentCar.currentContour;
    existingCars[index].currentBoundingRect = currentCar.currentBoundingRect;
    
    existingCars[index].centerPositions.push_back(currentCar.centerPositions.back());
    
    existingCars[index].currentDiagonalSize = currentCar.currentDiagonalSize;
    
    existingCars[index].stillBeingTracked = true;
    existingCars[index].currentMatchFoundOrNewCar = true;
}

void Cars::addNewCar(CarDetector &currentCar, std::vector<CarDetector> &existingCars) {
    
    currentCar.currentMatchFoundOrNewCar = true;
    
    existingCars.push_back(currentCar);
}

double Cars::distanceBetweenPoints(cv::Point point1, cv::Point point2) {
    
    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);
    
    return (sqrt(pow(intX, 2) + pow(intY, 2)));
}

void Cars::showContours(cv::Size imageSize, std::vector<std::vector<cv::Point>> contours, std::string strImageName) {
    
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK); // CV_8UC3 is an 8-bit unsigned integer matrix/image with 3 channels
    
    // http://docs.opencv.org/3.1.0/d6/d6e/group__imgproc__draw.html#ga746c0625f1781f1ffc9056259103edbc
    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1); // contourIdx - negative, all the contours are drawn
    
    cv::imshow(strImageName, image);
}

void Cars::drawCarContours(cv::Size imageSize, std::vector<CarDetector> cars, std::string strImageName) {
    
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
    
    std::vector<std::vector<cv::Point>> contours;
    
    for (auto &car : cars) {
        if (car.stillBeingTracked == true) {
            contours.push_back(car.currentContour);
        }
    }
    
    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1); // contourIdx - negative, all the contours are drawn
    
    cv::imshow(strImageName, image);
}

bool Cars::checkIfCarCrossedLeftLine(std::vector<CarDetector> &cars, int &linePosition, int &carLeftCount) {
    
    bool leastOneCarCrossedLine = false;
    
    for (auto &car : cars) {
        
        if (car.stillBeingTracked == true && car.centerPositions.size() >= 2) {
            int prevFrameIndex = (int)car.centerPositions.size() - 2;
            int currFrameIndex = (int)car.centerPositions.size() - 1;
            
            if (car.centerPositions[prevFrameIndex].y > linePosition && car.centerPositions[currFrameIndex].y <= linePosition) {
                carLeftCount++;
                leastOneCarCrossedLine = true;
            }
        }
    }
    
    return leastOneCarCrossedLine;
}

bool Cars::checkIfCarCrossedRightLine(std::vector<CarDetector> &cars, int &linePosition, int &carRightCount) {
    
    bool leastOneCarCrossedLine = false;
    
    for (auto &car: cars) {
        
        if (car.stillBeingTracked == true && car.centerPositions.size() >= 2) {
            int prevFrameIndex = (int)car.centerPositions.size() - 2;
            int currFrameIndex = (int)car.centerPositions.size() - 1;
            
            if (car.centerPositions[prevFrameIndex].y <= linePosition && car.centerPositions[currFrameIndex].y > linePosition) {
                carRightCount++;
                leastOneCarCrossedLine = true;
            }
        }
    }
    
    return leastOneCarCrossedLine;
}

void Cars::drawCarInfo(std::vector<CarDetector> &cars, cv::Mat &image) {
    
    for (unsigned int i = 0; i < cars.size(); ++i) {
        
        if (cars[i].stillBeingTracked == true) {
            cv::rectangle(image, cars[i].currentBoundingRect, SCALAR_RED, 2);
            
            cv::putText(image, ".", cars[i].centerPositions.back(), CV_FONT_HERSHEY_SIMPLEX, 1, SCALAR_GREEN, 6);
        }
    }
}

void Cars::drawCarLeftCounter(int &carLeftCounter, cv::Mat &image) {
    cv::putText(image, "Levy pruh: " + std::to_string(carLeftCounter), cv::Point(14, 40), cv::FONT_HERSHEY_PLAIN, 2, SCALAR_BLUE, 2);
}

void Cars::drawCarRightCounter(int &carRightCounter, cv::Mat &image) {
    cv::putText(image, "Pravy pruh: " + std::to_string(carRightCounter), cv::Point(470, 40), cv::FONT_HERSHEY_PLAIN, 2, SCALAR_BLUE, 2);
}
