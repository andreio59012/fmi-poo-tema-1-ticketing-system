#pragma once
#include <map>
#include <string>
#include <iostream>
#include "User.h"
#include "Constants.h"

const int USER_PERM_NONE = 0;		// No perms.
const int USER_PERM_VIEWER = 1;		// View perms.
const int USER_PERM_MAINTAINER = 2;	// Create Ticket/PR perms.
const int USER_PERM_REVIEWER = 3;	// Review/Close Ticket/PR perms.
const int USER_PERM_OWNER = 4;		// Change project settings.

class Project 
{
private:
	int default_user_perm;
	std::string title, description;
	std::map<User*, int> user_perms;
	std::map<std::string, std::string> files;

public:
	/// Constructors
	// Constructor without parameters
	Project() :  
		title(""), 
		description(""),
		default_user_perm(USER_PERM_NONE),
		user_perms({}), 
		files({})
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Constructor without parameters.\n";
	}
	
	// Constructor with parameters
	Project(
		const std::string& title_ = "", 
		const std::string& description_ = "",
		const int default_user_perm_ = USER_PERM_NONE,
		const std::map<User*, int>& user_perms_ = {},
		const std::map<std::string, std::string>& files_ = {}
	) : 
		default_user_perm(default_user_perm_),
		title(title_),
		description(description_),
		user_perms(user_perms_),
		files(files_)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Constructor with parameters.\n";
	}

	// Copy Constructor
	Project(const Project& other) :
		default_user_perm(other.default_user_perm),
		title(other.title),
		description(other.description),
		user_perms(other.user_perms),
		files(other.files)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Copy Constructor.\n";
	}

	// Copy Operator
	Project& operator=(const Project& other) {
		default_user_perm = other.default_user_perm;
		title = other.title;
		description = other.description;
		user_perms = other.user_perms;
		files = other.files;

		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Copy Operator.\n";
		return *this;
	}

	// Destructor
	~Project() {
		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Destructor.\n";
	}

	// OStream Operator (#TODO)
	friend std::ostream& operator<<(std::ostream& os, const Project& p) {
		os << "'Project': Cout for title '" << p.title << "'.\n";
		return os;
	}

	// Getters & Setters
	int getUserPerm(User* user) {
		if (user_perms.find(user) == user_perms.end())
			return default_user_perm;
		return user_perms[user];
	}

	void setUserPerm(User* auth, User* user, const int user_perm) {
		if (getUserPerm(auth) >= USER_PERM_OWNER)
			user_perms[user] = user_perm;
	}

	int getDefaultUserPerm() const {
		return default_user_perm;
	}

	void setDefaultUserPerm(User* auth, const int user_perm) {
		if (getUserPerm(auth) >= USER_PERM_OWNER)
			default_user_perm = user_perm;
	}

	const std::string& getTitle(User* auth) {
		if (getUserPerm(auth) >= USER_PERM_VIEWER)
			return title;
		return STRING_PRIVATE;
	}

	void setTitle(User* auth, const std::string& title_) {
		if (getUserPerm(auth) >= USER_PERM_OWNER)
			title = title_;
	}

	const std::string& getDescription(User* auth) {
		if (getUserPerm(auth) >= USER_PERM_VIEWER)
			return title;
		return STRING_PRIVATE;
	}

	void setDescription(User* auth, const std::string& description_) {
		if (getUserPerm(auth) >= USER_PERM_OWNER)
			description = description_;
	}

	const std::string& getFileContent(User* auth, const std::string& file_path) {
		if (getUserPerm(auth) >= USER_PERM_VIEWER)
			return files[file_path];
		return STRING_PRIVATE;
	}

	void setFileContent(User* auth, const std::string& file_name, const std::string& file_content) {
		if (getUserPerm(auth) >= USER_PERM_MAINTAINER)
			files[file_name] = file_content;
	}
};