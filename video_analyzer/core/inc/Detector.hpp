#ifndef DETECTOR_HPP
#define DETECTOR_HPP

// Headers from OpenCV
//#include "opencv2/core/types.hpp"
#include "opencv2/objdetect/objdetect.hpp"

// Headers from system
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

namespace VA {
    class Detector {
        private: 
            cv::CascadeClassifier cascadeClassifier;

        public: 
            /**
             * Class constructor.
             */
            Detector ();

            /**
             * Class destructor
             */
            ~Detector ();
            
            bool loadClassifier();

            /**
             * Detect human using haarCascad
             */
            void detect (cv::Mat& img, std::vector<cv::Rect>& human );


    }; // end of class Detector
} // end of namespace VA

#endif // DETECTOR_HPP
