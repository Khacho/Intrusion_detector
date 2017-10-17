// Class headers
#include "VideoProcessor.hpp"
#include "Detector.hpp"
#include "ArgumentValidater.hpp"

// Headers from OpenCV
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

// Headers from system
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include <ctime>

#include <sys/types.h>
#include <dirent.h>
#include <vector>

#define MAX_FAIL_FRAMES_COUNT 30

VA::VideoProcessor::
VideoProcessor() {
    this->frameIndex = 0;
    // this->http = new HTTP("");
    for (int i = 0; i < 4; ++i) {
        this->points[0][i] = cv::Point(0,0);
    }
}

VA::VideoProcessor::
~VideoProcessor() {
    // delete this->http;
}

/**********************************************************
 * Public members
 **********************************************************/

bool VA::VideoProcessor::
run() {
    utils::ArgumentValidater* arguments = utils::ArgumentValidater::getInstance();
    cv::String videoPath = arguments->getVideoPath();
    bool isShowWindow = arguments->isShowEnable();
    cv::String keepImagesPath = arguments->getKeepImagesPath();
    if (!(arguments->isFileExists(videoPath)) || !this->videoCapture.open(videoPath)) {
        std::cout << "Faled to open the following video file: " << videoPath << std::endl;
        return false;
    }
    
    this->videoCapture.set(CV_CAP_PROP_FPS, 10);
    
    if (isShowWindow) {
        this->createNamedWindow();
    }

    cv::Mat origin;
    int wrongFramesCount = 0;
    int sliderVal[4] = {0,0,0,0};
    if (!this->detector.loadClassifier()) {
        return false;
    }
    do {
        if (!this->videoCapture.read(origin)) {
            wrongFramesCount++;
            if (MAX_FAIL_FRAMES_COUNT <= wrongFramesCount) {
                std::cout << "Video capture fail\n";
                break;
            }
            continue;
        }
        wrongFramesCount = 0;
        this->frameIndex++;
        std::vector<cv::Rect> rects;
        cv::Mat currentImage;
        // Process the current frame
        cv::resize(origin, origin, cv::Size(640, 480));
        origin.copyTo(currentImage);
        fillPolyTopBottom(currentImage);
        fillPolyLeftRight(currentImage);         
        std::vector<cv::Rect> boundRect = bgSubtractor.getForground(currentImage);
        cv::cvtColor(currentImage, currentImage, CV_BGR2GRAY);
        // Detects
        this->detector.detect(currentImage, rects);
        // Draw detected objects
        drawObjects(rects, boundRect, origin);
        if (isShowWindow) {
            cv::imshow("showDetect", origin);
        }

        if (keepImagesPath != ".") { 
            mkdir(keepImagesPath.c_str(), 0777);
            std::string folderName = generateFolderName(keepImagesPath );
            this->createFolder(rects, origin,  folderName);
        }
    } while((char)cv::waitKey(10) != 'q');
    return true;
}   

/********************************************************** 
 * Private members
 **********************************************************/ 

void VA::VideoProcessor::
drawObjects(std::vector<cv::Rect> rects, std::vector<cv::Rect> boundRect, cv::Mat& origin) {
    for (int i = 0; i < rects.size(); ++i) {
        for (int j = 0; j < boundRect.size(); ++j) {
            if ((rects[i].br().x + rects[i].tl().x) / 2 > boundRect[j].tl().x 
                    && (rects[i].br().x + rects[i].tl().x) / 2 < boundRect[j].br().x
                    && (rects[i].br().y + rects[i].tl().y) / 2 > boundRect[j].tl().y 
                    && (rects[i].br().y + rects[i].tl().y) / 2 < boundRect[j].br().y ) {
                cv::rectangle(origin, rects[i].tl(), rects[i].br(), cv::Scalar(255,0,0), 2, 8, 0);
                break;
            }
        }
    }
    for (int i = 0; i < 3; ++i) {
        cv::line(origin, points[0][i], points[0][i + 1], cv::Scalar(0, 221, 55),2,8,0);
    }
    cv::line(origin, points[0][3], points[0][0], cv::Scalar(0, 221, 55),2,8,0);
}

