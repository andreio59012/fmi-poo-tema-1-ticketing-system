#pragma once
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
	User* owner;
	Project* project;
	int state;
	std::string title, description, review_comment;
	int changed_file_count = 0;
	File changed_files[FIXED_ARRAY_SIZE];

public:
	// Constructor
	explicit PullRequest(
		User* owner_ = nullptr,
		Project* project_ = nullptr,
		const std::string& title_ = "",
		const std::string& description_ = ""
	) :
		owner(owner_),
		project(project_),
		state(PR_STATE_DRAFT),
		title(title_),
		description(description_),
		review_comment(""),
		changed_file_count(0),
		changed_files()
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
		changed_file_count(other.changed_file_count)
	{
		for (int i = 0; i < other.changed_file_count; i++)
			changed_files[i] = other.changed_files[i];

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
		changed_file_count = other.changed_file_count;

		for (int i = 0; i < other.changed_file_count; i++)
			changed_files[i] = other.changed_files[i];

		if (LOG_CONSTRUCTORS)
			std::cout << "'PullRequest': Copy Operator.\n";

		return *this;
	}

	// OStream Operator (#TODO)
	friend std::ostream& operator<<(std::ostream& os, const PullRequest& p) {
		os << "Pull Request:\n\tTitle: " << p.title << "\n\tDescription: " << p.description << "\n\tOwner: " << p.owner->getUsername() << "\n\tState: " << p.state << "\n\tReview Comment: " << p.review_comment << "\n";

		if (p.changed_file_count > 0) {
			os << "\tFiles: ";
			for (int i = 0; i < p.changed_file_count; i++)
				os << p.changed_files[i].path << ": " << p.changed_files[i].content << "; ";
			os << '\n';
		}

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

	const std::string& getTitle(const User* auth) const {
		if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
			return title;
		return STRING_PRIVATE;
	}

	void setTitle(const User* auth, const std::string& title_) {
		if (auth == owner)
			title = title_;
	}

	const std::string& getDescription(const User* auth) const {
		if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
			return title;
		return STRING_PRIVATE;
	}

	void setDescription(const User* auth, const std::string& description_) {
		if (auth == owner)
			description = description_;
	}

	int getState() const { return state; }

	void setState(const User* auth, const int state_, const std::string& review_comment_ = "")
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
				for (int i = 0; i < changed_file_count; i++)
					project->setFileContent(auth, changed_files[i].path, changed_files[i].content);
		}
	}

	int getChangedFileCount(const User* auth) const {
		if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
			return changed_file_count;
		return 0;
	}

	const File* getChangedFiles(const User* auth) const
	{
		if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
			return changed_files;
		return {};
	}

	void setChangedFileContent(const User* auth, const std::string& file_path, const std::string& file_content) {
		if (auth != owner)
			return;

		for (int i = 0; i < changed_file_count; i++)
			if (changed_files[i].path == file_path) {
				changed_files[i].content = file_content;
				return;
			}

		if (changed_file_count < FIXED_ARRAY_SIZE - 1) {
			changed_files[changed_file_count].path = file_path;
			changed_files[changed_file_count].content = file_content;
			changed_file_count += 1;
		}
	}
};