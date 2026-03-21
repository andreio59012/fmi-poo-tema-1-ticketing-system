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
	std::string title, description;
	int default_user_perm;
	std::map<const User*, int> user_perms;
	std::map<std::string, std::string> files;

public:
	// Constructor
	explicit Project(
		const std::string& title_ = "", 
		const std::string& description_ = "",
		const int default_user_perm_ = USER_PERM_NONE,
		const std::map<const User*, int>& user_perms_ = {},
		const std::map<std::string, std::string>& files_ = {}
	) : 
		title(title_),
		description(description_),
		default_user_perm(default_user_perm_),
		user_perms(user_perms_),
		files(files_)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Constructor with parameters.\n";
	}

	// Copy Constructor
	Project(const Project& other) :
		title(other.title),
		description(other.description),
		default_user_perm(other.default_user_perm),
		user_perms(other.user_perms),
		files(other.files)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Copy Constructor.\n";
	}

	// Copy Operator
	Project& operator=(const Project& other) {
		title = other.title;
		description = other.description;
		default_user_perm = other.default_user_perm;
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
		os << "Project:\n\tTitle: " << p.title << "\n\tDescription: " << p.description << "\n\tDefault User Permission: " << p.default_user_perm << "\n";

		if (p.user_perms.size() > 0) {
			std::cout << "\tUsers: ";
			for (const auto& pair : p.user_perms) {
				os << pair.first->getUsername() << " (" << pair.second << "), ";
			}
			std::cout << '\n';
		}

		if (p.files.size() > 0) {
			std::cout << "\tFiles: ";
			for (const auto& pair : p.files) {
				os << pair.first << ": " << pair.second << "; ";
			}
			std::cout << '\n';
		}

		return os;
	}

	// Getters & Setters
	int getUserPerm(const User* user) const {
		auto a = user_perms.find(user);
		if (a == user_perms.end())
			return default_user_perm;
		return a->second;
	}

	void setUserPerm(const User* auth, User* user, const int user_perm) {
		if (getUserPerm(auth) >= USER_PERM_OWNER)
			user_perms[user] = user_perm;
	}

	int getDefaultUserPerm() const {
		return default_user_perm;
	}

	void setDefaultUserPerm(const User* auth, const int user_perm) {
		if (getUserPerm(auth) >= USER_PERM_OWNER)
			default_user_perm = user_perm;
	}

	const std::string& getTitle(const User* auth) const {
		if (getUserPerm(auth) >= USER_PERM_VIEWER)
			return title;
		return STRING_PRIVATE;
	}

	void setTitle(const User* auth, const std::string& title_) {
		if (getUserPerm(auth) >= USER_PERM_OWNER)
			title = title_;
	}

	const std::string& getDescription(const User* auth) const {
		if (getUserPerm(auth) >= USER_PERM_VIEWER)
			return title;
		return STRING_PRIVATE;
	}

	void setDescription(const User* auth, const std::string& description_) {
		if (getUserPerm(auth) >= USER_PERM_OWNER)
			description = description_;
	}

	const std::map<std::string, std::string> getFiles(const User* auth) const
	{
		if (getUserPerm(auth) >= USER_PERM_VIEWER)
			return files;
		return {};
	}

	void setFileContent(const User* auth, const std::string& file_name, const std::string& file_content) {
		if (getUserPerm(auth) >= USER_PERM_MAINTAINER)
			files[file_name] = file_content;
	}
};