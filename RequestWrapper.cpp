#include <iostream>
#include <vector>
#include <set>
#include <curl/curl.h>
#include <stdexcept>

// clang++ --std=c++17 -o main test.cpp -stdlib=libc++ -lcurl

size_t writeCallback(char *ptr, size_t size, size_t nmemb, std::string *data) { 
    // Callback function to handle the response data
    // std::string::append ptr data buffer into 
    // our response_data string for storage
    data->append(ptr, size * nmemb);                
                                                    
    // returning the bytes we've processed (needed )
    return size * nmemb;                                                                               
}


class RequestsWrapper{
    private:

    public:
        RequestsWrapper(){

        }
        int get( std::string url,
                std::string& response_text,
                std::vector< std::string > headers = { "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:133.0) Gecko/20100101 Firefox/133.0" },
                int timeout_MS = 10'000L,
                bool follow_redirects = true
                 ){
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
            if( url.find("http://") != std::string::npos ){
                curl_easy_setopt( curlObj, CURLOPT_DEFAULT_PROTOCOL, "http" );
            }
            else{
                // defaulting to https requests
                curl_easy_setopt( curlObj, CURLOPT_DEFAULT_PROTOCOL, "https" );
            }

            // setup timeout (10 seconds, literal casting to a Long integer using suffixed "L")
            curl_easy_setopt( curlObj, CURLOPT_TIMEOUT_MS, timeout_MS );

            // setup to allow following redirects
            if( follow_redirects ){
                curl_easy_setopt( curlObj, CURLOPT_FOLLOWLOCATION, 1L );
            }
            else{
                curl_easy_setopt( curlObj, CURLOPT_FOLLOWLOCATION, 0L );
            }
            
            // set up the headers
            struct curl_slist* curl_headers = NULL;

            // this is a linkedlist of headers which is why "slist" is used w/ a pointer
            for( std::string& header : headers ){
                curl_headers = curl_slist_append( curl_headers, header.c_str() );
            }

            curl_easy_setopt( curlObj, CURLOPT_HTTPHEADER, curl_headers );

            // std::cout << "[ Performing the GET request to resource \"" << url << "\" ]\n";
            

            // tell curl which function to use to write callback data (point of this is buffering, more efficient)
            curl_easy_setopt( curlObj, CURLOPT_WRITEFUNCTION, writeCallback );
            // tell curl what variable will store the output
            curl_easy_setopt( curlObj, CURLOPT_WRITEDATA, &response_text );

            responseCode = curl_easy_perform( curlObj );

            // something went wrong when sending the GET request:
            std::cout << "[ ResponseCode = " << responseCode << " ]\n";
            if( responseCode != CURLE_OK ){
                // just simply prints the associated key-value error from the responseCode integer:
                // makes it similar to python's .GET function, easier to program with this behavior
                std::string error = "[ Something went wrong when sending the GET request, error ] --> " + std::string(curl_easy_strerror(responseCode));
                throw std::runtime_error(error);
                // return 1;
            }
            
            // std::cout << "[ Output of web request ]: " << output << "\n";

            // clean up our handler
            curl_easy_cleanup(curlObj);

            return 0;
        }

};


int main(){
    std::string url = "http://htetpbin.org/ip";
    std::vector< std::string > headers = {
        "User-Agent: Chrome",
        "Referer: https://www.google.com",
        "Origin: https://www.google.com"
    };
    std::string output = "";
    RequestsWrapper requestsObj;
    
    try{
        requestsObj.get( url, output, headers, 10'000, false );
        std::cout << "[ Output of the web request ]: " << output << '\n';
    }
    catch(const std::runtime_error& error){
        // catch our custom runtime error
        std::cout << "[ The following standard exception was thrown ]: \n";
        std::cerr << error.what() << '\n';
    }
    catch(...){
        // ^^^^ unknown error, something was thrown, and we weren't able to access the values
        // could just be a const std::exception& error (standard library error)
        std::cerr << "[ Some unknown error was thrown - not able to access the object/value of it. ]\n";

    }
    

    return 0;

}
