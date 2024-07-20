#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <openssl/sha.h>

#include "user_management.h"
#include "utils.h"



std::string hash_password(std::string password){
    
    std::string SALT = "OPKIJHUEGWYTFDR^%&*(U0)";                               // NOTE: should not be hard-coding sensitive information

    password = password + SALT;
    const char* c_str_password = password.c_str();                              // to use strlen we need c-strings which are const char*
    
    unsigned int c_str_len = strlen(c_str_password);                            // openSSL works with raw bytes (unsigned char arrays)
    unsigned char text[c_str_len];
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // /*
    // for(int i=0; i<strlen(original_string.c_str()); i++){                    // converting std::string into unsigned character array (LEGACY)
    //     text[i] = (unsigned char)original_string[i];
    // }
    // */
    std::memcpy(text, c_str_password, c_str_len);                               // copying raw bytes into text for openSSL (efficient)

    SHA256(text, c_str_len, hash);

    std::stringstream hash_string_stream;                                       // allow us to format to hex like std::cout into a variable

    for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
        hash_string_stream << std::hex << (int)hash[i];                         // have to encode-cannot display or safe raw bytes
    }
    
    return hash_string_stream.str();                                            // returning the std::string type from the string stream

}

std::string parse_username(std::string_view row){
                                                                                // parse the username
    if( row.find(':') == std::string::npos ){
        return "Invalid Row.";
    }
    int delim_index = row.find(':');
    std::string username;
    
    /* 
    ! For strlen use strlen(std::string.c_str());
    Legacy - was not working when password comparison came around 
    for(int i=0; i<delim_index; i++){
        username += row[i];
    }
    */
    username = row.substr( 0, delim_index );
    return username;
}


std::string parse_password(std::string_view username){
                                                                                // parse the password

    std::ifstream read_database{"database.txt"};
    if( !read_database.is_open() ){
        print_err("File failed to open for parse_password()");
        return "ERROR";
    }
    std::string row;
                                                                                //  more robust than data >> file
    while ( std::getline(read_database, row) ){ 
        int delim_index = row.find(':');
        std::string password;
        
        /*
        Legacy - was not working when password comparison came around 
        int string_len = sizeof(row)/sizeof(row[0]);
        for(int i=0; i<string_len-delim_index-1; i++){
            password += row[delim_index+i+1];
        }
        */
        if(parse_username(row) == username){
            password = row.substr( delim_index + 1 );
            return password;
        }


    }
    return "none.";

}

int user_exists(std::string_view username){
                                                                                // read database file.
    std::ifstream read_database{"database.txt"};
    if( !read_database.is_open() ){
        print_err("File failed to open for user_exists()");
        return 1;
    }
    std::string file_data;
    while(read_database >> file_data){
        std::string database_username { parse_username(file_data) };
        if( username == database_username ){
            return 0;
        }
        
    }
    return 1;
}
int create_user(){
                                                                                // open and write to file.
    int successful = 1;
    std::ofstream database_write{"database.txt", std::ofstream::app};
    if( !database_write.is_open() ){
        print_err("File failed to open for create_user()");
        return 1;
    }
    while( successful != 0 ){
        clear();
        std::string username;
        std::string password;
        std::string confirm_password;
        
        std::cout << design::banner;
        std::cout << col::yellow << "[ Enter Username ]: ";
        std::cin >> username;

                                                                                // verify username does not exist.
        if( user_exists(username) == 0 ){
            print_err("username taken");
            continue;
        }

        std::cout << col::yellow << "[ Enter Password ]: ";
        std::cin >> password;

        std::cout << col::yellow << "[ Confirm Password ]: ";
        std::cin >> confirm_password;

        
        if( password != confirm_password ){                                     // verify passwords match
            print_err("Passwords do not match");
            continue;
        }
        
        std::string hashed_password = hash_password( password );
        database_write << username << ":" << hashed_password << '\n';
        
        if( !database_write.good() ){                                           // Verify successful write
            print_err("Failed to update database");
            return 1;
        }
        
        successful = 0;

    }

    return 0;
}

int login_user(){
    std::string username;
    std::string password;

    clear();
    std::cout << design::banner;
    std::cout << col::yellow << "[ Enter Username ]: ";
    std::cin >> username;

    std::cout << col::yellow << "[ Enter Password ]: ";
    std::cin >> password;
    /*password = trim(password);*/                                              // Legacy, pass by reference
    trim(password);
    std::string hashed_password = hash_password(password);

    if(user_exists(username) != 0){
        print_err("username does not exist");
        return 1;
    }

    std::string correct_password = parse_password(username);
    /*
    correct_password = trim(correct_password);                                  // Legacy, pass by reference
    */
    trim(correct_password);


    if( hashed_password == correct_password ){
        clear();
        std::cout << design::banner;
        std::cout << design::line
                  << col::green << "\n[ Welcome " << username << " !]"
                  << design::line;
        return 0;
    }

    print_err("Invalid credentials");
    return 1;                                                                   // incorrect password
}