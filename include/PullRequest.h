#pragma once
#include <map>
#include <set>
#include <string>
#include <iostream>
#include "Project.h"
#include "Ticket.h"
#include "Constants.h"

const int PR_STATE_DRAFT = 0;
const int PR_STATE_AWAITING_REVIEW = 1;
const int PR_STATE_NEEDS_CHANGES = 2;
const int PR_STATE_MERGED = 3;
const int PR_STATE_BLOCKED = 4;

class PullRequest
{
private:
	Project* project;
	User* owner;
	int state;

	std::string title, description, review_comment;
	std::map<std::string, std::string> changed_files;

public:
	// Constructor without parameters
	PullRequest() :
		owner(nullptr),
		project(nullptr),
		state(0),
		title(""),
		description(""),
		review_comment(""),
		changed_files({})
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'PullRequest': Constructor without parameters.\n";
	}

	// Constructor with parameters
	PullRequest(
		User* owner_ = nullptr,
		Project* project_ = nullptr,
		const std::string& title_ = "",
		const std::string& description_ = "",
		const std::map<std::string, std::string>& changed_files_ = {}
	) :
		owner(owner_),
		project(project_),
		state(PR_STATE_DRAFT),
		title(title_),
		description(description_),
		review_comment(""),
		changed_files(changed_files_)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'PullRequest': Constructor with parameters.\n";
	}

	// Copy Constructor
	PullRequest(const PullRequest& other) :
		owner(other.owner),
		project(other.project),
		state(other.state),
		title(other.title),
		description(other.description),
		review_comment(other.review_comment),
		changed_files(other.changed_files)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'PullRequest': Copy Constructor.\n";
	}

	// Copy Operator
	PullRequest& operator=(const PullRequest& other) {
		owner = other.owner;
		project = other.project;
		state = other.state;
		title = other.title;
		description = other.description;
		review_comment = other.review_comment;
		changed_files = other.changed_files;

		if (LOG_CONSTRUCTORS)
			std::cout << "'PullRequest': Copy Operator.\n";

		return *this;
	}

	// OStream Operator (#TODO)
	friend std::ostream& operator<<(std::ostream& os, const PullRequest& p) {
		os << "'PullRequest': Cout for title '" << p.title << "'.\n";
		return os;
	}

	// Destructor
	~PullRequest() {
		if (LOG_CONSTRUCTORS)
			std::cout << "'PullRequest': Destructor.\n";
	}

	// Getters & Setters
	User* getOwner() const { return owner; }
	Project* getProject() const { return project; }

	const std::string& getTitle(User* auth) {
		if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
			return title;
		return STRING_PRIVATE;
	}

	void setTitle(User* auth, const std::string& title_) {
		if (auth == owner)
			title = title_;
	}

	const std::string& getDescription(User* auth) {
		if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
			return title;
		return STRING_PRIVATE;
	}

	void setDescription(User* auth, const std::string& description_) {
		if (auth == owner)
			description = description_;
	}

	bool getState() const { return state; }

	void setState(User* auth, const int state_, const std::string& review_comment_ = "")
	{
		if (state == PR_STATE_MERGED)
			return;

		if (
			(
				project->getUserPerm(auth) >= USER_PERM_REVIEWER &&
				(state_ == PR_STATE_NEEDS_CHANGES || state_ == PR_STATE_MERGED || state_ == PR_STATE_BLOCKED)
			) || (
				auth == owner &&
				(state_ == PR_STATE_DRAFT || state_ == PR_STATE_AWAITING_REVIEW)
			)
		) {
			state = state_;
			review_comment = review_comment_;

			if (state == PR_STATE_MERGED)
				for (auto const& pair : changed_files)
					project->setFileContent(auth, pair.first, pair.second);
		}
	}

	const std::map<std::string, std::string> getChangedFiles(User* auth)
	{
		if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
			return changed_files;
		return {};
	}

	const std::string& getChangedFileContent(User* auth, const std::string& file_path) {
		if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
			return changed_files[file_path];
		return STRING_PRIVATE;
	}

	void setChangedFileContent(User* auth, const std::string& file_name, const std::string& file_content) {
		if (auth == owner)
			changed_files[file_name] = file_content;
	}
};