#pragma once
#include <iostream>
#include <cstring>
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
    char title[FIXED_STRING_SIZE];
    char description[FIXED_STRING_SIZE];
    char review_comment[FIXED_STRING_SIZE];
    int changed_file_count = 0;
    File changed_files[FIXED_ARRAY_SIZE];

public:
    explicit PullRequest(
        User* owner_ = nullptr,
        Project* project_ = nullptr,
        const char* title_ = "",
        const char* description_ = ""
    ) :
        owner(owner_),
        project(project_),
        state(PR_STATE_DRAFT),
        changed_file_count(0),
        changed_files()
    {
        strncpy(title, title_, FIXED_STRING_SIZE - 1);
        title[FIXED_STRING_SIZE - 1] = '\0';
        strncpy(description, description_, FIXED_STRING_SIZE - 1);
        description[FIXED_STRING_SIZE - 1] = '\0';
        review_comment[0] = '\0';

        if (LOG_CONSTRUCTORS)
            std::cout << "'PullRequest': Constructor with parameters.\n";
    }

    PullRequest(const PullRequest& other) :
        owner(other.owner),
        project(other.project),
        state(other.state),
        changed_file_count(other.changed_file_count)
    {
        strncpy(title, other.title, FIXED_STRING_SIZE);
        strncpy(description, other.description, FIXED_STRING_SIZE);
        strncpy(review_comment, other.review_comment, FIXED_STRING_SIZE);

        for (int i = 0; i < other.changed_file_count; i++)
            changed_files[i] = other.changed_files[i];

        if (LOG_CONSTRUCTORS)
            std::cout << "'PullRequest': Copy Constructor.\n";
    }

    PullRequest& operator=(const PullRequest& other) {
        owner = other.owner;
        project = other.project;
        state = other.state;
        changed_file_count = other.changed_file_count;
        strncpy(title, other.title, FIXED_STRING_SIZE);
        strncpy(description, other.description, FIXED_STRING_SIZE);
        strncpy(review_comment, other.review_comment, FIXED_STRING_SIZE);

        for (int i = 0; i < other.changed_file_count; i++)
            changed_files[i] = other.changed_files[i];

        if (LOG_CONSTRUCTORS)
            std::cout << "'PullRequest': Copy Operator.\n";
        return *this;
    }

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

    ~PullRequest() {
        if (LOG_CONSTRUCTORS)
            std::cout << "'PullRequest': Destructor.\n";
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

    int getState() const { return state; }

    void setState(const User* auth, const int state_, const char* review_comment_ = "") {
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
            strncpy(review_comment, review_comment_, FIXED_STRING_SIZE - 1);
            review_comment[FIXED_STRING_SIZE - 1] = '\0';

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

    const File* getChangedFiles(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return changed_files;
        return nullptr;
    }

    void setChangedFileContent(const User* auth, const char* file_path, const char* file_content) {
        if (auth != owner)
            return;

        for (int i = 0; i < changed_file_count; i++)
            if (strncmp(changed_files[i].path, file_path, FIXED_STRING_SIZE) == 0) {
                strncpy(changed_files[i].content, file_content, FIXED_STRING_SIZE - 1);
                changed_files[i].content[FIXED_STRING_SIZE - 1] = '\0';
                return;
            }

        if (changed_file_count < FIXED_ARRAY_SIZE - 1) {
            strncpy(changed_files[changed_file_count].path, file_path, FIXED_STRING_SIZE - 1);
            changed_files[changed_file_count].path[FIXED_STRING_SIZE - 1] = '\0';
            strncpy(changed_files[changed_file_count].content, file_content, FIXED_STRING_SIZE - 1);
            changed_files[changed_file_count].content[FIXED_STRING_SIZE - 1] = '\0';
            changed_file_count++;
        }
    }
};