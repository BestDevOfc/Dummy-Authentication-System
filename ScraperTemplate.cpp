#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ctime>
#include <filesystem>


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


class Scraper{
    // only accessible within the Scraper() class.
    private:
        bool validate_threads(){
            // valid threads value:
            return threads > 0;
        }
    
    // public means code within the Scraper class and OUTSIDE can access these values:
    public:

        std::string url = "URL_HERE";
        std::string file_path = "";
        std::ofstream results_file;
        int threads;
        Scraper( int a_threads, std::string a_file_path ){
            
            threads = a_threads;
            file_path = a_file_path;
            std::cout << col::green << "[ Creating the Scraper Object - this is the equiavlent of __init__() in Python! ] \n";
        }

        int main(){
            std::cout << col::yellow << "[ Initializing the Scraper... ]\n";
            
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
            
            results_file << "[ Test C++ scraper Results. ]\n";

            std::cout << col::green << "[ Starting the scraper... ]\n";
            
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
    std::cout << col::green << "[ Enter the Threads Count ]: ";
    std::cin >> threads;

    // run the scraper
    Scraper Obj( threads, file_path );
    std::cout << col::yellow << "\n[ The number of Threads ]: " << Obj.threads << '\n';

    Obj.main();


    return 0;
}
