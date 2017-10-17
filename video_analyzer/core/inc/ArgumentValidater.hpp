#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP
// Headers from OpenCV
#include "opencv2/opencv.hpp"

namespace utils {
    class ArgumentValidater {

        public: 
            /**
            * Get instance of ArgumentParcer object.
            * @param argc - count of the sent arguments to the program
            * @param argv - the arguments sent to the program
            * @return instance of ArgumentParcer object
            */
            static ArgumentValidater* getInstance(int argc = 0, const char** argv = nullptr);

            /**
            * Is exist path.
            * @param path - video file path
            * @return true if file exist and false otherwise
            */
            bool isFileExists(cv::String path); 

			/**
			 * Class destructor
			 */
            ~ArgumentValidater ();

            /**
            * Checking is valid arguemnts.
            */
            bool validate ();

            /**
            * Get video path.
            */
            cv::String getVideoPath () const;

            /**
            * Get keep images foler path.
            */
            cv::String getKeepImagesPath () const;

            /**
            * Get uploading host. 
            */
            cv::String getHost () const;

            /**
            * Get is show enable.
            */
            bool isShowEnable () const;

        private: 
            static ArgumentValidater* argumentV;
            cv::CommandLineParser parser;

            /**
            * Get command line parser keys.
            */
            static cv::String getKeys();

            /**
            * Class constructor.
            */
            ArgumentValidater (cv::CommandLineParser prs);                       
           
           
    }; // end of class ArgumentValidater
} // end of namespace utils
#endif // ARGUMENT_HPP
