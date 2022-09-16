#ifndef _IMAGE_PROCESS_H_
#define _IMAGE_PROCESS_H_

#include <fstream>
#include <opencv2/opencv.hpp>
#include <iostream>

#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <utility>

#include <pthread.h>
#include "yolo_v2_class.hpp"
#include "Yolo3Detection.h"

class imageProcessor{
    public:
    imageProcessor();
    cv::Mat ImageDetect(cv::Mat img, std::vector<int> &detret);     //目标检测
};

#endif