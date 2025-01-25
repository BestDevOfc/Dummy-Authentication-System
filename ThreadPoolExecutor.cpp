#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <ctime>
#include <curl/curl.h>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <string.h>

// clang++ --std=c++20 -o main test.cpp -stdlib=libc++ -I/opt/homebrew/include -L/opt/homebrew/lib -ljsoncpp -lpthread -lcurl


int trim( std::string& input ){
    // remember, std::string& is a direct reference to an existing variable,
    // std::string* is a pointer that stores an address to a variable,
    // you can do std::string& lol = "1"; and then lol += "2"; and it would work as it's a direct
    // manipulation via a reference.

    std::vector< std::string > CRLFs = {"\r", "\n", "\t"};
    int position = 0;
    
    for( std::string& crlf : CRLFs ){
        // removes all CRLFs
        while( ( position = input.find( crlf ) ) != std::string::npos ){ 
            input.erase( position, crlf.length() );

        }
    }

    // removes all starting whitespaces
    for( int i = 0; i < input.length();){
        char character = input[i];
        /*
        remove this since this is starting whitespaces which we don't want
        had a bug where I was doing i++, when we do .erase() everything is shifted to the left,
        it's dynamically done in C++ whereas in C it doesn't change when iterating.
        */
        if( character == ' ' ){
            input.erase( i, 1 );
        }
        else{
            // now that we've found a non-whitespace character we can stop removing the beginning whitespaces
            break;
        }
    }

    // removes all trailing whitespaces
    for( int i = input.length()-1; i > 0; i-- ){
        char character = input[i];
        if( character == ' ' ){
            input.erase( i, 1 );
        }
        else{
            // no more trailing whitespaces so we can stop
            break;
        }
    }
    return 0;
}


class RequestsWrapper{
    private:
        /*
        
            The reason we are using static for the writeCallback function in the Curl handler is due to how member functions in
            C++ work. All non-static member functions implicitly receive a this pointer as their first argument, which refers to the
            instance of the class that invoked the function. This means their function signature includes the this pointer, even
            though it’s not explicitly visible in the code.

            This is why we don't need to write self.member or this->member in C++ unless there's ambiguity — the compiler implicitly
            associates the member variables and functions with the current instance through the this pointer.

            However, the Curl library is written in C and expects callback functions to have a specific
            signature that does not include this or any concept of class instances. Curl only knows how
            to handle plain old data types (PODs) like pointers and fundamental types. 
            It does not understand the complexities of C++ objects or the this pointer.

            To bridge this gap, we make the callback function static. Static member functions belong to the class itself,
            not any specific instance, and therefore do not have a this pointer in their signature.
            This allows the function signature to match what Curl expects. If we need to access instance-specific data
            inside the static function, we can pass the instance explicitly via Curl's userdata mechanism.

            Code Example:

                class MyClass {
                public:
                    void member_function() {
                        // `this` is implicitly passed as an argument
                        std::cout << "Address of this class in memory is: " << this << '\n';
                    }

                    static void not_a_member_function() {
                        // No `this` pointer here; this function belongs to the class, not any instance
                        std::cout << "This does not exist implicitly for this function!\n";
                    }
                };        

            
            *** I just realized, this is the same as doing 
            class MyClass(object):
                @staticmethod
                def some_func():
                    ...

        */
        static size_t writeCallback(char *ptr, size_t size, size_t nmemb, std::string *data) { 
            // Callback function to handle the response data
            // std::string::append ptr data buffer into 
            // our response_data string for storage
            data->append(ptr, size * nmemb);                
                                                            
            // returning the bytes we've processed (needed )
            return size * nmemb;                                                                               
        }


