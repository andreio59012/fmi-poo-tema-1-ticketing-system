/*
Tot codul din proiect mutat intr-un singur fisier.

Programul simuleaza o platforma de gestionare a aplicatiilor software, similara cu GitHub/GitLab.
Utilizatorii pot sa creeze si sa modifice proiecte, care au fisiere, un sistem de permisiuni,
**tickete** pentru raportarea problemelor, si pull-requesti pentru propuneri de modificari.
Date de exemplu sunt generate la inceputul rularii programului, iar alte date in plus pot fi introduse de utilizator.

Date de intrare:
- Utilizatori: nume de utilizator si parola
- Proiecte: titlu, descriere, permisiune pentru utilizatori noi, fisiere
- Tickete: titlu, descriere, proiectul si utilizatorul care l-a creat, starea (deschis/inchis)
- Pull Request-uri: titlu, descriere, fisierele modificate (cale + continut),
	starea (draft, asteapta review, trebuie modificat, merged, blocat)
- Permisiuni per utilizator per proiect: NONE(0), VIEWER(1), MAINTAINER(2),
	REVIEWER(3), OWNER(4)

Operatii posibile:
- Utilizatori: creare cont, autentificare, modificare date personale
- Proiecte: creare, modificare titlu/descriere, gestionare permisiuni utilizatori,
	setare permisiune implicita
- Tickete: creare (minim MAINTAINER), modificare (doar owner-ul ticketului),
	inchidere/deschidere (owner sau minim REVIEWER)
- Pull Request-uri: creare (minim MAINTAINER), modificare titlu/descriere/fisiere
	(doar owner-ul PR-ului), schimbare stare (owner-ul poate pune DRAFT sau AWAITING REVIEW,
	un REVIEWER poate pune NEEDS CHANGES, MERGED sau BLOCKED; la MERGED fisierele schimbate
	in PR sunt schimbate si in proiect)
*/

#include <iostream>
#include <cstring>

const int FIXED_ARRAY_SIZE = 100;
const int FIXED_STRING_SIZE = 256;
const bool LOG_CONSTRUCTORS = false;
const char STRING_PRIVATE[] = "(PRIVATE)";

class User
{
private:
    char username[FIXED_STRING_SIZE];
    char password[FIXED_STRING_SIZE];

public:
    // Constructor de initializare cu parametrii
    explicit User(
        const char* username_ = "",
        const char* password_ = ""
    ) {
        strncpy(username, username_, FIXED_STRING_SIZE - 1);
        username[FIXED_STRING_SIZE - 1] = '\0';
        strncpy(password, password_, FIXED_STRING_SIZE - 1);
        password[FIXED_STRING_SIZE - 1] = '\0';

        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Constructor with parameters for username '" << username << "'.\n";
    }

    // Constructor de mutare
    User(User&& other) noexcept {
        strncpy(username, other.username, FIXED_STRING_SIZE);
        strncpy(password, other.password, FIXED_STRING_SIZE);

        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Move Constructor for username '" << username << "'.\n";
    }

    // Constructor de copiere
    User(const User& other) {
        strncpy(username, other.username, FIXED_STRING_SIZE);
        strncpy(password, other.password, FIXED_STRING_SIZE);

        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Copy Constructor for username '" << username << "'.\n";
    }

    // Operator de copiere
    User& operator=(const User& other) {
        strncpy(username, other.username, FIXED_STRING_SIZE);
        strncpy(password, other.password, FIXED_STRING_SIZE);

        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Copy Operator for username '" << username << "'.\n";
        return *this;
    }

    // Afisarea datelor
    friend std::ostream& operator<<(std::ostream& os, const User& p) {
        os << "User:\n\tUsername: " << p.username << "\n\tPassword: " << p.password << "\n";
        return os;
    }

    // Destructor
    ~User() {
        if (LOG_CONSTRUCTORS)
            std::cout << "'User': Destructor for username '" << username << "'.\n";
    }

    // Returneaza username-ul utilizatorului
    const char* getUsername() const { return username; }

    // Verifica daca parola data coincide cu parola utilizatorului
    bool isPasswordCorrect(const char* password_) const {
        return strncmp(password, password_, FIXED_STRING_SIZE) == 0;
    }

