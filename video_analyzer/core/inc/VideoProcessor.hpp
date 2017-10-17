#ifndef VIDEOPROCESSOR_HPP
#define VIDEOPROCESSOR_HPP

#include "opencv2/highgui.hpp"
#include "Detector.hpp"
#include "BgSubtractor.hpp"
#include "HTTP.hpp"
#include "ArgumentValidater.hpp"

namespace VA {

    class VideoProcessor {
        private: 
            cv::VideoCapture videoCapture;
            VA::Detector detector;
            VA::BgSubtractor bgSubtractor;
            int frameIndex;
            cv::Point points[1][4];
            
            // HTTP* http;
        public: 

            /**
             * Class constructor.
             */
            VideoProcessor();

            /**
             * Class destructor
             */
            ~VideoProcessor();

            /**
             * Reads video frames and do some transformations with them.
             * Detectes human in video frame.
             *
             * @param name - the video file name or steam
             * @return true if process completed with success and false otherwise
             */
            bool run();

        private:
            void drawObjects(std::vector<cv::Rect> rects, std::vector<cv::Rect> boundRect, cv::Mat& origin);  // Draw detected objects
            void fillPolyTopBottom(cv::Mat& origin); // Drow white polygon in frame top and bottom side.
            void fillPolyLeftRight(cv::Mat& origin); // Drow white polygon in frame left and right side.
            std::string generateFolderName(cv::String folderName) const; // Generates folder name
            void createNamedWindow() const; // Creates windows to show images;
            void createFolder(const std::vector<cv::Rect> rects, const cv::Mat& original, const std::string& folderName) const; // Creates rectangles for detected objects.
            void saveImages(const cv::Mat& original, const std::string& folderName) const; // Saves the images.
	        static void CallBackFunc(int event, int x, int y, int flags, void* userdata);

            void insertImageInServer(std::string & detectedObjectImagePath) const;

            // void insertObjectInServer(std::string folderPath, std::vector<std::string> & imgNamesVector) const;
            std::string getCurrentDate() const;
            // std::vector<std::string> getImgNamesFromFolder(std::string& savedImgFullPath, std::vector<std::string>& imgNamesVector)const;

    }; // end of VideoProcessor class

} // end of VA namespace

#endif // VIDEOPROCESSOR_HPP
