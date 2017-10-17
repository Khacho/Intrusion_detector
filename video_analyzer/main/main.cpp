// Class headers
#include "VideoProcessor.hpp"
#include "ArgumentValidater.hpp"

utils::ArgumentValidater* utils::ArgumentValidater::argumentV;

int main(int argc, const char** argv) {
    utils::ArgumentValidater* arguments = utils::ArgumentValidater::getInstance(argc, argv);
    if (arguments->validate()) {
        VA::VideoProcessor videoProcessor;
        bool status = videoProcessor.run();
        return 0;
    }
    return 1;
} 