    public:        
        RequestsWrapper(){
            
        }
        int post(
            std::string url,
            std::string& response_text,
            std::string body, // not sure how the body should be passed into the curl object yet, just putting this here as a placeholder
            std::vector< std::string > headers = { "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:133.0) Gecko/20100101 Firefox/133.0" },
            int timeout_MS = 10'000L,
            bool follow_redirects = true
        ){

            CURL* curlObj;
            CURLcode responseCode;

            curlObj = curl_easy_init();
            if( !curlObj ){
                throw std::runtime_error("[ Failed to initialize the Curl handler !  ]\n");
            }

            // set curl optoins, a lot of these are explained more in-depth in the get() version of this function
            curl_easy_setopt( curlObj, CURLOPT_URL, url.c_str() );
            curl_easy_setopt( curlObj, CURLOPT_CUSTOMREQUEST, "POST" );
            
            // set the protocol
            if( url.find("http://") != std::string::npos ){
                curl_easy_setopt( curlObj, CURLOPT_DEFAULT_PROTOCOL, "http" );
            }
            else{
                curl_easy_setopt( curlObj, CURLOPT_DEFAULT_PROTOCOL, "https" );
            }

            // set timeout
            curl_easy_setopt( curlObj, CURLOPT_TIMEOUT_MS, timeout_MS );

            // setup to allow following redirects (in case it's in front of a CDN/proxy or something)
            // when refactoring make this more compact
            if( follow_redirects == true ){
                curl_easy_setopt( curlObj, CURLOPT_FOLLOWLOCATION, 1L );
            }
            else{
                curl_easy_setopt( curlObj, CURLOPT_FOLLOWLOCATION, 0L );
            }

            // setting up the headers
            // linked list
            struct curl_slist* curl_headers = NULL;
            for( std::string header : headers ){
                curl_slist_append( curl_headers, header.c_str() );
            }
            curl_easy_setopt( curlObj, CURLOPT_HTTPHEADER, curl_headers );

            // setting up our data payload for the POST request:
            curl_easy_setopt( curlObj, CURLOPT_POSTFIELDS, body.c_str() );

            // setup the writecallback function to save buffered results
            curl_easy_setopt( curlObj, CURLOPT_WRITEFUNCTION, writeCallback );
            // tell curl what variable we want the data to go to 
            curl_easy_setopt( curlObj, CURLOPT_WRITEDATA, &response_text );

            // perform the request
            responseCode = curl_easy_perform( curlObj );

            // something went wrong, throw error & associated error string that matches that code that was returned
            if( responseCode != CURLE_OK ){
                std::string error = "[ Something went wrong when sending the GET request, error ] --> " + std::string(curl_easy_strerror(responseCode));
                throw std::runtime_error(error);
            }

            // clean up the handlers
            curl_easy_cleanup( curlObj );

            return 0;

        }
        int get( std::string url,
                std::string& response_text,
                std::vector< std::string > headers = { "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:133.0) Gecko/20100101 Firefox/133.0" },
                int timeout_MS = 10'000L,
                bool follow_redirects = true
                 ){
            CURL* curlObj;
            // stores the status code from the curl handler when we make a request, this helps us see what kind of error happened
            CURLcode responseCode;

            // initializing the curl handler
            curlObj = curl_easy_init();
            if( !curlObj ){
                throw std::runtime_error("[ Failed to initialize the Curl handler !  ]\n");
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

            // setup the timeout (10 seconds, literal casting to a Long integer using suffixed "L")
            curl_easy_setopt( curlObj, CURLOPT_TIMEOUT_MS, timeout_MS );

            // setup to allow following redirects (in case it's in front of a CDN/proxy or something)
            if( follow_redirects ){
                curl_easy_setopt( curlObj, CURLOPT_FOLLOWLOCATION, 1L );
            }
            else{
                curl_easy_setopt( curlObj, CURLOPT_FOLLOWLOCATION, 0L );
            }
            
            // set up the headers
            struct curl_slist* curl_headers = NULL;

            // this is a linkedlist of headers which is why "slist" is used w/ a pointer
            for( std::string header : headers ){
                curl_headers = curl_slist_append( curl_headers, header.c_str() );
            }

            curl_easy_setopt( curlObj, CURLOPT_HTTPHEADER, curl_headers );

            // tell curl which function to use to write callback data (point of this is buffering, more efficient)
            curl_easy_setopt( curlObj, CURLOPT_WRITEFUNCTION, writeCallback );
            // tell curl what variable will store the output
            curl_easy_setopt( curlObj, CURLOPT_WRITEDATA, &response_text );

            responseCode = curl_easy_perform( curlObj );

            // something went wrong when sending the GET request:
            if( responseCode != CURLE_OK ){
                // just simply prints the associated key-value error from the responseCode integer:
                // makes it similar to python's .GET function, easier to program with this behavior
                std::string error = "[ Something went wrong when sending the GET request, error ] --> " + std::string(curl_easy_strerror(responseCode));
                throw std::runtime_error(error);
            }
            
            // clean up our handler
            curl_easy_cleanup(curlObj);

            return 0;
        }

};




/*****************************************************************************************************/

class MakeRequests{
    private:

    public:
        std::mutex gLock;
        MakeRequests(){
            
        }
        int make_req( std::string url ){
            RequestsWrapper requestsObj;
            std::string response_text;
            try{
                requestsObj.get(
                    url, response_text
                );
                
                // will handle .lock() and .unlock()s for us in case of exceptions, when function terminates so does the 
                // lock
                std::lock_guard guardLock( gLock );
                std::cout << response_text << '\n';
                
            }
            catch( std::runtime_error& error ){
                std::cerr << "[ The following exception occured ]" << error.what() << '\n';
                return 2;
            }
            catch( ... ){
                std::cerr << "[ An unknown exception occured ! ]\n";
                return 2;
            }

            return 0;
        }
        int main(){
            
            
            // load URLs
            std::ifstream urls_file( "urls_big.txt", std::ios::in );
            std::string file_line = "";
            std::vector< std::string > urls;
            while( std::getline( urls_file, file_line ) ){
                // remove the CRLF
                trim( file_line );
                urls.push_back( file_line );
            }

            // set limits for our ThreadPoolExecutor implementation
            int max_workers = 100;
            int num_tasks = urls.size();
            int tasks_done = 0;
            
            while( tasks_done != num_tasks && tasks_done != num_tasks ){
                // making sure we're not endlessly adding threads when we've already passed in all of the URLs
                if( max_workers > ( num_tasks - tasks_done ) ){
                    max_workers = num_tasks - tasks_done;
                }
                std::vector< std::thread > task_threads;
                
                // run the chunk of threads (limited by max_workers)
                for( int i = tasks_done; i < tasks_done+max_workers; i++ ){
                    std::thread task_thread = std::thread( &MakeRequests::make_req, this, urls[i] );
                    task_threads.push_back( std::move( task_thread ) );
                }

                /* This isn't the more efficient way because we're waiting for all 100 to finish */
                // now that we've reached the max possible workers we'll join all of them
                for( auto& thread : task_threads ){
                    thread.join();
                    tasks_done += 1;
                }
                task_threads.clear();
            }
     
            std::cout << "[ Finished Running all Threads ! ]\n";

            return 0;

        }
};


int main(){
    MakeRequests Obj;
    Obj.main();

    return 0;
}
