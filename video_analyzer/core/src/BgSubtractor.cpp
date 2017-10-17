#include "BgSubtractor.hpp"

#include <iostream>


VA::BgSubtractor::
BgSubtractor () {
    this -> pMOG2 = cv::createBackgroundSubtractorMOG2();
}

VA::BgSubtractor::
~BgSubtractor() {}

/**********************************************************
 * Public members
 **********************************************************/

std::vector<cv::Rect> VA::BgSubtractor::
getForground( cv::Mat frame ) {
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::RNG rng(12345);

    // update the background model
    blur(frame,frame, cv::Size(7,7)); 
    pMOG2->apply(frame, foreground);
    cleanNoise(foreground);

    cv::findContours( foreground, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(1, 1) );
    
    // got rectangles for contours
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    std::vector<cv::Rect> boundRect( contours.size() );
    for( int i = 0; i < contours.size(); i++ ) { 
        cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, false ); 
        boundRect[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );
    }

    // drow contours nad rectangles
    cv::Mat drawing = cv::Mat::zeros( frame.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ ) {
        cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
        cv::rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
    }
    cv::imshow("Contours", drawing);
    cv::imshow("Foreground", foreground);
    return boundRect;
}

/**********************************************************
 * Private members
 **********************************************************/

void VA::BgSubtractor::
cleanNoise(cv::Mat& foreground) {
    int erosionType = cv::MORPH_RECT;
    int erosionSize = 2;
    cv::Mat erosionElement = cv::getStructuringElement(
            erosionType,
            cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
            cv::Point(erosionSize, erosionSize));
    
    erode(foreground, foreground, erosionElement); 
    dilate(foreground, foreground, erosionElement);
}