void VA::VideoProcessor::
fillPolyTopBottom(cv::Mat& image) {
    cv::Point topPoints[1][4];
    cv::Point bottomPoints[1][4];
    topPoints[0][0] = cv::Point(0,0);
    topPoints[0][1] = cv::Point(640,0);
    bottomPoints[0][0]= cv::Point(0,480);
    bottomPoints[0][1]= cv::Point(640,480);
    if (points[0][0].y < points[0][1].y) {
        topPoints[0][2] = points[0][0];
        topPoints[0][3] = points[0][1];
    } else {
        topPoints[0][3] = points[0][0];
        topPoints[0][2] = points[0][1];
    }
    for (int i = 2; i < 4; ++i) {
        if (topPoints[0][2].y >= points[0][i].y) {
            topPoints[0][3] = topPoints[0][2];
            topPoints[0][2] = points[0][i];
        } else if(topPoints[0][3].y > points[0][i].y) {
            topPoints[0][3] = points[0][i];
        }
    }
    if (topPoints[0][3].x > topPoints[0][2].x) {
        std::swap(topPoints[0][2], topPoints[0][3]);
    }
    for (int i = 0; i < 4; ++i) {
        if (points[0][i] != topPoints[0][2] && points[0][i] != topPoints[0][3]) {
            bottomPoints[0][2] = points[0][i];
            for (int j = 0; j < 4; ++j) {
                if(points[0][j] != topPoints[0][2] && points[0][j] != topPoints[0][3] && points[0][j] != bottomPoints[0][2]) {
                    bottomPoints[0][3] = points[0][j];
                }
            }
        }
    }
    if (bottomPoints[0][3].x > bottomPoints[0][2].x) {
        std::swap(bottomPoints[0][2], bottomPoints[0][3]);
    }
    const cv::Point* topPointsList[1] = { topPoints[0] };
    const cv::Point* bottomPointsList[1] = { bottomPoints[0] };
    int num_points = 4;
    if (points[0][0] != cv::Point(0,0) && points[0][1] != cv::Point(0,0) && 
            points[0][2] != cv::Point(0,0) && points[0][3] != cv::Point(0,0)) {
        cv::fillPoly( image, topPointsList, &num_points, 1,  cv::Scalar( 255, 255, 255 ),8);
        cv::fillPoly( image, bottomPointsList, &num_points, 1,  cv::Scalar( 255, 255, 255 ),8);
    }
}

void VA::VideoProcessor::
fillPolyLeftRight(cv::Mat& image) {
    cv::Point leftPoints[1][4];
    cv::Point rightPoints[1][4];
    leftPoints[0][0] = cv::Point(0,0);
    leftPoints[0][1] = cv::Point(0,480);
    rightPoints[0][0]= cv::Point(640,0);
    rightPoints[0][1]= cv::Point(640,480);
    if (points[0][0].x < points[0][1].x) {
        leftPoints[0][2] = points[0][0];
        leftPoints[0][3] = points[0][1];
    } else {
        leftPoints[0][3] = points[0][0];
        leftPoints[0][2] = points[0][1];
    }
    for (int i = 2; i < 4; ++i) {
        if (leftPoints[0][2].x >= points[0][i].x) {
            leftPoints[0][3] = leftPoints[0][2];
            leftPoints[0][2] = points[0][i];
        } else if(leftPoints[0][3].x > points[0][i].x) {
            leftPoints[0][3] = points[0][i];
        }
    }
    if (leftPoints[0][3].y > leftPoints[0][2].y) {
        std::swap(leftPoints[0][2], leftPoints[0][3]);
    }
    for (int i = 0; i < 4; ++i) {
        if (points[0][i] != leftPoints[0][2] && points[0][i] != leftPoints[0][3]) {
            rightPoints[0][2] = points[0][i];
            for (int j = 0; j < 4; ++j) {
                if(points[0][j] != leftPoints[0][2] && points[0][j] != leftPoints[0][3] && points[0][j] != rightPoints[0][2]) {
                    rightPoints[0][3] = points[0][j];
                }
            }
        }
    }
    if (rightPoints[0][3].y > rightPoints[0][2].y) {
        std::swap(rightPoints[0][2], rightPoints[0][3]);
    }
    const cv::Point* leftPointsList[1] = { leftPoints[0] };
    const cv::Point* rightPointsList[1] = { rightPoints[0] };
    int num_points = 4;
    if (points[0][0] != cv::Point(0,0) && points[0][1] != cv::Point(0,0) && 
            points[0][2] != cv::Point(0,0) && points[0][3] != cv::Point(0,0)) {
        cv::fillPoly( image, leftPointsList, &num_points, 1,  cv::Scalar( 255, 255, 255 ),8);
        cv::fillPoly( image, rightPointsList, &num_points, 1,  cv::Scalar( 255, 255, 255 ),8);
    }
}



std::string VA::VideoProcessor::
generateFolderName(cv::String folderName) const {
    std::ostringstream s;
    s << "./" << folderName << "/folder" << this->frameIndex;
    return s.str();
}

