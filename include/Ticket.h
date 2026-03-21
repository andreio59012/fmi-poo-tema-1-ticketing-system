#pragma once
#include <iostream>
#include <cstring>
#include "Project.h"

class Ticket
{
private:
    User* owner;
    Project* project;
    bool closed;
    char title[FIXED_STRING_SIZE];
    char description[FIXED_STRING_SIZE];

public:
    explicit Ticket(
        User* owner_ = nullptr,
        Project* project_ = nullptr,
        const bool closed_ = false,
        const char* title_ = "",
        const char* description_ = ""
    ) :
        owner(owner_),
        project(project_),
        closed(closed_)
    {
        strncpy(title, title_, FIXED_STRING_SIZE - 1);
        title[FIXED_STRING_SIZE - 1] = '\0';
        strncpy(description, description_, FIXED_STRING_SIZE - 1);
        description[FIXED_STRING_SIZE - 1] = '\0';

        if (LOG_CONSTRUCTORS)
            std::cout << "'Ticket': Constructor with parameters.\n";
    }

    Ticket(const Ticket& other) :
        owner(other.owner),
        project(other.project),
        closed(other.closed)
    {
        strncpy(title, other.title, FIXED_STRING_SIZE);
        strncpy(description, other.description, FIXED_STRING_SIZE);

        if (LOG_CONSTRUCTORS)
            std::cout << "'Ticket': Copy Constructor.\n";
    }

    Ticket& operator=(const Ticket& other) {
        owner = other.owner;
        project = other.project;
        closed = other.closed;
        strncpy(title, other.title, FIXED_STRING_SIZE);
        strncpy(description, other.description, FIXED_STRING_SIZE);

        if (LOG_CONSTRUCTORS)
            std::cout << "'Ticket': Copy Operator.\n";
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Ticket& p) {
        os << "Ticket:\n\tTitle: " << p.title << "\n\tDescription: " << p.description << "\n\tOwner: " << p.owner->getUsername() << "\n\tClosed: " << p.closed << "\n";
        return os;
    }

    ~Ticket() {
        if (LOG_CONSTRUCTORS)
            std::cout << "'Ticket': Destructor.\n";
    }

    User* getOwner() const { return owner; }
    Project* getProject() const { return project; }

    const char* getTitle(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return title;
        return STRING_PRIVATE;
    }

    void setTitle(const User* auth, const char* title_) {
        if (auth == owner) {
            strncpy(title, title_, FIXED_STRING_SIZE - 1);
            title[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    const char* getDescription(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return description;
        return STRING_PRIVATE;
    }

    void setDescription(const User* auth, const char* description_) {
        if (auth == owner) {
            strncpy(description, description_, FIXED_STRING_SIZE - 1);
            description[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    bool getClosed() const { return closed; }

    void setClosed(const User* auth, const bool closed_) {
        if (auth == owner || project->getUserPerm(auth) >= USER_PERM_REVIEWER)
            closed = closed_;
    }
};