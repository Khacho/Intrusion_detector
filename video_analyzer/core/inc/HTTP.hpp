#include <curl/curl.h> 
#include <string>

class HTTP {

    private:
        CURL* _request;// request handle
        std::string _defaultURL;// URL requst
       // HTTP* _http;

        bool isSuccessCode(CURLcode res, int successCode);// is response success
        static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);// write response callback 
    
    public:
        HTTP (std::string url);// constructor
        ~HTTP();// destructor
       // static HTTP* getHTTP(std::string url = " ");// create singlotaon object 
        std::string insertObject(std::string rout, std::string data);// insert object to host
        bool insertImages(std::string rout, std::string* imagesPath, int count, std::string queryParams);// insert object images to host        
};
