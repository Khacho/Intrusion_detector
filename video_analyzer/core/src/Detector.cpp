// Class headers
#include "Detector.hpp"

// Headers from OpenCV
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

// Headers from system
#include <iostream>
#include <sstream>


VA::Detector::
Detector () {
}

VA::Detector::
~Detector() {
}

/**********************************************************
 * Public members
 **********************************************************/ 

bool VA::Detector::
loadClassifier() {
    std::string cascadeName = "../resources/cascadG.xml";
    if (!this->cascadeClassifier.load(cascadeName)) {
        std::cout << "ERROR: Could not load the following classifier" << std::endl;
        return false;
    }
    return true;
}

void VA::Detector::
detect(cv::Mat& img, std::vector<cv::Rect>& rects) {
    if (this->cascadeClassifier.empty()) {
        std::cout << "WAR: The classifier is not loaded!" << std::endl;
        return;
    }
    this->cascadeClassifier.detectMultiScale(
        img,
        rects,
        1.1,
        2,
        0 | 1,
        cv::Size(40,70),
        cv::Size(80, 300)
    );
}

