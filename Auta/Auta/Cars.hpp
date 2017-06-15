//
//  Cars.h
//  CarTracking
//
//  Created by David KrÈnar on 26/10/16.
//  Copyright © 2016 David KrÈnar. All rights reserved.
//

#ifndef Cars_h
#define Cars_h

#include <iostream>
#include <opencv2/opencv.hpp>
#include "CarDetector.hpp"

class Cars {
    
public:
    Cars(std::string _videoFilename);
    void run();
    
private:
    cv::VideoCapture video;
    std::string videoFilename;
    cv::Mat videoFrame1;
    cv::Mat videoFrame2;
    
    std::vector<CarDetector> cars;
    
    cv::Point crossingLeftLine[2];
    cv::Point crossingRightLine[2];
    
    std::vector<std::vector<cv::Point>> contours;
    
    int leftCounter;
    int rightCounter;
    
    int photoCounter;
    //cv::String imagename;
    std::string imagename;
    std::string file;
    
    std::vector<int> compression_params; // vector that stores the compression parameters of the image
    
    //methods
    void matchCurrentCarsToExisting(std::vector<CarDetector> &existingCars, std::vector<CarDetector> &currentCars);
    void addCarToExisting(CarDetector &currentCar, std::vector<CarDetector> &existingCars, int &index);
    void addNewCar(CarDetector &currentCar, std::vector<CarDetector> &existingCars);
    
    double distanceBetweenPoints(cv::Point point1, cv::Point point2);
    
    void showContours(cv::Size imageSize, std::vector<std::vector<cv::Point>> contours, std::string strImageName);
    void drawCarContours(cv::Size imageSize, std::vector<CarDetector> cars, std::string strImageName);
    
    bool checkIfCarCrossedLeftLine(std::vector<CarDetector> &cars, int &linePosition, int &carLeftCount);
    bool checkIfCarCrossedRightLine(std::vector<CarDetector> &cars, int &linePosition, int &carRightCount);
    
    void drawCarInfo(std::vector<CarDetector> &cars, cv::Mat &image);
    void drawCarLeftCounter(int &carLeftCounter, cv::Mat &image);
    void drawCarRightCounter(int &carRightCounter, cv::Mat &image);
    
};

#endif /* Cars_h */
