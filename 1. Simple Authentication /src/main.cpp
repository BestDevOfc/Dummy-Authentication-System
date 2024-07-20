#include <iostream>
#include <string>
#include <fstream>
#include "user_management.h"
#include "utils.h"



int main(){
    print_MOTD("https://raw.githubusercontent.com/BestDevOfc/MOTD/main/MOTD.txt");


    int invalid_option = 1;
    while ( invalid_option == 1 ){
        clear();
        std::cout << design::banner
                  << design::line   << '\n'
                  
                  << col::yellow    <<  "Choose an Option: \n"
                  << col::red       << "[1]"
                  << col::yellow    << " - Register \n"
                  << col::red       << "[2]"
                  << col::yellow    << " - Login \n"
                  
                  << design::line << '\n';

        int option {0};
        std::cin >> option;
        clear();

        switch(option){
            case 1:
                invalid_option = 0;
                create_user();
                break;
            case 2:
                invalid_option = 0;
                login_user();
                break;
            default:
                break;
        }
    }

    std::cout << col::reset;
    return 0;
}