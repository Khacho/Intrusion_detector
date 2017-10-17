// Class headers
#include "ArgumentValidater.hpp"
// Headers from OpenCV
#include "opencv2/opencv.hpp"

//
#include <sys/types.h>
#include <sys/stat.h>

/**********************************************************
* Public members
**********************************************************/

utils::ArgumentValidater* utils::ArgumentValidater::
getInstance(int argc, const char** argv) {
    if (!utils::ArgumentValidater::argumentV) {
        cv::CommandLineParser parser(argc, argv, utils::ArgumentValidater::getKeys());
        utils::ArgumentValidater::argumentV = new utils::ArgumentValidater(parser);
    }
    return utils::ArgumentValidater::argumentV;
}


utils::ArgumentValidater::
~ArgumentValidater() {
    /*empty*/
}

bool utils::ArgumentValidater::
validate() {
    if (this->parser.has("help")) {
        this->parser.printMessage();
        return false;
    }
    if (!this->parser.check()) {
        this->parser.printErrors();
        return false;
    }
    return true;
}

bool utils::ArgumentValidater::
isFileExists(cv::String path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    } else if (info.st_mode & S_IFMT) {
        return true;
    }
    return false;
}

cv::String utils::ArgumentValidater::
getVideoPath() const {
    return this->parser.get<cv::String>("video");
}

cv::String utils::ArgumentValidater::
getHost() const {
    return this->parser.get<cv::String>("host");
}

cv::String utils::ArgumentValidater::
getKeepImagesPath() const {
    return this->parser.get<cv::String>("path");
}

bool utils::ArgumentValidater::
isShowEnable() const {
    if (this->parser.get<cv::String>("show") == "true") {
        return true;
    }
    return false; 
}

/**********************************************************
* Private members
**********************************************************/

utils::ArgumentValidater::
ArgumentValidater (cv::CommandLineParser prs):parser(prs) { }

cv::String utils::ArgumentValidater::
getKeys() {
    return  "{? help       |           | help message.}"
            "{v video      |   <none>  | path to video file.}"
            "{h host       |   .       | the server host where results should by located.Don`t send the results if flag is not specified}"
            "{p path       |   .       | the folder path where generated output should by kept.}"
            "{s show       |   true    | enables the UI windows showing in case of true and hidden then othervise.}";
}
