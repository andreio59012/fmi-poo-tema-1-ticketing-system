#pragma once
#include <map>
#include <string>
#include <iostream>
#include "Constants.h"

class User
{
private:
	std::string username, password;

public:
	// Constructor
	explicit User(
		const std::string& username_ = "",
		const std::string& password_ = ""
	) :
		username(username_),
		password(password_)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'User': Constructor with parameters for username '" << username << "'.\n";
	}

	// Move Constructor
	User(User&& other) noexcept :
		username(std::move(other.username)),
		password(std::move(other.password))
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'User': Move Constructor for username '" << username << "'.\n";
	}

	// Copy Constructor
	User(const User& other) :
		username(other.username),
		password(other.password)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'User': Copy Constructor for username '" << username << "'.\n";
	}

	// Copy Operator
	User& operator=(const User& other) {
		username = other.username;
		password = other.password;

		if (LOG_CONSTRUCTORS)
			std::cout << "'User': Copy Operator for username '" << username << "'.\n";
		return *this;
	}

	// OStream Operator (#TODO)
	friend std::ostream& operator<<(std::ostream& os, const User& p) {
		os << "User:\n\tUsername: " << p.username << "\n\tPassword: " << p.password << "\n";
		return os;
	}

	// Destructor
	~User() {
		if (LOG_CONSTRUCTORS)
			std::cout << "'User': Destructor for username '" << username << "'.\n";
	}

	// Getters & Setters
	const std::string getUsername () const { return username; }

	bool isPasswordCorrect(const std::string& password_) const {
		return password == password_;
	}

	void setUsername(const User* auth, const std::string& username_) {
		if(auth == this)
			username = username_;
	}

	void setPassword(const User* auth, const std::string& password_) {
		if (auth == this)
			password = password_;
	}
};