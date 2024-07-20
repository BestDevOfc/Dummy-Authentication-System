// Header guards:
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <string_view>
#include <iostream>


#ifdef _WIN32
#include <windows.h>

#else
#include <unistd.h>

#endif

void clear();

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

namespace design{
    const std::string line          =   col::red+"\n__________________________________________\n";
    const std::string banner        =   col::magenta + ""
                                        " █████╗ ██╗     ██╗         ███████╗███████╗ ██████╗ \n"
                                        "██╔══██╗██║     ██║         ██╔════╝██╔════╝██╔════╝ \n"
                                        "███████║██║     ██║         ███████╗█████╗  ██║      \n"
                                        "██╔══██║██║     ██║         ╚════██║██╔══╝  ██║      \n"
                                        "██║  ██║███████╗██║  ██╗   ███████║███████╗╚██████╗ \n"
                                        "╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝  ╚══════╝╚══════╝ ╚═════╝ \n";
                                              

}


/*const char* ws = " \t\n\r\f\v";*/                                         // cannot pass in vars for func declarations.

                                                                            // trim from end (right)
std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");

                                                                            // trim from beginning (left)
std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");

                                                                            // trim from both ends (left & right)
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");


typedef struct ParseKeys{
    std::string left_key;
    std::string right_key;
} ParseKeys;
std::string parse_data(ParseKeys parse_keys, std::string data);

size_t writeCallback(char *ptr, size_t size, size_t nmemb, std::string *data);
int make_get_request(const char* URL, std::string& response_data);

void print_err(const char* error);

#endif