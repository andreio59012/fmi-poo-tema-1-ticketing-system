#pragma once
#include <iostream>
#include <cstring>
#include "Constants.h"

class User
{
private:
    char username[FIXED_STRING_SIZE];
    char password[FIXED_STRING_SIZE];

public:
    explicit User(
        const char* username_ = "",
        const char* password_ = ""
    ) {
        strncpy(username, username_, FIXED_STRING_SIZE - 1);
        username[FIXED_STRING_SIZE - 1] = '\0';
        strncpy(password, password_, FIXED_STRING_SIZE - 1);
        password[FIXED_STRING_SIZE - 1] = '\0';

        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Constructor with parameters for username '" << username << "'.\n";
    }

    User(User&& other) noexcept {
        strncpy(username, other.username, FIXED_STRING_SIZE);
        strncpy(password, other.password, FIXED_STRING_SIZE);

        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Move Constructor for username '" << username << "'.\n";
    }

    User(const User& other) {
        strncpy(username, other.username, FIXED_STRING_SIZE);
        strncpy(password, other.password, FIXED_STRING_SIZE);

        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Copy Constructor for username '" << username << "'.\n";
    }

    User& operator=(const User& other) {
        strncpy(username, other.username, FIXED_STRING_SIZE);
        strncpy(password, other.password, FIXED_STRING_SIZE);

        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Copy Operator for username '" << username << "'.\n";
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const User& p) {
        os << "User:\n\tUsername: " << p.username << "\n\tPassword: " << p.password << "\n";
        return os;
    }

    ~User() {
        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Destructor for username '" << username << "'.\n";
    }

    const char* getUsername() const { return username; }

    bool isPasswordCorrect(const char* password_) const {
        return strncmp(password, password_, FIXED_STRING_SIZE) == 0;
    }

    void setUsername(const User* auth, const char* username_) {
        if (auth == this) {
            strncpy(username, username_, FIXED_STRING_SIZE - 1);
            username[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    void setPassword(const User* auth, const char* password_) {
        if (auth == this) {
            strncpy(password, password_, FIXED_STRING_SIZE - 1);
            password[FIXED_STRING_SIZE - 1] = '\0';
        }
    }
};