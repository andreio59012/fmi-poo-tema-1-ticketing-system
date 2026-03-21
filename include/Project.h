#pragma once
#include <string>
#include <iostream>
#include "User.h"
#include "Constants.h"

const int USER_PERM_NONE = 0;		// No perms.
const int USER_PERM_VIEWER = 1;		// View perms.
const int USER_PERM_MAINTAINER = 2;	// Create Ticket/PR perms.
const int USER_PERM_REVIEWER = 3;	// Review/Close Ticket/PR perms.
const int USER_PERM_OWNER = 4;		// Change project settings.

struct UserPerm {
	User* user;
	int perm;
};

struct File {
	std::string path;
	std::string content;
};

class Project 
{
private:
	std::string title, description;
	int default_user_perm;

	int user_perm_count = 0;
	UserPerm user_perms[FIXED_ARRAY_SIZE];

	int file_count = 0;
	File files[FIXED_ARRAY_SIZE];

public:
	// Constructor
	explicit Project(
		const std::string& title_ = "", 
		const std::string& description_ = "",
		const int default_user_perm_ = USER_PERM_NONE
	) : 
		title(title_),
		description(description_),
		default_user_perm(default_user_perm_),
		user_perm_count(0),
		user_perms(),
		file_count(0),
		files()
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Constructor with parameters.\n";
	}

	// Copy Constructor
	Project(const Project& other) :
		title(other.title),
		description(other.description),
		default_user_perm(other.default_user_perm),
		user_perm_count(other.user_perm_count),
		file_count(other.file_count)
	{
		for (int i = 0; i < other.user_perm_count; i++)
			user_perms[i] = other.user_perms[i];

		for (int i = 0; i < other.file_count; i++)
			files[i] = other.files[i];

		if (LOG_CONSTRUCTORS)
			std::cout << "'Project': Copy Constructor.\n";
	}

	// Copy Operator
	Project& operator=(const Project& other) {
		title = other.title;
		description = other.description;
		default_user_perm = other.default_user_perm;
		user_perm_count = other.user_perm_count;
		file_count = other.file_count;

		for (int i = 0; i < other.user_perm_count; i++)
			user_perms[i] = other.user_perms[i];

		for (int i = 0; i < other.file_count; i++)
			files[i] = other.files[i];

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

		if (p.user_perm_count > 0) {
			os << "\tUsers: ";
			for (int i = 0; i < p.user_perm_count; i++)
				os << p.user_perms[i].user->getUsername() << " (" << p.user_perms[i].perm << "), ";
			os << '\n';
		}

		if (p.file_count > 0) {
			os << "\tFiles: ";
			for (int i = 0; i < p.file_count; i++)
				os << p.files[i].path << ": " << p.files[i].content << "; ";
			os << '\n';
		}

		return os;
	}

	// Getters & Setters
	int getUserPerm(const User* user) const {
		for (int i = 0; i < user_perm_count; i++)
			if (user_perms[i].user == user)
				return user_perms[i].perm;
		return default_user_perm;
	}

	void setUserPerm(const User* auth, User* user, const int user_perm) {
		if (user_perm_count > 0 && getUserPerm(auth) < USER_PERM_OWNER)
			return;

		for (int i = 0; i < user_perm_count; i++)
			if (user_perms[i].user == user) {
				user_perms[i].perm = user_perm;
				return;
			}

		if (user_perm_count < FIXED_ARRAY_SIZE - 1) {
			user_perms[user_perm_count].user = user;
			user_perms[user_perm_count].perm = user_perm;
			user_perm_count += 1;
		}
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

	int getFileCount(const User* auth) const {
		if (getUserPerm(auth) >= USER_PERM_VIEWER)
			return file_count;
		return 0;
	}

	const File* getFiles(const User* auth) const
	{
		if (getUserPerm(auth) >= USER_PERM_VIEWER)
			return files;
		return {};
	}

	void setFileContent(const User* auth, const std::string& file_path, const std::string& file_content) {
		if (getUserPerm(auth) >= USER_PERM_MAINTAINER)
			return;

		for (int i = 0; i < file_count; i++)
			if (files[i].path == file_path) {
				files[i].content = file_content;
				return;
			}

		if (file_count < FIXED_ARRAY_SIZE - 1) {
			files[file_count].path = file_path;
			files[file_count].content = file_content;
			file_count += 1;
		}
	}
};