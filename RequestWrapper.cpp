#include <iostream>
#include <vector>
#include <set>
#include <curl/curl.h>

// clang++ --std=c++17 -o main test.cpp -stdlib=libc++ -lcurl

size_t writeCallback(char *ptr, size_t size, size_t nmemb, std::string *data) { 
    // Callback function to handle the response data
    // std::string::append ptr data buffer into 
    // our response_data string for storage
    data->append(ptr, size * nmemb);                
                                                    
    // returning the bytes we've processed (needed )
    return size * nmemb;                                                                               
}

class RequestMethod{
    private:

    public:
        int GET = 1;
        int POST = 2;
        int PUT = 3;
        int DELETE = 4;
        int TRACE = 5;
        int HEAD = 6;
        int OPTIONS = 7;
        int PATCH = 8;
        RequestMethod(){

        }
};

class RequestsWrapper{
    private:

    public:
        std::string url = "";
        int method = 0;
        std::vector< std::string > headers = {};
        // need to create it as a reference
        std::string& output;

        // ^^ currently we're not actually processing this - do it!
        RequestsWrapper( std::string a_url,
                        int a_method,
                        std::vector< std::string > a_headers,
                        std::string& a_output ): 
                        
                        url(a_url),
                        method(a_method),
                        headers(a_headers),
                        output(a_output){
            // need to use an initializer list instead, otherwise a copy will be made of the reference output
            // and it won't persist outside of the lifetime of our class
            /*
                url = a_url;
                method = a_method;
                headers = a_headers;
                output = a_output;
            */
            
        }

        int execute(){
            // will execute the web request requested by the caller:
            switch( method ){
                case 1:
                    make_get_request();
                    break;
                default:
                    // could just use a validate function in private, but this is just more simple
                    std::cerr << "[ Request Method \'" << method << "\' << not supported ]\n";
                    break;
            }

            return 0;
        }
        int make_get_request(){
            CURL* curlObj;
            CURLcode responseCode;

            std::cout << "[ Initializing the Curl Handler... ]\n";
            curlObj = curl_easy_init();
            if( !curlObj ){
                std::cerr << "[ Failed to initialize the Curl handler !  ]\n";
                return 1;
            }

            // setup the protocol & url
            // curl handlers only work with plain old datatypes (PODs), and URL is of type std::string, so we'll just convert
            // it into a C-string using .c_str();
            curl_easy_setopt( curlObj, CURLOPT_URL, url.c_str() );
            curl_easy_setopt( curlObj, CURLOPT_CUSTOMREQUEST, "GET" );
            
            // this works because of the URL normalizing we did earlier
            if( url.find("https://") != std::string::npos ){
                curl_easy_setopt( curlObj, CURLOPT_DEFAULT_PROTOCOL, "https" );
            }
            else{
                curl_easy_setopt( curlObj, CURLOPT_DEFAULT_PROTOCOL, "http" );
            }

            // setup timeout (10 seconds, literal casting to a Long integer using suffixed "L")
            curl_easy_setopt( curlObj, CURLOPT_TIMEOUT_MS, 10'000L );

            // setup to allow following redirects
            curl_easy_setopt( curlObj, CURLOPT_FOLLOWLOCATION, 1L );

            // set up the headers
            struct curl_slist* headers = NULL;

            // this is a linkedlist of headers which is why "slist" is used w/ a pointer
            
            // now we need to process the header values which were passed into the class's constructor!
            
            headers = curl_slist_append( headers, "User-Agent: Chrome" );
            headers = curl_slist_append( headers, "Referer: https://www.google.com" );
            headers = curl_slist_append( headers, "Origin: google.com" );

            curl_easy_setopt( curlObj, CURLOPT_HTTPHEADER, headers );

            std::cout << "[ Performing the GET request to resource \"" << url << "\" ]\n";
            

            // tell curl which function to use to write callback data (point of this is buffering, more efficient)
            curl_easy_setopt( curlObj, CURLOPT_WRITEFUNCTION, writeCallback );
            // tell curl what variable will store the output
            curl_easy_setopt( curlObj, CURLOPT_WRITEDATA, &output );

            responseCode = curl_easy_perform( curlObj );

            // something went wrong when sending the GET request:
            std::cout << "[ ResponseCode = " << responseCode << " ]\n";
            if( responseCode != CURLE_OK ){
                // just simply prints the associated key-value error from the responseCode integer:
                std::cerr << "[ Something went wrong when sending the GET request, error ] --> " << curl_easy_strerror( responseCode ) << '\n';
                return 1;
            }
            
            // std::cout << "[ Output of web request ]: " << output << "\n";

            // clean up our handler
            curl_easy_cleanup(curlObj);

            return 0;
        }

};


int main(){
    std::string url = "http://httpbin.org/ip";
    std::vector< std::string > headers = {
        "User-Agent: Chrome",
        "Referer: https://www.google.com",
        "Origin: https://www.google.com"
    };
    std::string output = "";


    RequestsWrapper requestObj(
        url,
        RequestMethod().GET, // keeps developers (us) from making mistakes by doing "get" instead of "GET"
        headers,
        output
    );
    requestObj.execute();

    std::cout << "[ Output of the GET request ]: " << output << '\n';

    return 0;

}