void VA::VideoProcessor::
createNamedWindow() const {
    cv::namedWindow("showDetect", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("showDetect",0, 0);
    cv::setMouseCallback("showDetect", CallBackFunc, (void*) points[0]);	
    cv::namedWindow("Contours", cv::WINDOW_NORMAL);
    cv::resizeWindow("Contours",400,300);
    cv::moveWindow("Contours",700, 350);
    cv::namedWindow("Foreground", cv::WINDOW_NORMAL);
    cv::resizeWindow("Foreground",400,300);
    cv::moveWindow("Foreground",700, 0);
}

void VA::VideoProcessor::
createFolder(const std::vector<cv::Rect> rects, const cv::Mat& original, const std::string& folderName ) const {
    if (rects.size() > 0) { 
        int res = mkdir(folderName.c_str(), 0777);
        this->saveImages(original, folderName);
    }
}

void VA::VideoProcessor::
saveImages (const cv::Mat& original, const std::string& folderName ) const {
    std::ostringstream s;
    s << "./" << folderName << "/image" << this->frameIndex << ".jpg";
    std::string name = s.str();
    cv::imwrite(name, original);

    ///////////////////////////////////////

    std::string folderPath = name; // folderPath = ./images/folderX/imageX.jpg
    folderPath.erase(0,1);         // folderPath =  /images/folderX/imageX.jpg
    std::string mycwd(getenv("PWD")); // get current directory

    std::string detectedObjImagePath = mycwd + folderPath;
    
    utils::ArgumentValidater* arguments = utils::ArgumentValidater::getInstance();

    if (arguments->getHost() != ".") {
        insertImageInServer(detectedObjImagePath);
    }

/*
    std::string mycwd(getenv("PWD"));                     // get current directory
    std::vector<std::string> imgNamesVec;                 // save all images name
    std::string savedImgPath = folderName;                // .images/folderX
    savedImgPath.erase(0,1);                              // images/folderX
    std::string savedImgFullPath = mycwd + savedImgPath;  // current_dir/images/folderX

    getImgNamesFromFolder(savedImgFullPath, imgNamesVec); // save one folder images name in vector

    const utils::ArgumentValidater* arguments = utils::ArgumentValidater::getInstance();
    if (arguments->getHost() != ".") {  
        insertObjectInServer(savedImgFullPath, imgNamesVec);
    }
*/

}
void VA::VideoProcessor::
insertImageInServer(std::string & detectedObjectImagePath) const {
    utils::ArgumentValidater* arguments = utils::ArgumentValidater::getInstance();
    HTTP * http = new HTTP(arguments -> getHost());
    std::string detectedDate = getCurrentDate();
    std::string cameraName = "camera_1";
    std::string serverFolderName = http -> insertObject("/object", "type=people&cameraName=" + cameraName + "&firstDetectedDate=" + detectedDate);

    std::cout << "Server folder name: " << serverFolderName << std::endl;

    std::string* imagesName = new std::string[1];
    imagesName[0] = detectedObjectImagePath;
    if (http -> insertImages("/objectImages", imagesName, 1, "?folderName=" + serverFolderName)) {
        std::cout << "\nImages inserted success.\n";
    } else {
        std::cout << "\nImages inserted failure.\n";
    }   
    delete http;
}
/*
std::vector<std::string> VA::VideoProcessor::
getImgNamesFromFolder(std::string& savedImgFullPath, std::vector<std::string>& imgNamesVector)const {
    DIR* dirp = opendir(savedImgFullPath.c_str());
    struct dirent * dp;
    while (NULL != (dp = readdir(dirp))) {
        //if ("." != dp -> d_name || ".." != dp -> d_name) {
        imgNamesVector.push_back(dp -> d_name);
        //}
    }
    closedir(dirp);

    // for (int i = 0; i < imgNamesVector.size(); ++i) {
    //     imagesName[i] = folderPath + "/" + imgNamesVector[i];
    //     if ("." == imagesName[i] || ".." == imagesName[i]) {
    //         imgNamesVector.erase(imgNamesVector.begin()); // delete . / ..
    //     }
    // }

    imgNamesVector.erase(imgNamesVector.begin()); // delete .
    imgNamesVector.erase(imgNamesVector.begin()); // delete ..
    return imgNamesVector;
}

void VA::VideoProcessor::
insertObjectInServer(std::string folderPath, std::vector<std::string> & imgNamesVector) const{
    // HTTP* http = new HTTP("");
    // HTTP* http = new HTTP(arguments->getHost());
    std::string detectedDate = getCurrentDate();
    std::string cameraName = "Camera-2";
    std::string serverFolderName = this -> http -> insertObject("/object", "type=people&cameraName=" + cameraName + "&firstDetectedDate=" + detectedDate);

    std::string* imagesName = new std::string[10];
    int i = 0;
    for (i = 0; i < imgNamesVector.size(); ++i) {
        imagesName[i] = folderPath + "/" + imgNamesVector[i];
        if (10 == i) {
            break;
        }
    }
    if (http -> insertImages("/objectImages", imagesName, i, "?folderName=" + serverFolderName)) {
        std::cout << "\nImages inserted success.\n";
    } else {
        std::cout << "\nImages inserted failure.\n";
    }
    // delete http;
}
*/
std::string VA::VideoProcessor::
getCurrentDate() const {
    std::time_t result = std::time(nullptr);
    std::asctime(std::localtime(&result));
    std::string date = std::to_string(result);
    return date;
}

void VA::VideoProcessor::
CallBackFunc(int event, int x, int y, int flags, void* userdata) {
    cv::Point* points = (cv::Point*) userdata;
    if  ( event == cv::EVENT_LBUTTONDOWN ) {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")"     << std::endl << "\n";
        for (int i = 0; i < 4; ++i) {
            if (points[i] == cv::Point(0,0)) {
                points[i] = cv::Point(x,y);
                return;
            }
        }            
        points[0] = cv::Point(0,0);
        points[1] = cv::Point(0,0);
        points[2] = cv::Point(0,0);
        points[3] = cv::Point(0,0);
    }
}
