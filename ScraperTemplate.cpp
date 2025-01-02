#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ctime>
#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <set>


/*
    clang++ --std=c++17 -o main main.cpp -stdlib=libc++ -lpthread

    [+] - Program: XMLRPC.PHP Admin Panel BruteForcer.
    [+] - Author: Muhammad Ali
    [+] - Articles: https://medium.com/@alimuhammadsecured
    [+] - Future Improvements: Using multicalls to spray multiple passwords via one request.


    1) Load + Normalize URLs
    2) Check which have XMLrpc.php enabled
    3) Scrape usernames from XMLrpc.php enabled URLs
        - if not allowed / enabled then default to admin

    4) Process and save everything into a file 
    5) start threadpool3 to now bruteforce the usernames to passwords combinations

    [*] - Watch-outs when developing:
        1) http:// and https:// discrepancies (similar to the GoPhish sprayer)
        2) Request exceptions ( use try & excepts with proper logging setup )

    [*] - Closing Notes:
        Due to portability I decided not to use JThreads (CLang & Apple Silicon Chip Portability Issues)

*/



/*****************************************************************************************************/
// Utils.cpp
namespace col{
    const std::string reset         =       "\033[0m";
    const std::string bold          =       "\033[1m";
    const std::string red           =       bold+"\033[31m";
    const std::string blue          =       bold+"\033[34m";
    const std::string yellow        =       bold+"\033[33m";
    const std::string purple        =       bold+"\033[35m";
    const std::string cyan          =       bold+"\033[36m";
    const std::string green         =       bold+"\033[32m";
    const std::string magenta       =       bold + "\033[35m";
    const std::string black         =       bold+"\033[30m";
    const std::string pink          =       bold + "\033[95m";
    const std::string white         =       bold+"\033[37m";
    const std::string underline     =       bold+"\033[4m";
}
int trim( std::string& input ){
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
/*****************************************************************************************************/





class XMLSprayer{
    // only accessible within the XMLSprayer() class.
    private: 
        std::vector< std::string > user_agents = {
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36",
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36",
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:133.0) Gecko/20100101 Firefox/133.0",
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36 Edg/131.0.0.0",
            "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36",
            "Mozilla/5.0 (X11; Linux x86_64; rv:133.0) Gecko/20100101 Firefox/133.0",
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:132.0) Gecko/20100101",
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:233.0) Gecko/20100101",
            "Mozilla/5.0 (Macintosh; Intel Windows 10.15; rv:133.0) Gecko/20100101",
            "Mozilla/5.0 (Macintosh; Intel OS X 10.15; rv:133.0) Gecko/20100101",
        };
        std::mutex gLock;


        bool validate_threads(){
            // valid threads value:
            return threads > 0;
        }
        std::string get_user_agent(){
            // Get a random index
            std::random_device rd;  // Random number generator
            std::mt19937 gen(rd()); // Mersenne Twister RNG
            // this helps with ranges & biases:
            std::uniform_int_distribution<> dis(0, user_agents.size() - 1);

            int rand_index = dis(gen);
            return user_agents[rand_index];
        }
        std::string normalize_url( std::string url ){
            // read the URLs and normalize them, if they don't have the protocol
            // default to http://
            
            return url;
        }
        int load_urls(){
            // 
            std::ifstream urls_file( urls_filepath, std::ios::in );
            std::string url_line = "";

            if( !urls_file ){
                std::cerr << col::red << "[ Failed to open the file \'" << urls_filepath << "\' ]\n";
                return 1;
            }

            // ohhh now the getline(cin, variable) actually makes sense now LOL
            while( std::getline( urls_file, url_line ) ){
                std::cout << col::yellow << "[ URL ]: " << url_line << '\n';
                /*

                    1) trim out the CRLFs (\r\n), starting/trailing whitespaces, \ts
                    2) run the normalize function
                    3) push into the vector


                */
                trim( url_line );
                if( (url_line.find("http://") == std::string::npos) && (url_line.find("https://") == std::string::npos) ){
                    // the protocol is not explicitly defined, push both http:// and https://

                    urls.push_back( "http://"+url_line );
                    urls.push_back( "https://"+url_line );


                }
                else{
                    // everything is good, has been trimmed & the protocol is explicitly specified
                    urls.push_back( url_line );
                }

                
            }

            // now that post-processing has been done, remove all duplicate entries:
            // *urls.begin() is just the first element of the vector
            // if spraying 1M URLs from services like shodan,censys,dorkParsing, and you have 100K duplicates this will save immense time
            std::set< std::string > unique_urls( urls.begin(), urls.end() );

            // converting back to aa vector to continue
            urls = {};
            // *urls.begin() is just the first element of the vector
            urls.assign( unique_urls.begin(), unique_urls.end() );
            
            urls_file.close();
            return 0;
        }

    
    // public means code within the XMLSprayer class and OUTSIDE can access these values:
    public:
        std::string url = "URL_HERE";
        std::string file_path = "";
        std::ofstream results_file;
        std::string urls_filepath = "";
        std::vector< std::string > urls = {};
        
        int threads;

        XMLSprayer( int a_threads, std::string a_file_path, std::string a_urls_filepath ){
            
            threads = a_threads;
            file_path = a_file_path;
            urls_filepath = a_urls_filepath;

            // std::cout << col::green << "[ Creating the XMLSprayer Object - this is the equiavlent of __init__() in Python! ] \n";
        }

        
        bool XMLRPC_is_enabled( std::string url ){            
            
            
            return false;
        }
        int get_usernames( std::string url ){
           return 0; 
        }  

        int login( std::string url, std::string username, std::string password ){
            return 0;
        }
        int main(){
            std::cout << col::yellow << "[ Initializing the XMLSprayer... ]\n";

            // this is what makes C++ objects so powerful, we're given a lot of granular control.
            if( validate_threads() != true ){
                std::cerr << col::red << "[ Invalid Threads Count! ]\n";
                return 1;
            }

            // open the results file:
            results_file.open(file_path, std::ios::app);
            if( !results_file ){
                std::cerr << col::red << "[ Failed to open the \'Results.txt\' file needed to save the results !  ]\n";
                return 1;
                
            }

            std::cout << col::green << "\n[ Reading the URLs & Normalizing them... ]\n";

            load_urls();
            
            

            results_file << "[ Test C++ XMLSprayer Results. ]\n";

            std::cout << col::green << "[ Starting the XMLSprayer... ]\n";


            std::cout << "[ What the URLs look like after we've trimmed them ]: \n";
            for( std::string& url : urls ){
                std::cout << "[ URL ]: \"" << url << "\"\n";
            }

            
            /*
            std::vector< std::thread > threads = {};
            for( int i = 0; i < 100; i++ ){
                threads.emplace_back( std::thread( &XMLSprayer::test, this, i ) );
            }

            // join all of the threads together:
            for( std::thread& thread : threads ){
                thread.join();
            }
            */

            results_file.close();            
            
            return 0;
        }
};



int main(){
    
    
    int threads = 0;
    std::string file_path = "";

    // setting up our results file path:
    if( !std::filesystem::exists("Results") ){
        std::filesystem::create_directory("Results");
    }
    file_path = "Results/Results-" + std::to_string( std::time(nullptr) ) + ".txt";

    
    // get user input:
    std::string urls_filepath = "";
    std::cout << col::green << "[ Enter the filepath to the target URLS ]: ";
    std::getline(std::cin, urls_filepath);

    // check if the filepath exists.
    if( !std::filesystem::is_regular_file( urls_filepath ) ){
        std::cerr << col::red << "[ Not a file or does not exist! !  ]\n";
        return 1;
    }


    std::cout << col::green << "[ Enter the Threads Count ]: ";
    std::cin >> threads;

    // run the Sprayer
    XMLSprayer Obj( threads, file_path, urls_filepath );
    std::cout << col::yellow << "\n[ The number of Threads ]: " << Obj.threads << '\n';


    Obj.main();


    return 0;
}
