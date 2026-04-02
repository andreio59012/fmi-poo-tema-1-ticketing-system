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
    // Constructor de initializare cu parametrii
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

    // Constructor de copiere
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

    // Operator de copiere
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

    // Afisarea datelor
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

    // Returneaza utilizatorul care a creat PR-ul
    User* getOwner() const { return owner; }

    // Returneaza proiectul caruia ii apartine PR-ul
    Project* getProject() const { return project; }

    // Returneaza titlul PR-ului daca auth are minim VIEWER in proiect, altfel STRING_PRIVATE
    const char* getTitle(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return title;
        return STRING_PRIVATE;
    }

    // Modifica titlul PR-ului; doar owner-ul PR-ului poate face asta
    void setTitle(const User* auth, const char* title_) {
        if (auth == owner) {
            strncpy(title, title_, FIXED_STRING_SIZE - 1);
            title[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    // Returneaza descrierea PR-ului daca auth are minim VIEWER in proiect, altfel STRING_PRIVATE
    const char* getDescription(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return description;
        return STRING_PRIVATE;
    }

    // Modifica descrierea PR-ului; doar owner-ul PR-ului poate face asta
    void setDescription(const User* auth, const char* description_) {
        if (auth == owner) {
            strncpy(description, description_, FIXED_STRING_SIZE - 1);
            description[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    // Returneaza starea curenta a PR-ului
    int getState() const { return state; }

    // Schimba starea PR-ului cu un comentariu de review; owner-ul poate seta DRAFT/AWAITING_REVIEW,
    // un REVIEWER poate seta NEEDS_CHANGES/MERGED/BLOCKED; la MERGED fisierele sunt aplicate pe proiect
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
       
    // Returneaza numarul de fisiere modificate daca auth are minim VIEWER, altfel 0
    int getChangedFileCount(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return changed_file_count;
        return 0;
    }

    // Returneaza lista de fisiere modificate daca auth are minim VIEWER, altfel nullptr
    const File* getChangedFiles(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return changed_files;
        return nullptr;
    }

    // Adauga sau modifica un fisier in lista de fisiere schimbate ale PR-ului; doar owner-ul poate face asta
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