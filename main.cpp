#include <iostream>
#include <stdlib.h>
#include "Project.h"
#include "PullRequest.h"
#include "Ticket.h"
#include "User.h"

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

// Forward Declarations
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

	User* auth = nullptr;

	if (x == 0)
		cli_home(&db, auth);

	return 0;
}

// Helpers
void cli_clear() {
	system("CLS");
	std::cin.clear();
}

void cli_await() {
	int x; std::cout << "\nEnter any key to continue.\n"; std::cin >> x;
}

int cli_menu(const int option_count, const std::string options[])
{
	std::cout << "Menus:\n";

	for (int i = 0; i < option_count; i++)
		std::cout << "\t" << i + 1 << ". " << options[i] << "\n";

	std::cout << "\nOption: #";
	int option = 0;

	while (option < 1 || option > option_count) {
		std::cin >> option;
		std::cin.clear();
		std::cin.ignore();
	}

	return option;
}

void cli_home(Database* db, User*& auth)
{
	cli_clear();

	if (auth == nullptr)
	{
		std::string options[] = { "Log in", "Sign up", "View Users", "View Projects", "Exit" };
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
		std::string options[] = {
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

// User
void cli_user_view(Database* db, User*& auth, User* user) {
	cli_clear();
	std::cout << "Viewing User...\nUsername: " << user->getUsername() << '\n';

	if (auth == user) {
		std::string options[] = { "Update User Details", "View User's Projects", "View User's Tickets", "View User's Pull Requests", "Log Out", "Home" };
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
		std::string options[] = { "View User's Projects", "View User's Tickets", "View User's Pull Requests", "Force Log In", "Home" };
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

void cli_user_update(Database* db, User*& auth, User* user) {
	cli_clear();

	std::cout << "Creating User...\nNew Username: ";
	std::string username; std::cin >> username;
	user->setUsername(auth, username);

	std::cout << "New Password: ";
	std::string password; std::cin >> password;
	user->setPassword(auth, password);

	cli_home(db, auth);
}

void cli_user_login(Database* db, User*& auth) {
	cli_clear();

	std::cout << "Logging in...\nUsername: ";
	std::string username; std::cin >> username;

	User* target = nullptr;

	for (int i = 0; i < db->user_count; i++)
		if (db->users[i]->getUsername() == username)
			target = db->users[i];

	if (target == nullptr) {
		std::cout << "\nNo user with this name exists!\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Password: ";
	std::string password; std::cin >> password;

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

void cli_user_signup(Database* db, User*& auth) {
	cli_clear();

	if (db->user_count >= FIXED_ARRAY_SIZE - 1) {
		std::cout << "Can't create any new users.\nReached fixed limit of 100.\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Signing up...\nUsername: ";
	std::string username; std::cin >> username;
	std::cout << "Password: ";
	std::string password; std::cin >> password;

	auth = new User(username, password);
	db->users[db->user_count] = auth;
	db->user_count++;

	std::cout << "You are now logged in as user '" << auth->getUsername() << "'.\n";

	cli_await();
	cli_home(db, auth);
}

void cli_user_logout(Database* db, User*& auth) {
	cli_clear();
	std::cout << "You are now logged out.";
	auth = nullptr;
	cli_await();
	cli_home(db, auth);
}

// Project
void cli_project_view(Database* db, User*& auth, Project* project) {
	cli_clear();
	std::cout << "Viewing Project...\nTitle: " << project->getTitle(auth) << "\nDescription: " << project->getDescription(auth) << "\nFiles: \n";

	for (int i = 0; i < project->getFileCount(auth); i++)
		std::cout << project->getFiles(auth)[i].path << " : " << project->getFiles(auth)[i].content << "\n";

	if (project->getUserPerm(auth) >= USER_PERM_OWNER && auth != nullptr) {
		std::string options[] = {
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
		std::string options[] = {
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
		std::string options[] = {
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

void cli_project_create(Database* db, User*& auth) {
	cli_clear();

	if (db->project_count >= FIXED_ARRAY_SIZE - 1) {
		std::cout << "Can't create any new projects.\nReached fixed limit of 100.\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Creating New Project...\nProject Title: ";
	std::string title; std::cin >> title;
	std::cout << "Project Description: ";
	std::string description; std::cin >> description;

	Project* p = new Project(title, description, USER_PERM_VIEWER);
	p->setUserPerm(nullptr, auth, USER_PERM_OWNER);

	db->projects[db->project_count] = p;
	db->project_count++;

	std::cout << "New project has been created.";

	cli_await();
	cli_home(db, auth);
}

void cli_project_update(Database* db, User*& auth, Project* project) {
	cli_clear();

	std::cout << "Modifying Project Details...\nTitle: ";
	std::string title; std::cin >> title;
	project->setTitle(auth, title);

	std::cout << "Description: ";
	std::string description; std::cin >> description;
	project->setDescription(auth, description);

	std::cout << "Project details updated.\n";

	cli_await();
	cli_home(db, auth);
}

void cli_project_modify_users(Database* db, User*& auth, Project* project) {
	cli_clear();

	std::cout << "Modifying Project User Perms...\nUsername: ";
	std::string username; std::cin >> username;

	User* user = nullptr;

	for (int i = 0; i < db->user_count; i++) {
		if (db->users[i]->getUsername() == username) {
			user = db->users[i];
			break;
		}
	}

	std::cout << "Permissions: (NONE = 0, VIEW = 1, MAINTAIN = 2, REVIEW = 3, ADMINISTRATE = 4)";
	int user_perm; std::cin >> user_perm;

	project->setUserPerm(auth, user, user_perm);
	std::cout << "Project user permissions updated.\n";

	cli_await();
	cli_home(db, auth);
}

void cli_project_modify_user_default(Database* db, User*& auth, Project* project) {
	cli_clear();

	std::cout << "Modifying Project User Default Permission...\nPermissions: (NONE = 0, VIEW = 1, MAINTAIN = 2, REVIEW = 3, ADMINISTRATE = 4): #";
	int user_perm; std::cin >> user_perm;

	project->setDefaultUserPerm(auth, user_perm);
	std::cout << "Project user permissions updated.\n";

	cli_await();
	cli_home(db, auth);
}

// Ticket
void cli_ticket_view(Database* db, User*& auth, Ticket* ticket) {
	cli_clear();
	std::cout << "Viewing Ticket...\nTitle: " << ticket->getTitle(auth) << "\nDescription: " << ticket->getDescription(auth) << "\nClosed: " << (ticket->getClosed() ? "Yes" : "No") << "\n";

	if (auth == ticket->getOwner()) {
		std::string options[] = { "Update Ticket Details", "Update Ticket State", "View Ticket Owner", "View Ticket Project", "Home" };
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
		std::string options[] = { "Update Ticket State", "View Ticket Owner", "View Ticket Project", "Go Home" };
		int op = cli_menu(4, options);

		switch (op) {
		case 1: cli_ticket_closed_update(db, auth, ticket); break;
		case 2: cli_user_view(db, auth, ticket->getOwner()); break;
		case 3: cli_project_view(db, auth, ticket->getProject()); break;
		case 4: cli_home(db, auth); break;
		}
	}
	else {
		std::string options[] = { "View Ticket Owner", "View Ticket Project", "Home" };
		int op = cli_menu(3, options);

		switch (op) {
		case 1: cli_user_view(db, auth, ticket->getOwner()); break;
		case 2: cli_project_view(db, auth, ticket->getProject()); break;
		case 3: cli_home(db, auth); break;
		}
	}
}

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

void cli_ticket_create(Database* db, User*& auth, Project* project) {
	cli_clear();

	if (db->ticket_count >= FIXED_ARRAY_SIZE - 1) {
		std::cout << "Can't create any new tickets.\nReached fixed limit of 100.\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Creating Ticket...\nTicket Title: ";
	std::string title; std::cin >> title;
	std::cout << "Ticket Description: ";
	std::string description; std::cin >> description;

	Ticket* t = new Ticket(auth, project, false, title, description);
	db->tickets[db->ticket_count] = t;
	db->ticket_count++;

	std::cout << "New ticket has been created.";

	cli_await();
	cli_home(db, auth);
}

void cli_ticket_update(Database* db, User*& auth, Ticket* ticket) {
	cli_clear();

	std::cout << "Updating Ticket Details...\nTitle: ";
	std::string title; std::cin >> title;
	ticket->setTitle(auth, title);

	std::cout << "Description: ";
	std::string description; std::cin >> description;
	ticket->setDescription(auth, description);

	std::cout << "Ticket details updated.\n";

	cli_await();
	cli_home(db, auth);
}

void cli_ticket_closed_update(Database* db, User*& auth, Ticket* ticket) {
	cli_clear();

	std::cout << "Updating Ticket State...\nState (0 for Open, 1 for Closed): ";
	int state; std::cin >> state;
	ticket->setClosed(auth, state >= 1);

	std::cout << "Ticket state updated.\n";

	cli_await();
	cli_home(db, auth);
}

// PR
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

	std::cout << "Changed Files : \n";

	for (int i = 0; i < pr->getChangedFileCount(auth); i++)
		std::cout << pr->getChangedFiles(auth)[i].path << " : " << pr->getChangedFiles(auth)[i].content << "\n";

	if (auth == pr->getOwner()) {
		std::string options[] = { "Update PR Details", "Update PR State", "Update PR Files", "View PR Owner", "View PR Project", "Home" };
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
		std::string options[] = { "Update PR State", "View PR Owner", "View PR Project", "Go Home" };
		int op = cli_menu(4, options);

		switch (op) {
		case 1: cli_pr_state_update(db, auth, pr); break;
		case 2: cli_user_view(db, auth, pr->getOwner()); break;
		case 3: cli_project_view(db, auth, pr->getProject()); break;
		case 4: cli_home(db, auth); break;
		}
	}
	else {
		std::string options[] = { "View PR Owner", "View PR Project", "Home" };
		int op = cli_menu(3, options);

		switch (op) {
		case 1: cli_user_view(db, auth, pr->getOwner()); break;
		case 2: cli_project_view(db, auth, pr->getProject()); break;
		case 3: cli_home(db, auth); break;
		}
	}
}

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

void cli_pr_create(Database* db, User*& auth, Project* project) {
	cli_clear();

	if (db->pr_count >= FIXED_ARRAY_SIZE - 1) {
		std::cout << "Can't create any new PRs.\nReached fixed limit of 100.\n";
		cli_await();
		cli_home(db, auth);
		return;
	}

	std::cout << "Creating PR...\nPR Title: ";
	std::string title; std::cin >> title;
	std::cout << "PR Description: ";
	std::string description; std::cin >> description;

	PullRequest* pr = new PullRequest(auth, project, title, description);
	db->prs[db->pr_count] = pr;
	db->pr_count++;

	std::cout << "New Pull Request has been created.";

	cli_await();
	cli_home(db, auth);
}

void cli_pr_update(Database* db, User*& auth, PullRequest* pr) {
	cli_clear();

	std::cout << "Updating PR Details...\nTitle: ";
	std::string title; std::cin >> title;
	pr->setTitle(auth, title);

	std::cout << "Description: ";
	std::string description; std::cin >> description;
	pr->setDescription(auth, description);

	std::cout << "Pull Request details updated.\n";

	cli_await();
	cli_home(db, auth);
}

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
	std::string review_comment; std::getline(std::cin, review_comment);

	pr->setState(auth, state, review_comment);

	std::cout << "Ticket state updated.\n";

	cli_await();
	cli_home(db, auth);
}

void cli_pr_file_update(Database* db, User*& auth, PullRequest* pr) {
	cli_clear();

	std::cout << "Updating PR File...\nFile Name: ";
	std::string file_name; std::cin >> file_name;
	std::cout << "File Content: ";
	std::string file_content; std::cin >> file_content;

	pr->setChangedFileContent(auth, file_name, file_content);
	std::cout << "Pull Request details updated.\n";

	cli_await();
	cli_home(db, auth);
}