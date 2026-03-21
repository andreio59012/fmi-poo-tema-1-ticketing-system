#pragma once
#include <string>
#include <iostream>
#include "Project.h"

class Ticket
{
private:
	User* owner;
	Project* project;
	bool closed;
	std::string title, description;

public:
	// Constructor
	explicit Ticket(
		User* owner_ = nullptr,
		Project* project_ = nullptr,
		const bool closed_ = false,
		const std::string& title_ = "",
		const std::string& description_ = ""
	) :
		owner(owner_),
		project(project_),
		closed(closed_),
		title(title_),
		description(description_)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'Ticket': Constructor with parameters.\n";
	}

	// Copy Constructor
	Ticket(const Ticket& other) :
		owner(other.owner),
		project(other.project),
		closed(other.closed),
		title(other.title),
		description(other.description)
	{
		if (LOG_CONSTRUCTORS)
			std::cout << "'Ticket': Copy Constructor.\n";
	}

	// Copy Operator
	Ticket& operator=(const Ticket& other) {
		owner = other.owner;
		project = other.project;
		closed = other.closed;
		title = other.title;
		description = other.description;

		if (LOG_CONSTRUCTORS)
			std::cout << "'Ticket': Copy Operator.\n";

		return *this;
	}

	// OStream Operator (#TODO)
	friend std::ostream& operator<<(std::ostream& os, const Ticket& p) {
		os << "Ticket:\n\tTitle: " << p.title << "\n\tDescription: " << p.description << "\n\tOwner: " << p.owner->getUsername() << "\n\tClosed: " << p.closed << "\n";
		return os;
	}

	// Destructor
	~Ticket() {
		if (LOG_CONSTRUCTORS)
			std::cout << "'Ticket': Destructor.\n";
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

	bool getClosed() const { return closed; }

	void setClosed(const User* auth, const bool closed_) {
		if (auth == owner || project->getUserPerm(auth) >= USER_PERM_REVIEWER)
			closed = closed_;
	}
};