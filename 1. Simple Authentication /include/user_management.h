#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H

#include <string>

std::string parse_username(std::string_view row);
std::string parse_password(std::string_view username);

int user_exists(std::string_view username);
int create_user();
int login_user();

std::string hash_password(std::string password);
int print_MOTD(const char * URL);

#endif