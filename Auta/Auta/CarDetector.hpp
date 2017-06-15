//
//  CarDetector.h
//  CarTracking
//
//  Created by David KrÈnar on 18/11/16.
//  Copyright © 2016 David KrÈnar. All rights reserved.
//

#ifndef CarDetector_h
#define CarDetector_h

#include <stdio.h>
#include <opencv2/opencv.hpp>

class CarDetector {
    
public:
    CarDetector(std::vector<cv::Point> _contour);
    
    std::vector<cv::Point> currentContour;
    cv::Rect currentBoundingRect;
    std::vector<cv::Point> centerPositions;
    
    double currentDiagonalSize;
    
    bool currentMatchFoundOrNewCar;
    bool stillBeingTracked;
    
    int numOfConsecutiveFramesWithoutAMatch;
    
    cv::Point predictedNextPosition;
    
    void predictNextPosition();
};

#endif /* CarDetector_h */
