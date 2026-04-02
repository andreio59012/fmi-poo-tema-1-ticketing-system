#pragma once
#include <iostream>
#include <cstring>
#include "User.h"
#include "Constants.h"

const int USER_PERM_NONE = 0;
const int USER_PERM_VIEWER = 1;
const int USER_PERM_MAINTAINER = 2;
const int USER_PERM_REVIEWER = 3;
const int USER_PERM_OWNER = 4;

struct UserPerm {
    User* user;
    int perm;
};

struct File {
    char path[FIXED_STRING_SIZE];
    char content[FIXED_STRING_SIZE];
};

class Project
{
private:
    char title[FIXED_STRING_SIZE];
    char description[FIXED_STRING_SIZE];
    int default_user_perm;

    int user_perm_count = 0;
    UserPerm user_perms[FIXED_ARRAY_SIZE];

    int file_count = 0;
    File files[FIXED_ARRAY_SIZE];

public:
    //Constructor cu parametrii
    explicit Project(
        const char* title_ = "",
        const char* description_ = "",
        const int default_user_perm_ = USER_PERM_NONE
    ) :
        default_user_perm(default_user_perm_),
        user_perm_count(0),
        user_perms(),
        file_count(0),
        files()
    {
        strncpy(title, title_, FIXED_STRING_SIZE - 1);
        title[FIXED_STRING_SIZE - 1] = '\0';
        strncpy(description, description_, FIXED_STRING_SIZE - 1);
        description[FIXED_STRING_SIZE - 1] = '\0';

        if (LOG_CONSTRUCTORS)
            std::cout << "'Project': Constructor with parameters.\n";
    }

    // Constructor de copiere
    Project(const Project& other) :
        default_user_perm(other.default_user_perm),
        user_perm_count(other.user_perm_count),
        file_count(other.file_count)
    {
        strncpy(title, other.title, FIXED_STRING_SIZE);
        strncpy(description, other.description, FIXED_STRING_SIZE);

        for (int i = 0; i < other.user_perm_count; i++)
            user_perms[i] = other.user_perms[i];

        for (int i = 0; i < other.file_count; i++)
            files[i] = other.files[i];

        if (LOG_CONSTRUCTORS)
            std::cout << "'Project': Copy Constructor.\n";
    }

    // Operator de copiere
    Project& operator=(const Project& other) {
        strncpy(title, other.title, FIXED_STRING_SIZE);
        strncpy(description, other.description, FIXED_STRING_SIZE);
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

    // Afisarea datelor
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

    // Returneaza permisiunea unui utilizator in proiect; daca nu are permisiune explicita, returneaza permisiunea implicita
    int getUserPerm(const User* user) const {
        for (int i = 0; i < user_perm_count; i++)
            if (user_perms[i].user == user)
                return user_perms[i].perm;
        return default_user_perm;
    }

    // Seteaza permisiunea unui utilizator in proiect; necesita ca auth sa fie OWNER
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
            user_perm_count++;
        }
    }

    // Returneaza permisiunea implicita pentru utilizatori fara permisiune explicita
    int getDefaultUserPerm() const { return default_user_perm; }

    // Modifica permisiunea implicita; necesita OWNER
    void setDefaultUserPerm(const User* auth, const int user_perm) {
        if (getUserPerm(auth) >= USER_PERM_OWNER)
            default_user_perm = user_perm;
    }

    // Returneaza titlul proiectului daca auth are minim VIEWER, altfel STRING_PRIVATE
    const char* getTitle(const User* auth) const {
        if (getUserPerm(auth) >= USER_PERM_VIEWER)
            return title;
        return STRING_PRIVATE;
    }

    // Modifica titlul proiectului; necesita OWNER
    void setTitle(const User* auth, const char* title_) {
        if (getUserPerm(auth) >= USER_PERM_OWNER) {
            strncpy(title, title_, FIXED_STRING_SIZE - 1);
            title[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    // Returneaza descrierea proiectului daca auth are minim VIEWER, altfel STRING_PRIVATE
    const char* getDescription(const User* auth) const {
        if (getUserPerm(auth) >= USER_PERM_VIEWER)
            return description;
        return STRING_PRIVATE;
    }

    // Modifica descrierea proiectului; necesita OWNER
    void setDescription(const User* auth, const char* description_) {
        if (getUserPerm(auth) >= USER_PERM_OWNER) {
            strncpy(description, description_, FIXED_STRING_SIZE - 1);
            description[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    // Returneaza numarul de fisiere din proiect daca auth are minim VIEWER, altfel 0
    int getFileCount(const User* auth) const {
        if (getUserPerm(auth) >= USER_PERM_VIEWER)
            return file_count;
        return 0;
    }

    // Returneaza lista de fisiere daca auth are minim VIEWER, altfel nullptr
    const File* getFiles(const User* auth) const {
        if (getUserPerm(auth) >= USER_PERM_VIEWER)
            return files;
        return nullptr;
    }

    // Adauga sau modifica un fisier in proiect; necesita minim MAINTAINER
    void setFileContent(const User* auth, const char* file_path, const char* file_content) {
        if (getUserPerm(auth) < USER_PERM_MAINTAINER)
            return;

        for (int i = 0; i < file_count; i++)
            if (strncmp(files[i].path, file_path, FIXED_STRING_SIZE) == 0) {
                strncpy(files[i].content, file_content, FIXED_STRING_SIZE - 1);
                files[i].content[FIXED_STRING_SIZE - 1] = '\0';
                return;
            }

        if (file_count < FIXED_ARRAY_SIZE - 1) {
            strncpy(files[file_count].path, file_path, FIXED_STRING_SIZE - 1);
            files[file_count].path[FIXED_STRING_SIZE - 1] = '\0';
            strncpy(files[file_count].content, file_content, FIXED_STRING_SIZE - 1);
            files[file_count].content[FIXED_STRING_SIZE - 1] = '\0';
            file_count++;
        }
    }
};