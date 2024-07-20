#include <iostream>
#include <curl/curl.h> 

                                                                                                       // Callback function to handle the response data
size_t writeCallback(char *ptr, size_t size, size_t nmemb, std::string *data) { 
    data->append(ptr, size * nmemb);                                                                   // std::string::append ptr data buffer into 
                                                                                                       // our response_data string for storage
    return size * nmemb;                                                                               // returning the bytes we've processed (needed )
}                                                                                                      // for our curl handle when writing buffer data

int make_get_request(const char* URL, std::string& response_data){
    CURL* curl = curl_easy_init();
    CURLcode res;                                                                                       // CURL's response code, if it's a 0 it means everything went okay, if it's any other integer then an error occured.

    if( !curl ){
        std::cerr << "[ Curl Failed to Initialize ! ]\n";
        return 1;
    }

    const char* USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36";

    curl_easy_setopt(curl, CURLOPT_URL, URL);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);                                             // Will now follow all redirects
    curl_easy_setopt(curl, CURLOPT_ACCEPTTIMEOUT_MS, 5000L);                                        // 5 second timeout (curl works using long integer, literal casting 5000 into a long int using the suffix 'L'), thus, we don't have to declare a variable now.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);                                   // won't print to STDout as we've used custom callback buffering
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);                                      // writing the data from our custom buffer into our string

    res = curl_easy_perform(curl);
    if( res != CURLE_OK ){                                                                          // CURLE_OK = 0
        std::cerr << "[ curl_easy_perform failed --> " << curl_easy_strerror(res) << " ]\n";        // prints the error message depending on the integer status code returned
        return 1;
    }


    
    curl_easy_cleanup(curl);                                                                        // cleaning this handle up

    return 0;
}


typedef struct ParseKeys{
    std::string left_key;
    std::string right_key;
} ParseKeys;


std::string parse_data(ParseKeys parse_keys, std::string data){
                                                                                                    // parse data similar to python's .split() functionality
    int left_delim_index = data.find(parse_keys.left_key);
    std::string parsed_data = data.substr( left_delim_index + parse_keys.left_key.length() );                    // go up to this index and get everything to the right and ignore everything up until that index
    
    
    int right_delim_index = parsed_data.find(parse_keys.right_key);
    parsed_data = parsed_data.substr( 0, right_delim_index );                                       // from 0 to index get substring

    return parsed_data;
}



int main() {
    std::string response_data;
    const char* URL = "https://httpbin.org/ip";
    int status = make_get_request(URL, response_data);
    
    if( status != 0 ){
        return status;
    }

    ParseKeys ip_parse_keys = {
        .left_key = "\": \"",
        .right_key = "\""
    };
    
    std::string PUB_IP_ADDR = parse_data(ip_parse_keys, response_data);
    std::cout << "[ Your Public IP Address is ]: \n";
    std::cout << PUB_IP_ADDR << '\n';

    return 0;
}
