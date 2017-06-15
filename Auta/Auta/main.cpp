//
//  main.cpp
//  CarTracking
//
//  Created by David KrÈnar on 26/10/16.
//  Copyright © 2016 David KrÈnar. All rights reserved.
//

#include <iostream>
#include "Cars.hpp"

int main(int argc, const char * argv[]) {
    
    Cars cars("car_dataset.avi");
    cars.run();
    
    return 0;
}