    // Modifica username-ul (daca auth poate face asta)
    void setUsername(const User* auth, const char* username_) {
        if (auth == this) {
            strncpy(username, username_, FIXED_STRING_SIZE - 1);
            username[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    // Modifica parola (daca auth poate face asta)
    void setPassword(const User* auth, const char* password_) {
        if (auth == this) {
            strncpy(password, password_, FIXED_STRING_SIZE - 1);
            password[FIXED_STRING_SIZE - 1] = '\0';
        }
    }
};

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

class Ticket
{
private:
    User* owner;
    Project* project;
    bool closed;
    char title[FIXED_STRING_SIZE];
    char description[FIXED_STRING_SIZE];

public:
    //Constructor de initializare cu parametrii
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

    // Constructor de copiere
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

    // Operator de copiere
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

    // Afisarea datelor
    friend std::ostream& operator<<(std::ostream& os, const Ticket& p) {
        os << "Ticket:\n\tTitle: " << p.title << "\n\tDescription: " << p.description << "\n\tOwner: " << p.owner->getUsername() << "\n\tClosed: " << p.closed << "\n";
        return os;
    }

    // Destructor
    ~Ticket() {
        if (LOG_CONSTRUCTORS)
            std::cout << "'Ticket': Destructor.\n";
    }

    // Returneaza utilizatorul care a creat ticketul
    User* getOwner() const { return owner; }

    // Returneaza proiectul caruia ii apartine ticketul
    Project* getProject() const { return project; }

    // Returneaza titlul ticketului daca auth are minim VIEWER in proiect, altfel STRING_PRIVATE
    const char* getTitle(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return title;
        return STRING_PRIVATE;
    }

    // Modifica titlul ticketului; doar owner-ul ticketului poate face asta
    void setTitle(const User* auth, const char* title_) {
        if (auth == owner) {
            strncpy(title, title_, FIXED_STRING_SIZE - 1);
            title[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    // Returneaza descrierea ticketului daca auth are minim VIEWER in proiect, altfel STRING_PRIVATE
    const char* getDescription(const User* auth) const {
        if (project->getUserPerm(auth) >= USER_PERM_VIEWER)
            return description;
        return STRING_PRIVATE;
    }

    // Modifica descrierea ticketului; doar owner-ul ticketului poate face asta
    void setDescription(const User* auth, const char* description_) {
        if (auth == owner) {
            strncpy(description, description_, FIXED_STRING_SIZE - 1);
            description[FIXED_STRING_SIZE - 1] = '\0';
        }
    }

    // Returneaza daca ticketul este inchis sau nu
    bool getClosed() const { return closed; }

    // Schimba starea ticketului; poate fi facut de owner sau de un utilizator cu minim REVIEWER
    void setClosed(const User* auth, const bool closed_) {
        if (auth == owner || project->getUserPerm(auth) >= USER_PERM_REVIEWER)
            closed = closed_;
    }
};

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

struct Database {
	int user_count = 0;
	User* users[FIXED_ARRAY_SIZE];

	int project_count = 0;
	Project* projects[FIXED_ARRAY_SIZE];

	int ticket_count = 0;
	Ticket* tickets[FIXED_ARRAY_SIZE];

	int pr_count = 0;
	PullRequest* prs[FIXED_ARRAY_SIZE];
};

/// Forward Declarations
void cli_home(Database*, User*&);

void cli_user_view(Database*, User*&, User*);
void cli_user_view_vector(Database*, User*&, const int count, User* v[]);
void cli_user_view_in_project(Database*, User*&, const Project*);
void cli_user_update(Database*, User*&, User*);
void cli_user_login(Database*, User*&);
void cli_user_signup(Database*, User*&);
void cli_user_logout(Database*, User*&);

void cli_project_view(Database*, User*&, Project*);
void cli_project_view_vector(Database*, User*&, const int count, Project* v[]);
void cli_project_view_in_user(Database*, User*&, const User*);
void cli_project_create(Database*, User*&);
void cli_project_update(Database*, User*&, Project*);
void cli_project_modify_users(Database*, User*&, Project*);
void cli_project_modify_user_default(Database*, User*&, Project*);

void cli_ticket_view(Database*, User*&, Ticket*);
void cli_ticket_view_vector(Database*, User*&, const int count, Ticket* v[]);
void cli_ticket_view_in_user(Database*, User*&, const User*);
void cli_ticket_view_in_project(Database*, User*&, const Project*);
void cli_ticket_create(Database*, User*&, Project*);
void cli_ticket_update(Database*, User*&, Ticket*);
void cli_ticket_closed_update(Database*, User*&, Ticket*);

void cli_pr_view(Database*, User*&, PullRequest*);
void cli_pr_view_vector(Database*, User*&, const int count, PullRequest* v[]);
void cli_pr_view_in_user(Database*, User*&, const User*);
void cli_pr_view_in_project(Database*, User*&, const Project*);
void cli_pr_create(Database*, User*&, Project*);
void cli_pr_update(Database*, User*&, PullRequest*);
void cli_pr_state_update(Database*, User*&, PullRequest*);
void cli_pr_file_update(Database*, User*&, PullRequest*);

int main()
{
	Database db;

	// Generate Users
	std::cout << "Creating Users...\n";
	db.user_count = 5;
	db.users[0] = new User("Samantha Gray", "password123");
	db.users[1] = new User("Jonathan Roberts", "NO_HACKING_ALLOWED");
	db.users[2] = new User("Julie Rich", "march252006");
	db.users[3] = new User("Andrew Knox", "CM%T_u&90q!A+Ojg*jA");
	db.users[4] = new User("Jerry Smith", "asdfggggg");

	for (int i = 0; i < db.user_count; i++)
		std::cout << *db.users[i];

	// Generate Projects
	std::cout << "\nCreating Projects...\n";
	db.project_count = 2;
	db.projects[0] = new Project(
		"FMEI School Website",
		"Source code for FMEI school's official website.",
		USER_PERM_VIEWER
	);
	db.projects[0]->setUserPerm(nullptr, db.users[0], USER_PERM_OWNER);
	db.projects[0]->setUserPerm(db.users[0], db.users[1], USER_PERM_REVIEWER);
	db.projects[0]->setUserPerm(db.users[0], db.users[2], USER_PERM_MAINTAINER);

	db.projects[1] = new Project(
		"FMEI Student Forums",
		"Non-official FMEI forums, guides and info. Maintained by students, for students.",
		USER_PERM_MAINTAINER
	);
	db.projects[1]->setUserPerm(nullptr, db.users[3], USER_PERM_OWNER);
	db.projects[1]->setUserPerm(db.users[4], db.users[1], USER_PERM_REVIEWER);

	for (int i = 0; i < db.project_count; i++)
		std::cout << *db.projects[i];

	// Generate Tickets
	std::cout << "\nCreating Tickets...\n";
	db.ticket_count = 4;
	db.tickets[0] = new Ticket(db.users[0], db.projects[0], false, "Wrong titlebar color", "On devices running iOS 7 and up, the title bar doesn't show the correct color.");
	db.tickets[1] = new Ticket(db.users[1], db.projects[0], false, "Database may be insecure", "We're storing all of the students' passwords in plain-text? Really? This needs to be addressed ASAP!");
	db.tickets[2] = new Ticket(db.users[3], db.projects[1], false, "Missing class courses", "We haven't updated our course lists to include the newly-added Cybersec course.");
	db.tickets[3] = new Ticket(db.users[4], db.projects[1], false, "No dark mode", "I'm gonna have to fix this soon, it hurts to look at.");

	for (int i = 0; i < db.ticket_count; i++)
		std::cout << *db.tickets[i];

	// Generate Pull Requests
	std::cout << "\nCreating Pull Requests...\n";
	db.pr_count = 3;
	db.prs[0] = new PullRequest(db.users[2], db.projects[0], "Fix titlebar color issue", "Updated titlebar.css follow iOS 7 website specifications.");
	db.prs[0]->setChangedFileContent(db.users[2], "titlebar.css", "ios_color_tag = black");
	db.prs[1] = new PullRequest(db.users[1], db.projects[1], "Add 'secure' field to database", "Surely this fixes all of our problems!");
	db.prs[1]->setChangedFileContent(db.users[1], "database.oxl", "is_secure = true");
	db.prs[2] = new PullRequest(db.users[4], db.projects[1], "Add dark mode", "Needs more testing");
	db.prs[2]->setChangedFileContent(db.users[4], "settings.cfg", "dark_mode = false");
	db.prs[2]->setChangedFileContent(db.users[4], "home.html", "if (dark_mode)\n\tmake_dark();");

	for (int i = 0; i < db.pr_count; i++)
		std::cout << *db.prs[i];

	// Review Pull Requests
	std::cout << "\nReviewing pull requests...\n";

	db.prs[0]->setState(db.users[0], PR_STATE_MERGED, "Looks good to me!");
	db.tickets[0]->setClosed(db.users[0], true);

	db.prs[1]->setState(db.users[0], PR_STATE_BLOCKED, "This is spam. Will delete later.");
	db.prs[1]->setTitle(db.users[1], ":(");

	db.prs[2]->setState(db.users[2], PR_STATE_NEEDS_CHANGES, "Missing a dark mode entry in 'gallery.html'");
	db.prs[2]->setChangedFileContent(db.users[4], "gallery.html", "if (dark_mode)\n\tmake_dark();");
	db.prs[2]->setState(db.users[2], PR_STATE_MERGED, "Nice, that was fast!");
	db.tickets[3]->setClosed(db.users[2], true);

	for (int i = 0; i < db.pr_count; i++)
		std::cout << *db.prs[i];

	// Finish
	std::cout << "\nFinished generating sample data.\nEnter 0 to go to the Interactive CLI, or 1 to exit: #";
	int x; std::cin >> x;

	if (x == 0) {
		User* auth = nullptr;
		cli_home(&db, auth);
	}

	// Cleanup
	for (int i = 0; i < db.pr_count; i++)
		delete db.prs[i];

	for (int i = 0; i < db.ticket_count; i++)
		delete db.tickets[i];

	for (int i = 0; i < db.project_count; i++)
		delete db.projects[i];

	for (int i = 0; i < db.user_count; i++)
		delete db.users[i];

	return 0;
}

/// Helpers

// Sterge totul din consola si reseteaza starea cin-ului
void cli_clear() {
	system("CLS");
	std::cin.clear();
}

// Asteapta input de la utilizator inainte sa continue
void cli_await() {
	int x; std::cout << "\nEnter any key to continue.\n"; std::cin >> x;
}

// Afiseaza un meniu cu optiunile date si returneaza optiunea aleasa de utilizator
int cli_menu(const int option_count, const char* options[])
{
	std::cout << "Menus:\n";

	for (int i = 0; i < option_count; i++)
		std::cout << "\t" << i + 1 << ". " << options[i] << "\n";

	std::cout << "\nOption: #";
	int option = 0;

	while (option < 1 || option > option_count) {
		std::cin >> option;
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}

	return option;
}

// Afiseaza optiunile principale, diferite daca utilizatorul e autentificat sau nu
void cli_home(Database* db, User*& auth)
{
	cli_clear();

	if (auth == nullptr)
	{
		const char* options[] = { "Log in", "Sign up", "View Users", "View Projects", "Exit" };
		int op = cli_menu(5, options);

		switch (op) {
		case 1: cli_user_login(db, auth); break;
		case 2: cli_user_signup(db, auth); break;
		case 3: cli_user_view_vector(db, auth, db->user_count, db->users); break;
		case 4: cli_project_view_vector(db, auth, db->project_count, db->projects); break;
		case 5: return;
		}
	}
	else {
		const char* options[] = {
			"View My Profile",
			"View My Projects",
			"View My Tickets",
			"View My Pull Requests",
			"View All Users",
			"View All Projects",
			"Create New Project",
			"Log out",
			"Exit",
		};

		int op = cli_menu(9, options);

		switch (op) {
		case 1: cli_user_view(db, auth, auth); break;
		case 2: cli_project_view_in_user(db, auth, auth); break;
		case 3: cli_ticket_view_in_user(db, auth, auth); break;
		case 4: cli_pr_view_in_user(db, auth, auth); break;
		case 5: cli_user_view_vector(db, auth, db->user_count, db->users); break;
		case 6: cli_project_view_vector(db, auth, db->project_count, db->projects); break;
		case 7: cli_project_create(db, auth); break;
		case 8: cli_user_logout(db, auth); break;
		case 9: return;
		}
	}
}

/// User
// Afiseaza profilul unui utilizator, cu optiuni diferite daca e propriul profil sau al altcuiva
void cli_user_view(Database* db, User*& auth, User* user) {
	cli_clear();
	std::cout << "Viewing User...\nUsername: " << user->getUsername() << '\n';

	if (auth == user) {
		const char* options[] = { "Update User Details", "View User's Projects", "View User's Tickets", "View User's Pull Requests", "Log Out", "Home" };
		int op = cli_menu(6, options);

		switch (op) {
		case 1: cli_user_update(db, auth, user); break;
		case 2: cli_project_view_in_user(db, auth, user); break;
		case 3: cli_ticket_view_in_user(db, auth, user); break;
		case 4: cli_pr_view_in_user(db, auth, user); break;
		case 5: cli_user_logout(db, auth); break;
		case 6: cli_home(db, auth); break;
		}
	}
	else {
		const char* options[] = { "View User's Projects", "View User's Tickets", "View User's Pull Requests", "Force Log In", "Home" };
		int op = cli_menu(5, options);

		switch (op) {
		case 1: cli_project_view_in_user(db, auth, user); break;
		case 2: cli_ticket_view_in_user(db, auth, user); break;
		case 3: cli_pr_view_in_user(db, auth, user); break;
		case 4:
			auth = user;
			std::cout << "You are now logged in as user '" << user->getUsername() << "'.\n";
			cli_await();
			cli_home(db, auth);
			break;
		case 5: cli_home(db, auth); break;
		}
	}
}

// Afiseaza o lista de utilizatori si permite selectarea unuia pentru vizualizare
void cli_user_view_vector(Database* db, User*& auth, const int count, User* v[]) {
	cli_clear();
	std::cout << "Users:\n";

	for (int i = 0; i < count; i++)
		std::cout << '\t' << i + 1 << ". " << v[i]->getUsername() << '\n';

	std::cout << "\nSelect a user to view (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= count)
		cli_user_view(db, auth, v[idx - 1]);
	else
		cli_home(db, auth);
}

// Afiseaza lista de utilizatori cu permisiuni explicite intr-un proiect
void cli_user_view_in_project(Database* db, User*& auth, const Project* project) {
	int count = 0;
	User* v[FIXED_ARRAY_SIZE] = {};

	for (int i = 0; i < db->user_count; i++)
		if (project->getUserPerm(db->users[i]) != project->getDefaultUserPerm()) {
			v[count] = db->users[i];
			count++;
		}

	cli_user_view_vector(db, auth, count, v);
}

// Permite utilizatorului autentificat sa-si schimbe username-ul si parola
void cli_user_update(Database* db, User*& auth, User* user) {
	cli_clear();

	std::cout << "Updating User...\nNew Username: ";
	char username[FIXED_STRING_SIZE];

	std::cin.getline(username, FIXED_STRING_SIZE);
	user->setUsername(auth, username);

	std::cout << "New Password: ";
	char password[FIXED_STRING_SIZE];
	std::cin.getline(password, FIXED_STRING_SIZE);
	user->setPassword(auth, password);

	cli_home(db, auth);
}

// Autentifica un utilizator existent pe baza de username si parola
void cli_user_login(Database* db, User*& auth) {
	cli_clear();

	std::cout << "Logging in...\nUsername: ";
	char username[FIXED_STRING_SIZE];

	std::cin.getline(username, FIXED_STRING_SIZE);

	User* target = nullptr;

	for (int i = 0; i < db->user_count; i++)
		if (strncmp(db->users[i]->getUsername(), username, FIXED_STRING_SIZE) == 0)
			target = db->users[i];

	if (target == nullptr) {
		std::cout << "\nNo user with this name exists!\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Password: ";
	char password[FIXED_STRING_SIZE];
	std::cin.getline(password, FIXED_STRING_SIZE);

	if (!target->isPasswordCorrect(password)) {
		std::cout << "\nIncorrect password!\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	auth = target;
	std::cout << "You are now logged in as user '" << auth->getUsername() << "'.\n";

	cli_await();
	cli_home(db, auth);
}

// Creeaza un cont nou si autentifica utilizatorul imediat
void cli_user_signup(Database* db, User*& auth) {
	cli_clear();

	if (db->user_count >= FIXED_ARRAY_SIZE - 1) {
		std::cout << "Can't create any new users.\nReached fixed limit of 100.\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Signing up...\nUsername: ";
	char username[FIXED_STRING_SIZE];

	std::cin.getline(username, FIXED_STRING_SIZE);

	std::cout << "Password: ";
	char password[FIXED_STRING_SIZE];
	std::cin.getline(password, FIXED_STRING_SIZE);

	auth = new User(username, password);
	db->users[db->user_count] = auth;
	db->user_count++;

	std::cout << "You are now logged in as user '" << auth->getUsername() << "'.\n";

	cli_await();
	cli_home(db, auth);
}

// Deautentifica utilizatorul curent
void cli_user_logout(Database* db, User*& auth) {
	cli_clear();
	std::cout << "You are now logged out.";
	auth = nullptr;
	cli_await();
	cli_home(db, auth);
}

/// Project
// Afiseaza detaliile unui proiect, cu optiuni in functie de permisiunile utilizatorului
void cli_project_view(Database* db, User*& auth, Project* project) {
	cli_clear();
	std::cout << "Viewing Project...\nTitle: " << project->getTitle(auth) << "\nDescription: " << project->getDescription(auth) << "\nFiles: \n";

	for (int i = 0; i < project->getFileCount(auth); i++)
		std::cout << project->getFiles(auth)[i].path << " : " << project->getFiles(auth)[i].content << "\n";

	if (project->getUserPerm(auth) >= USER_PERM_OWNER && auth != nullptr) {
		const char* options[] = {
			"Update Project Details",
			"View Project's Users",
			"Update Project's Users",
			"Update Default Perms",
			"View Project's Tickets",
			"View Project's Pull Requests",
			"Create New Ticket",
			"Create New Pull Request",
			"Home"
		};

		int op = cli_menu(9, options);

		switch (op) {
		case 1: cli_project_update(db, auth, project); break;
		case 2: cli_user_view_in_project(db, auth, project); break;
		case 3: cli_project_modify_users(db, auth, project); break;
		case 4: cli_project_modify_user_default(db, auth, project); break;
		case 5: cli_ticket_view_in_project(db, auth, project); break;
		case 6: cli_pr_view_in_project(db, auth, project); break;
		case 7: cli_ticket_create(db, auth, project); break;
		case 8: cli_pr_create(db, auth, project); break;
		case 9: cli_home(db, auth); break;
		}
	}
	else if (project->getUserPerm(auth) >= USER_PERM_MAINTAINER && auth != nullptr) {
		const char* options[] = {
			"View Project's Users",
			"View Project's Tickets",
			"View Project's Pull Requests",
			"Create New Ticket",
			"Create New Pull Request",
			"Home"
		};

		int op = cli_menu(6, options);

		switch (op) {
		case 1: cli_user_view_in_project(db, auth, project); break;
		case 2: cli_ticket_view_in_project(db, auth, project); break;
		case 3: cli_pr_view_in_project(db, auth, project); break;
		case 4: cli_ticket_create(db, auth, project); break;
		case 5: cli_pr_create(db, auth, project); break;
		case 6: cli_home(db, auth); break;
		}
	}
	else if (project->getUserPerm(auth) >= USER_PERM_VIEWER) {
		const char* options[] = {
			"View Project's Users",
			"View Project's Tickets",
			"View Project's Pull Requests",
			"Home"
		};

		int op = cli_menu(4, options);

		switch (op) {
		case 1: cli_user_view_in_project(db, auth, project); break;
		case 2: cli_ticket_view_in_project(db, auth, project); break;
		case 3: cli_pr_view_in_project(db, auth, project); break;
		case 4: cli_home(db, auth); break;
		}
	}
	else {
		cli_await();
		cli_home(db, auth);
	}
}

// Afiseaza o lista de proiecte si permite selectarea unuia pentru vizualizare
void cli_project_view_vector(Database* db, User*& auth, const int count, Project* v[]) {
	cli_clear();
	std::cout << "Projects:\n";

	for (int i = 0; i < count; i++)
		std::cout << '\t' << i + 1 << ". " << v[i]->getTitle(auth) << '\n';

	std::cout << "\nSelect a project to view (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= count)
		cli_project_view(db, auth, v[idx - 1]);
	else
		cli_home(db, auth);
}

// Afiseaza proiectele detinute de un anumit utilizator
void cli_project_view_in_user(Database* db, User*& auth, const User* user) {
	int count = 0;
	Project* v[FIXED_ARRAY_SIZE];

	for (int i = 0; i < db->project_count; i++)
		if (db->projects[i]->getUserPerm(user) == USER_PERM_OWNER) {
			v[count] = db->projects[i];
			count++;
		}

	cli_project_view_vector(db, auth, count, v);
}

// Creeaza un proiect nou cu utilizatorul autentificat ca owner
void cli_project_create(Database* db, User*& auth) {
	cli_clear();

	if (db->project_count >= FIXED_ARRAY_SIZE - 1) {
		std::cout << "Can't create any new projects.\nReached fixed limit of 100.\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Creating New Project...\nProject Title: ";
	char title[FIXED_STRING_SIZE];

	std::cin.getline(title, FIXED_STRING_SIZE);

	std::cout << "Project Description: ";
	char description[FIXED_STRING_SIZE];
	std::cin.getline(description, FIXED_STRING_SIZE);

	Project* p = new Project(title, description, USER_PERM_VIEWER);
	p->setUserPerm(nullptr, auth, USER_PERM_OWNER);

	db->projects[db->project_count] = p;
	db->project_count++;

	std::cout << "New project has been created.";

	cli_await();
	cli_home(db, auth);
}

// Modifica titlul si descrierea unui proiect (necesita permisiune OWNER)
void cli_project_update(Database* db, User*& auth, Project* project) {
	cli_clear();

	std::cout << "Modifying Project Details...\nTitle: ";
	char title[FIXED_STRING_SIZE];

	std::cin.getline(title, FIXED_STRING_SIZE);
	project->setTitle(auth, title);

	std::cout << "Description: ";
	char description[FIXED_STRING_SIZE];
	std::cin.getline(description, FIXED_STRING_SIZE);
	project->setDescription(auth, description);

	std::cout << "Project details updated.\n";

	cli_await();
	cli_home(db, auth);
}

// Modifica permisiunea unui utilizator specific intr-un proiect (necesita OWNER)
void cli_project_modify_users(Database* db, User*& auth, Project* project) {
	cli_clear();

	std::cout << "Modifying Project User Perms...\nUsername: ";
	char username[FIXED_STRING_SIZE];

	std::cin.getline(username, FIXED_STRING_SIZE);

	User* user = nullptr;

	for (int i = 0; i < db->user_count; i++) {
		if (strncmp(db->users[i]->getUsername(), username, FIXED_STRING_SIZE) == 0) {
			user = db->users[i];
			break;
		}
	}

	std::cout << "Permissions: (NONE = 0, VIEW = 1, MAINTAIN = 2, REVIEW = 3, ADMINISTRATE = 4): #";
	int user_perm; std::cin >> user_perm;

	project->setUserPerm(auth, user, user_perm);
	std::cout << "Project user permissions updated.\n";

	cli_await();
	cli_home(db, auth);
}

// Modifica permisiunea implicita pentru utilizatori noi in proiect (necesita OWNER)
void cli_project_modify_user_default(Database* db, User*& auth, Project* project) {
	cli_clear();

	std::cout << "Modifying Project User Default Permission...\nPermissions: (NONE = 0, VIEW = 1, MAINTAIN = 2, REVIEW = 3, ADMINISTRATE = 4): #";
	int user_perm; std::cin >> user_perm;

	project->setDefaultUserPerm(auth, user_perm);
	std::cout << "Project user permissions updated.\n";

	cli_await();
	cli_home(db, auth);
}

/// Ticket
// Afiseaza detaliile unui ticket, cu optiuni in functie de permisiuni
void cli_ticket_view(Database* db, User*& auth, Ticket* ticket) {
	cli_clear();
	std::cout << "Viewing Ticket...\nTitle: " << ticket->getTitle(auth) << "\nDescription: " << ticket->getDescription(auth) << "\nClosed: " << (ticket->getClosed() ? "Yes" : "No") << "\n";

	if (auth == ticket->getOwner()) {
		const char* options[] = { "Update Ticket Details", "Update Ticket State", "View Ticket Owner", "View Ticket Project", "Home" };
		int op = cli_menu(5, options);

		switch (op) {
		case 1: cli_ticket_update(db, auth, ticket); break;
		case 2: cli_ticket_closed_update(db, auth, ticket); break;
		case 3: cli_user_view(db, auth, ticket->getOwner()); break;
		case 4: cli_project_view(db, auth, ticket->getProject()); break;
		case 5: cli_home(db, auth); break;
		}
	}
	else if (auth != nullptr && ticket->getProject()->getUserPerm(auth) >= USER_PERM_REVIEWER) {
		const char* options[] = { "Update Ticket State", "View Ticket Owner", "View Ticket Project", "Go Home" };
		int op = cli_menu(4, options);

		switch (op) {
		case 1: cli_ticket_closed_update(db, auth, ticket); break;
		case 2: cli_user_view(db, auth, ticket->getOwner()); break;
		case 3: cli_project_view(db, auth, ticket->getProject()); break;
		case 4: cli_home(db, auth); break;
		}
	}
	else {
		const char* options[] = { "View Ticket Owner", "View Ticket Project", "Home" };
		int op = cli_menu(3, options);

		switch (op) {
		case 1: cli_user_view(db, auth, ticket->getOwner()); break;
		case 2: cli_project_view(db, auth, ticket->getProject()); break;
		case 3: cli_home(db, auth); break;
		}
	}
}

// Afiseaza o lista de tickete si permite selectarea unuia pentru vizualizare
void cli_ticket_view_vector(Database* db, User*& auth, const int count, Ticket* tickets[]) {
	cli_clear();
	std::cout << "Tickets:\n";

	for (int i = 0; i < count; i++)
		std::cout << '\t' << i + 1 << ". " << tickets[i]->getTitle(auth) << '\n';

	std::cout << "\nSelect a ticket to view (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= count)
		cli_ticket_view(db, auth, tickets[idx - 1]);
	else
		cli_home(db, auth);
}

// Afiseaza ticketele create de un anumit utilizator
void cli_ticket_view_in_user(Database* db, User*& auth, const User* user) {
	int count = 0;
	Ticket* v[FIXED_ARRAY_SIZE] = {};

	for (int i = 0; i < db->ticket_count; i++)
		if (db->tickets[i]->getOwner() == user) {
			v[count] = db->tickets[i];
			count++;
		}

	cli_ticket_view_vector(db, auth, count, v);
}

// Afiseaza ticketele dintr-un anumit proiect
void cli_ticket_view_in_project(Database* db, User*& auth, const Project* project) {
	int count = 0;
	Ticket* v[FIXED_ARRAY_SIZE] = {};

	for (int i = 0; i < db->ticket_count; i++)
		if (db->tickets[i]->getProject() == project) {
			v[count] = db->tickets[i];
			count++;
		}

	cli_ticket_view_vector(db, auth, count, v);
}

// Creeaza un ticket nou intr-un proiect (necesita minim MAINTAINER)
void cli_ticket_create(Database* db, User*& auth, Project* project) {
	cli_clear();

	if (db->ticket_count >= FIXED_ARRAY_SIZE - 1) {
		std::cout << "Can't create any new tickets.\nReached fixed limit of 100.\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Creating Ticket...\nTicket Title: ";
	char title[FIXED_STRING_SIZE];

	std::cin.getline(title, FIXED_STRING_SIZE);

	std::cout << "Ticket Description: ";
	char description[FIXED_STRING_SIZE];
	std::cin.getline(description, FIXED_STRING_SIZE);

	Ticket* t = new Ticket(auth, project, false, title, description);
	db->tickets[db->ticket_count] = t;
	db->ticket_count++;

	std::cout << "New ticket has been created.";

	cli_await();
	cli_home(db, auth);
}

// Modifica titlul si descrierea unui ticket (doar owner-ul ticketului)
void cli_ticket_update(Database* db, User*& auth, Ticket* ticket) {
	cli_clear();

	std::cout << "Updating Ticket Details...\nTitle: ";
	char title[FIXED_STRING_SIZE];

	std::cin.getline(title, FIXED_STRING_SIZE);
	ticket->setTitle(auth, title);

	std::cout << "Description: ";
	char description[FIXED_STRING_SIZE];
	std::cin.getline(description, FIXED_STRING_SIZE);
	ticket->setDescription(auth, description);

	std::cout << "Ticket details updated.\n";

	cli_await();
	cli_home(db, auth);
}

// Schimba starea unui ticket intre deschis si inchis
void cli_ticket_closed_update(Database* db, User*& auth, Ticket* ticket) {
	cli_clear();

	std::cout << "Updating Ticket State...\nState (0 for Open, 1 for Closed): ";
	int state; std::cin >> state;
	ticket->setClosed(auth, state >= 1);

	std::cout << "Ticket state updated.\n";

	cli_await();
	cli_home(db, auth);
}

/// PR
// Afiseaza detaliile unui pull request, cu optiuni in functie de permisiuni
void cli_pr_view(Database* db, User*& auth, PullRequest* pr) {
	cli_clear();
	std::cout << "Viewing PR...\nTitle: " << pr->getTitle(auth) << "\nDescription: " << pr->getDescription(auth) << "\nState: ";

	switch (pr->getState()) {
	case 0: std::cout << "Draft\n"; break;
	case 1: std::cout << "Awaiting Review\n"; break;
	case 2: std::cout << "Needs Changes\n"; break;
	case 3: std::cout << "Merged\n"; break;
	case 4: std::cout << "Blocked\n"; break;
	}

	std::cout << "Changed Files:\n";

	for (int i = 0; i < pr->getChangedFileCount(auth); i++)
		std::cout << pr->getChangedFiles(auth)[i].path << " : " << pr->getChangedFiles(auth)[i].content << "\n";

	if (auth == pr->getOwner()) {
		const char* options[] = { "Update PR Details", "Update PR State", "Update PR Files", "View PR Owner", "View PR Project", "Home" };
		int op = cli_menu(6, options);

		switch (op) {
		case 1: cli_pr_update(db, auth, pr); break;
		case 2: cli_pr_state_update(db, auth, pr); break;
		case 3: cli_pr_file_update(db, auth, pr); break;
		case 4: cli_user_view(db, auth, pr->getOwner()); break;
		case 5: cli_project_view(db, auth, pr->getProject()); break;
		case 6: cli_home(db, auth); break;
		}
	}
	else if (auth != nullptr && pr->getProject()->getUserPerm(auth) >= USER_PERM_REVIEWER) {
		const char* options[] = { "Update PR State", "View PR Owner", "View PR Project", "Go Home" };
		int op = cli_menu(4, options);

		switch (op) {
		case 1: cli_pr_state_update(db, auth, pr); break;
		case 2: cli_user_view(db, auth, pr->getOwner()); break;
		case 3: cli_project_view(db, auth, pr->getProject()); break;
		case 4: cli_home(db, auth); break;
		}
	}
	else {
		const char* options[] = { "View PR Owner", "View PR Project", "Home" };
		int op = cli_menu(3, options);

		switch (op) {
		case 1: cli_user_view(db, auth, pr->getOwner()); break;
		case 2: cli_project_view(db, auth, pr->getProject()); break;
		case 3: cli_home(db, auth); break;
		}
	}
}

// Afiseaza o lista de pull request-uri si permite selectarea unuia pentru vizualizare
void cli_pr_view_vector(Database* db, User*& auth, const int count, PullRequest* v[]) {
	cli_clear();
	std::cout << "Pull Requests:\n";

	for (int i = 0; i < count; i++)
		std::cout << '\t' << i + 1 << ". " << v[i]->getTitle(auth) << '\n';

	std::cout << "\nSelect a pull request to view (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= count)
		cli_pr_view(db, auth, v[idx - 1]);
	else
		cli_home(db, auth);
}

// Afiseaza pull request-urile create de un anumit utilizator
void cli_pr_view_in_user(Database* db, User*& auth, const User* user) {
	int count = 0;
	PullRequest* v[FIXED_ARRAY_SIZE] = {};

	for (int i = 0; i < db->pr_count; i++)
		if (db->prs[i]->getOwner() == user) {
			v[count] = db->prs[i];
			count++;
		}

	cli_pr_view_vector(db, auth, count, v);
}

// Afiseaza pull request-urile dintr-un anumit proiect
void cli_pr_view_in_project(Database* db, User*& auth, const Project* project) {
	int count = 0;
	PullRequest* v[FIXED_ARRAY_SIZE] = {};

	for (int i = 0; i < db->pr_count; i++)
		if (db->prs[i]->getProject() == project) {
			v[count] = db->prs[i];
			count++;
		}

	cli_pr_view_vector(db, auth, count, v);
}

// Creeaza un pull request nou intr-un proiect (necesita minim MAINTAINER)
void cli_pr_create(Database* db, User*& auth, Project* project) {
	cli_clear();

	if (db->pr_count >= FIXED_ARRAY_SIZE - 1) {
		std::cout << "Can't create any new PRs.\nReached fixed limit of 100.\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Creating PR...\nPR Title: ";
	char title[FIXED_STRING_SIZE];

	std::cin.getline(title, FIXED_STRING_SIZE);

	std::cout << "PR Description: ";
	char description[FIXED_STRING_SIZE];
	std::cin.getline(description, FIXED_STRING_SIZE);

	PullRequest* pr = new PullRequest(auth, project, title, description);
	db->prs[db->pr_count] = pr;
	db->pr_count++;

	std::cout << "New Pull Request has been created.";

	cli_await();
	cli_home(db, auth);
}

// Modifica titlul si descrierea unui pull request (doar owner-ul PR-ului)
void cli_pr_update(Database* db, User*& auth, PullRequest* pr) {
	cli_clear();

	std::cout << "Updating PR Details...\nTitle: ";
	char title[FIXED_STRING_SIZE];

	std::cin.getline(title, FIXED_STRING_SIZE);
	pr->setTitle(auth, title);

	std::cout << "Description: ";
	char description[FIXED_STRING_SIZE];
	std::cin.getline(description, FIXED_STRING_SIZE);
	pr->setDescription(auth, description);

	std::cout << "Pull Request details updated.\n";

	cli_await();
	cli_home(db, auth);
}

// Schimba starea unui pull request; la MERGED aplica fisierele modificate pe proiect
void cli_pr_state_update(Database* db, User*& auth, PullRequest* pr) {
	cli_clear();
	std::cout << "Updating PR State...\nState (";

	if (pr->getOwner() == auth && pr->getProject()->getUserPerm(auth) >= USER_PERM_REVIEWER)
		std::cout << "0 for DRAFT, 1 for AWAITING REVIEW, 2 for NEEDS CHANGES, 3 for MERGED, 4 for BLOCKED): ";
	else if (pr->getProject()->getUserPerm(auth) >= USER_PERM_REVIEWER)
		std::cout << "2 for NEEDS CHANGES, 3 for MERGED, 4 for BLOCKED): ";
	else if (pr->getOwner() == auth)
		std::cout << "0 for DRAFT, 1 for AWAITING REVIEW): ";

	int state; std::cin >> state;

	std::cout << "Review Comment: ";
	char review_comment[FIXED_STRING_SIZE];

	std::cin.getline(review_comment, FIXED_STRING_SIZE);

	pr->setState(auth, state, review_comment);

	std::cout << "PR state updated.\n";

	cli_await();
	cli_home(db, auth);
}

// Adauga sau modifica un fisier in lista de fisiere schimbate ale unui pull request
void cli_pr_file_update(Database* db, User*& auth, PullRequest* pr) {
	cli_clear();

	std::cout << "Updating PR File...\nFile Name: ";
	char file_name[FIXED_STRING_SIZE];

	std::cin.getline(file_name, FIXED_STRING_SIZE);

	std::cout << "File Content: ";
	char file_content[FIXED_STRING_SIZE];
	std::cin.getline(file_content, FIXED_STRING_SIZE);

	pr->setChangedFileContent(auth, file_name, file_content);
	std::cout << "Pull Request file updated.\n";

	cli_await();
	cli_home(db, auth);
}