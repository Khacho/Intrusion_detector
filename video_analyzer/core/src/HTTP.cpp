#include "HTTP.hpp"
#include <iostream>
HTTP::HTTP(std::string url) {
    if (!url.empty()) {
        _defaultURL = url;
    } else {
        _defaultURL = "http://localhost:4300/api/v1";
    }
    curl_global_init(CURL_GLOBAL_ALL);
}

HTTP::~HTTP() {
    curl_global_cleanup();
}

std::string HTTP::insertObject(std::string rout, std::string data) {
    _request = curl_easy_init();

    std::string readBuffer;
    curl_easy_setopt(_request, CURLOPT_URL, (_defaultURL + rout).c_str());
    curl_easy_setopt(_request, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(_request, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(_request, CURLOPT_WRITEDATA, &readBuffer);

    std::string folderName = "";
    if (isSuccessCode(curl_easy_perform(_request), 201)) {
        std::string key = "\"folderName\":\"";
        std::size_t found = readBuffer.find(key);
        if (found != std::string::npos) {
            int i = found + key.size();
            while (readBuffer[i] !=  '\"') {
                folderName = folderName + readBuffer[i];
                ++i;
            }
        }
    }
    // std::cout << "Insert :" << folderName << std::endl;
    return folderName;
}

bool HTTP::insertImages(std::string rout,  std::string* imagesPath, int count, std::string queryParams) {
    curl_easy_reset(_request);
    _request = curl_easy_init();

    curl_easy_setopt(_request, CURLOPT_URL, (_defaultURL + rout + queryParams).c_str());

    struct curl_httppost *post=NULL;
    struct curl_httppost *last=NULL;

    for (int i = 0; i < count; ++i) { 
        curl_formadd(&post, &last,
            CURLFORM_COPYNAME, "images",
            CURLFORM_FILE, imagesPath[i].c_str(), 
            CURLFORM_FILENAME, imagesPath[i].c_str(),
            CURLFORM_END);
    }
    curl_easy_setopt(_request, CURLOPT_HTTPPOST, post);
    CURLcode res = curl_easy_perform(_request);
    curl_formfree(post);
    return isSuccessCode(res, 201);
} 

bool HTTP::isSuccessCode(CURLcode res, int successCode) {
    long http_code = 0;
    curl_easy_getinfo (_request, CURLINFO_RESPONSE_CODE, &http_code);
    if (res != CURLE_COULDNT_CONNECT && res == CURLE_OK) {
        if (http_code == successCode && res != CURLE_ABORTED_BY_CALLBACK){
            return true;
        }
    }
    return false;
}

size_t HTTP::writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
