#include <iostream>
#include <stdlib.h>
#include <vector>
#include "Project.h"
#include "PullRequest.h"
#include "Ticket.h"
#include "User.h"

// Database
std::vector<User*> db_users;
std::vector<Project*> db_projects;
std::vector<Ticket*> db_tickets;
std::vector<PullRequest*> db_prs;

void generate_sample_database_entries()
{
	// Generate Users
	std::cout << "Creating Users...";
	db_users.push_back(new User("Samantha Gray", "password123"));			// school platform manager
	db_users.push_back(new User("Jonathan Roberts", "NO_HACKING_ALLOWED"));	// school platform programmer 1
	db_users.push_back(new User("Julie Rich", "march252006"));				// school platform programmer 2
	db_users.push_back(new User("Andrew Knox", "CM%T_u&90q!A+Ojg*jA"));		// student guide platform manager
	db_users.push_back(new User("Jerry Smith", "asdfggggg"));				// student guide platform programmer

	// Generate Projects
	std::cout << "Creating Projects...\n";
	db_projects.push_back(new Project(
		"FMEI School Website",
		"Source code for FMEI school's official website.",
		USER_PERM_VIEWER,
		{ {db_users[0], USER_PERM_OWNER}, {db_users[1], USER_PERM_REVIEWER}, {db_users[2], USER_PERM_MAINTAINER}}
	));
	db_projects.push_back(new Project(
		"FMEI Student Forums",
		"Non-official FMEI forums, guides and info. Maintained by students, for students.",
		USER_PERM_MAINTAINER,
		{ {db_users[3], USER_PERM_OWNER}, {db_users[4], USER_PERM_REVIEWER}}
	));

	// Generate Tickets
	std::cout << "Creating Tickets...\n";
	db_tickets.push_back(new Ticket(
		db_users[0],
		db_projects[0],
		false,
		"Wrong titlebar color",
		"On devices running iOS 7 and up, the title bar doesn't show the correct color."
	));
	db_tickets.push_back(new Ticket(
		db_users[1],
		db_projects[0],
		false,
		"Database may be insecure", 
		"We're storing all of the students' passwords in plain-text? Really? This needs to be addressed ASAP!"
	));
	db_tickets.push_back(new Ticket(
		db_users[3],
		db_projects[1],
		false,
		"Missing class courses", 
		"We haven't updated our course lists to include the newly-added Cybersec course."
	));
	db_tickets.push_back(new Ticket(
		db_users[4],
		db_projects[1],
		false,
		"No dark mode", 
		"I'm gonna have to fix this soon, it hurts to look at."
	));

	// Generate Pull Requests
	std::cout << "Creating Pull Requests...\n";

	db_prs.push_back(new PullRequest(
		db_users[2], db_projects[0], 
		"Fix titlebar color issue", 
		"Updated titlebar.css follow iOS 7 website specifications.",
		{{"titlebar.css", "ios_color_tag = black"}}
	));

	db_prs.push_back(new PullRequest(
		db_users[1],
		db_projects[1],
		"Add 'secure' field to database", 
		"Surely this fixes all of our problems!",
		{ {"database.oxl", "is_secure = true"} }
	));

	db_prs.push_back(new PullRequest(
		db_users[4],
		db_projects[1],
		"Add dark mode", 
		"Needs more testing",
		{ { "settings.cfg", "dark_mode = false"}, {"home.html", "if (dark_mode)\n\tmake_dark();"} }
	));

	// Review Pull Requests
	std::cout << "Reviewing pull requests...\n";

	db_prs[0]->setState(db_users[0], PR_STATE_MERGED, "Looks good to me!");
	db_tickets[0]->setClosed(db_users[0], true);

	db_prs[1]->setState(db_users[0], PR_STATE_BLOCKED, "This is spam. Will delete later.");
	db_prs[1]->setTitle(db_users[1], ":(");

	db_prs[2]->setState(db_users[2], PR_STATE_NEEDS_CHANGES, "Missing a dark mode entry in 'gallery.html'");
	db_prs[2]->setChangedFileContent(db_users[4], "gallery.html", "if (dark_mode)\n\tmake_dark();");
	db_prs[2]->setState(db_users[2], PR_STATE_MERGED, "Nice, that was fast!");
	db_tickets[3]->setClosed(db_users[2], true);
}

// Forward Declarations
void cli_home();

void cli_user_view(User*);
void cli_user_view_vector(std::vector<User*>);
void cli_user_view_in_project(Project*);
void cli_user_update(User*);
void cli_user_login();
void cli_user_signup();
void cli_user_logout();

void cli_project_view(Project*);
void cli_project_view_vector(std::vector<Project*>);
void cli_project_view_in_user(User*);
void cli_project_create();
void cli_project_update(Project*);
void cli_project_modify_users(Project*);

void cli_ticket_view(Ticket*);
void cli_ticket_view_vector(std::vector<Ticket*>);
void cli_ticket_view_in_user(User*);
void cli_ticket_view_in_project(Project*);
void cli_ticket_create(Project*);
void cli_ticket_update(Ticket*);
void cli_ticket_closed_update(Ticket* ticket);

void cli_pr_view(PullRequest*);
void cli_pr_view_vector(std::vector<PullRequest*>);
void cli_pr_view_in_user(User*);
void cli_pr_view_in_project(Project*);
void cli_pr_create(Project*);
void cli_pr_update(PullRequest*);
void cli_pr_state_update(PullRequest*);
void cli_pr_file_update(PullRequest*);

int main()
{
	generate_sample_database_entries();
	cli_home();
	return 0;
}

// CLI Implementations
User* auth = nullptr;

void cli_clear() {
	system("CLS");
	std::cin.clear();
}

void cli_await() {
	int x; std::cout << "\nEnter any key to continue.\n"; std::cin >> x;
}

int cli_menu(std::vector<std::string> options)
{
	std::cout << "Menus:\n";

	for (int i = 0; i < options.size(); i++) {
		std::cout << "\t" << i + 1 << ". " << options[i] << "\n";
	}

	std::cout << "\nOption: #";
	int option = 0;

	while (option < 1 || option > options.size()) {
		std::cin >> option;
		std::cin.clear();
		std::cin.ignore();
	}

	return option;
}

void cli_home()
{
	cli_clear();

	if (auth == nullptr)
	{
		int op = cli_menu({ "Log in", "Sign up", "View Users", "View Projects" });

		switch (op) {
		case 1: cli_user_login(); break;
		case 2: cli_user_signup(); break;
		case 3: cli_user_view_vector(db_users); break;
		case 4: cli_project_view_vector(db_projects); break;
		}
	}
	else {
		int op = cli_menu({
			"View My Profile",
			"View My Projects",
			"View My Tickets",
			"View My Pull Requests",
			"View All Users",
			"View All Projects",
			"Create New Project",
			"Log out",
			});

		switch (op) {
		case 1: cli_user_view(auth); break;
		case 2: cli_project_view_in_user(auth); break;
		case 3: cli_ticket_view_in_user(auth); break;
		case 4: cli_pr_view_in_user(auth); break;
		case 5: cli_user_view_vector(db_users); break;
		case 6: cli_project_view_vector(db_projects); break;
		case 7: cli_project_create(); break;
		case 8: cli_user_logout(); break;
		}
	}
}

// User
void cli_user_view(User* user) {
	cli_clear();
	std::cout << "Viewing User...\nUsername: " << user->getUsername() << '\n';

	if (auth == user) {
		int op = cli_menu({ "Update User Details", "View User's Projects", "View User's Tickets", "View User's Pull Requests", "Log Out", "Home"});

		switch (op) {
		case 1: cli_user_update(user); break;
		case 2: cli_project_view_in_user(user); break;
		case 3: cli_ticket_view_in_user(user); break;
		case 4: cli_pr_view_in_user(user); break;
		case 5: cli_user_logout(); break;
		case 6: cli_home(); break;
		}
	}
	else {
		int op = cli_menu({ "View User's Projects", "View User's Tickets", "View User's Pull Requests", "Force Log In", "Home"});

		switch (op) {
		case 1: cli_project_view_in_user(user); break;
		case 2: cli_ticket_view_in_user(user); break;
		case 3: cli_pr_view_in_user(user); break;
		case 4: 
			auth = user; 
			std::cout << "You are now logged in as user '" << user->getUsername() << "'.\n";
			cli_await();
			cli_home(); 
			break;
		case 5: cli_home(); break;
		}
	}
}

void cli_user_view_vector(std::vector<User*> users) {
	cli_clear();
	std::cout << "Users:\n";

	for (int i = 0; i < users.size(); i++)
		std::cout << '\t' << i + 1 << ". " << users[i]->getUsername() << '\n';

	std::cout << "\nSelect a user to view (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= users.size())
		cli_user_view(users[idx - 1]);
	else
		cli_home();
}

void cli_user_view_in_project(Project* project) {
	std::vector<User*> users = {};
	for (int i = 0; i < db_users.size(); i++)
		if (project->getUserPerm(db_users[i]) != project->getDefaultUserPerm())
			users.push_back(db_users[i]);

	cli_user_view_vector(users);
}

void cli_user_update(User* user) {
	cli_clear();

	std::cout << "Creating User...\nNew Username: ";
	std::string username; std::cin >> username;

	user->setUsername(auth, username);

	std::cout << "New Password: ";
	std::string password; std::cin >> password;

	user->setPassword(auth, password);

	cli_home();
}

void cli_user_login() {
	cli_clear();
	std::cout << "Users:\n";

	for (int i = 0; i < db_users.size(); i++)
		std::cout << '\t' << i + 1 << ". " << db_users[i]->getUsername() << '\n';

	std::cout << "\nSelect a user to 'log in' as (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= db_users.size())
		auth = db_users[idx - 1];

	cli_home();
}

void cli_user_signup() {
	cli_clear();

	std::cout << "Signing up...\nUsername: ";
	std::string username; std::cin >> username;
	std::cout << "Password: ";
	std::string password; std::cin >> password;

	auth = new User(username, password);
	db_users.push_back(auth);
	
	std::cout << "You are now logged in as user '" << auth->getUsername() << "'.\n";

	cli_await();
	cli_home();
}

void cli_user_logout() {
	cli_clear();
	std::cout << "You are now logged out.";
	auth = nullptr;
	cli_await();
	cli_home();
}

// Project
void cli_project_view(Project* project) {
	cli_clear();
	std::cout << "Viewing Project...\nTitle: " << project->getTitle(auth) << "\nDescription: " << project->getDescription(auth) << "\nFiles: \n";

	for (std::pair<std::string, std::string> pair : project->getFiles(auth)) {
		std::cout << pair.first << " : " << pair.second << "\n";
	}

	if (project->getUserPerm(auth) >= USER_PERM_OWNER && auth != nullptr) {
		int op = cli_menu({ 
			"Update Project Details", 
			"View Project's Users", 
			"Update Project's Users", 
			"View Project's Tickets", 
			"View Project's Pull Requests", 
			"Create New Ticket", 
			"Create New Pull Request",
			"Home"
		});

		switch (op) {
		case 1: cli_project_update(project); break;
		case 2: cli_user_view_in_project(project); break;
		case 3: cli_project_modify_users(project); break;
		case 4: cli_ticket_view_in_project(project); break;
		case 5: cli_pr_view_in_project(project); break;
		case 6: cli_ticket_create(project); break;
		case 7: cli_pr_create(project); break;
		case 8: cli_home(); break;
		}
	} else if (project->getUserPerm(auth) >= USER_PERM_MAINTAINER && auth != nullptr) {
		int op = cli_menu({
			"View Project's Users",
			"View Project's Tickets",
			"View Project's Pull Requests",
			"Create New Ticket",
			"Create New Pull Request",
			"Home"
			});

		switch (op) {
		case 1: cli_user_view_in_project(project); break;
		case 2: cli_ticket_view_in_project(project); break;
		case 3: cli_pr_view_in_project(project); break;
		case 4: cli_ticket_create(project); break;
		case 5: cli_pr_create(project); break;
		case 6: cli_home(); break;
		}
	} else if (project->getUserPerm(auth) >= USER_PERM_VIEWER) {
		int op = cli_menu({
			"View Project's Users",
			"View Project's Tickets",
			"View Project's Pull Requests",
			"Home"
		});

		switch (op) {
		case 1: cli_user_view_in_project(project); break;
		case 2: cli_ticket_view_in_project(project); break;
		case 3: cli_pr_view_in_project(project); break;
		case 4: cli_home(); break;
		}
	}
	else {
		cli_await();
		cli_home();
	}
}

void cli_project_view_vector(std::vector<Project*> projects) {
	cli_clear();
	std::cout << "Projects:\n";

	for (int i = 0; i < projects.size(); i++)
		std::cout << '\t' << i + 1 << ". " << projects[i]->getTitle(auth) << '\n';

	std::cout << "\nSelect a project to view (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= projects.size())
		cli_project_view(projects[idx - 1]);
	else
		cli_home();
}

void cli_project_view_in_user(User* user) {
	std::vector<Project*> projects = {};
	for (int i = 0; i < db_projects.size(); i++)
		if (db_projects[i]->getUserPerm(user) == USER_PERM_OWNER)
			projects.push_back(db_projects[i]);

	cli_project_view_vector(projects);
}

void cli_project_create() {
	cli_clear();

	std::cout << "Creating New Project...\nProject Title: ";
	std::string title; std::cin >> title;
	std::cout << "Project Description: ";
	std::string description; std::cin >> description;

	Project* p = new Project(title, description, USER_PERM_VIEWER, {{auth, USER_PERM_OWNER}});
	db_projects.push_back(p);

	std::cout << "New project has been created.";

	cli_await();
	cli_home();
}

void cli_project_update(Project* project) {
	cli_clear();

	std::cout << "Modifying Project Details...\nTitle: ";
	std::string title; std::cin >> title;
	project->setTitle(auth, title);

	std::cout << "Description: ";
	std::string description; std::cin >> description;
	project->setDescription(auth, description);

	std::cout << "Project details updated.\n";

	cli_await();
	cli_home();
}

void cli_project_modify_users(Project* project) {
	cli_clear();

	std::cout << "Modifying Project User Perms...\nUsername: ";
	std::string username; std::cin >> username;

	User* user = nullptr;

	for (int i = 0; i < db_users.size(); i++) {
		if (db_users[i]->getUsername() == username) {
			user = db_users[i];
			break;
		}
	}

	std::cout << "Permissions: (NONE = 0, VIEW = 1, MAINTAIN = 2, REVIEW = 3, ADMINISTRATE = 4)";
	int user_perm; std::cin >> user_perm;

	project->setUserPerm(auth, user, user_perm);
	std::cout << "Project user permissions updated.\n";

	cli_await();
	cli_home();
}

// Ticket
void cli_ticket_view(Ticket* ticket) {
	cli_clear();
	std::cout << "Viewing Ticket...\nTitle: " << ticket->getTitle(auth) << "\nDescription: " << ticket->getDescription(auth) << "\nClosed: " << (ticket->getClosed() ? "Yes" : "No") << "\n";

	if (auth == ticket->getOwner()) {
		int op = cli_menu({ "Update Ticket Details", "Update Ticket State", "View Ticket Owner", "View Ticket Project", "Home" });

		switch (op) {
		case 1: cli_ticket_update(ticket); break;
		case 2: cli_ticket_closed_update(ticket); break;
		case 3: cli_user_view(ticket->getOwner()); break;
		case 4: cli_project_view(ticket->getProject()); break;
		case 5: cli_home(); break;
		}
	}
	else if (auth != nullptr && ticket->getProject()->getUserPerm(auth) >= USER_PERM_REVIEWER) {
		int op = cli_menu({ "Update Ticket State", "View Ticket Owner", "View Ticket Project", "Go Home" });

		switch (op) {
		case 1: cli_ticket_closed_update(ticket); break;
		case 2: cli_user_view(ticket->getOwner()); break;
		case 3: cli_project_view(ticket->getProject()); break;
		case 4: cli_home(); break;
		}
	}
	else {
		int op = cli_menu({ "View Ticket Owner", "View Ticket Project", "Home"});

		switch (op) {
		case 1: cli_user_view(ticket->getOwner()); break;
		case 2: cli_project_view(ticket->getProject()); break;
		case 3: cli_home(); break;
		}
	}
}

void cli_ticket_view_vector(std::vector<Ticket*> tickets) {
	cli_clear();
	std::cout << "Tickets:\n";

	for (int i = 0; i < tickets.size(); i++)
		std::cout << '\t' << i + 1 << ". " << tickets[i]->getTitle(auth) << '\n';

	std::cout << "\nSelect a ticket to view (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= tickets.size())
		cli_ticket_view(tickets[idx - 1]);
	else
		cli_home();
}

void cli_ticket_view_in_user(User* user) {
	std::vector<Ticket*> tickets = {};
	for (int i = 0; i < db_tickets.size(); i++)
		if (db_tickets[i]->getOwner() == user)
			tickets.push_back(db_tickets[i]);

	cli_ticket_view_vector(tickets);
}

void cli_ticket_view_in_project(Project* project) {
	std::vector<Ticket*> tickets = {};
	for (int i = 0; i < db_tickets.size(); i++)
		if (db_tickets[i]->getProject() == project)
			tickets.push_back(db_tickets[i]);

	cli_ticket_view_vector(tickets);
}

void cli_ticket_create(Project* project) {
	cli_clear();

	std::cout << "Creating Ticket...\nTicket Title: ";
	std::string title; std::cin >> title;
	std::cout << "Ticket Description: ";
	std::string description; std::cin >> description;

	Ticket* t = new Ticket(auth, project, false, title, description);
	db_tickets.push_back(t);

	std::cout << "New ticket has been created.";

	cli_await();
	cli_home();
}

void cli_ticket_update(Ticket* ticket) {
	cli_clear();

	std::cout << "Updating Ticket Details...\nTitle: ";
	std::string title; std::cin >> title;
	ticket->setTitle(auth, title);

	std::cout << "Description: ";
	std::string description; std::cin >> description;
	ticket->setDescription(auth, description);

	std::cout << "Ticket details updated.\n";

	cli_await();
	cli_home();
}

void cli_ticket_closed_update(Ticket* ticket) {
	cli_clear();

	std::cout << "Updating Ticket State...\nState (0 for Open, 1 for Closed): ";
	int state; std::cin >> state;
	ticket->setClosed(auth, state >= 1);

	std::cout << "Ticket state updated.\n";

	cli_await();
	cli_home();
}

// PR
void cli_pr_view(PullRequest* pr) {
	cli_clear();
	std::cout << "Viewing PR...\nTitle: " << pr->getTitle(auth) << "\nDescription: " << pr->getDescription(auth) << "\nChanged Files: \n";

	for (std::pair<std::string, std::string> pair : pr->getChangedFiles(auth)) {
		std::cout << pair.first << " : " << pair.second << "\n";
	}

	if (auth == pr->getOwner()) {
		int op = cli_menu({ "Update PR Details", "Update PR State", "Update PR Files", "View PR Owner", "View PR Project", "Home" });

		switch (op) {
		case 1: cli_pr_update(pr); break;
		case 2: cli_pr_state_update(pr); break;
		case 3: cli_pr_file_update(pr); break;
		case 4: cli_user_view(pr->getOwner()); break;
		case 5: cli_project_view(pr->getProject()); break;
		case 6: cli_home(); break;
		}
	}
	else if (auth != nullptr && pr->getProject()->getUserPerm(auth) >= USER_PERM_REVIEWER) {
		int op = cli_menu({ "Update PR State", "View PR Owner", "View PR Project", "Go Home" });

		switch (op) {
		case 1: cli_pr_state_update(pr); break;
		case 2: cli_user_view(pr->getOwner()); break;
		case 3: cli_project_view(pr->getProject()); break;
		case 4: cli_home(); break;
		}
	}
	else {
		int op = cli_menu({ "View PR Owner", "View PR Project", "Home" });

		switch (op) {
		case 1: cli_user_view(pr->getOwner()); break;
		case 2: cli_project_view(pr->getProject()); break;
		case 3: cli_home(); break;
		}
	}
}

void cli_pr_view_vector(std::vector<PullRequest*> prs) {
	cli_clear();
	std::cout << "Pull Requests:\n";

	for (int i = 0; i < prs.size(); i++)
		std::cout << '\t' << i + 1 << ". " << prs[i]->getTitle(auth) << '\n';

	std::cout << "\nSelect a pull request to view (or 0 to go home): #";
	int idx = 0; std::cin >> idx;

	if (idx > 0 && idx <= prs.size())
		cli_pr_view(prs[idx - 1]);
	else
		cli_home();
}

void cli_pr_view_in_user(User* user) {
	std::vector<PullRequest*> prs = {};
	for (int i = 0; i < db_prs.size(); i++)
		if (db_prs[i]->getOwner() == user)
			prs.push_back(db_prs[i]);

	cli_pr_view_vector(prs);
}

void cli_pr_view_in_project(Project* project) {
	std::vector<PullRequest*> prs = {};
	for (int i = 0; i < db_prs.size(); i++)
		if (db_prs[i]->getProject() == project)
			prs.push_back(db_prs[i]);

	cli_pr_view_vector(prs);
}

void cli_pr_create(Project* project) {
	cli_clear();

	std::cout << "Creating PR...\nPR Title: ";
	std::string title; std::cin >> title;
	std::cout << "PR Description: ";
	std::string description; std::cin >> description;

	PullRequest* pr = new PullRequest(auth, project, title, description);
	db_prs.push_back(pr);

	std::cout << "New Pull Request has been created.";

	cli_await();
	cli_home();
}

void cli_pr_update(PullRequest* pr) {
	cli_clear();

	std::cout << "Updating PR Details...\nTitle: ";
	std::string title; std::cin >> title;
	pr->setTitle(auth, title);

	std::cout << "Description: ";
	std::string description; std::cin >> description;
	pr->setDescription(auth, description);

	std::cout << "Pull Request details updated.\n";

	cli_await();
	cli_home();
}

void cli_pr_state_update(PullRequest* pr) {
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
	cli_home();
}

void cli_pr_file_update(PullRequest* pr) {
	cli_clear();

	std::cout << "Updating PR File...\nFile Name: ";
	std::string file_name; std::cin >> file_name;
	std::cout << "File Content: ";
	std::string file_content; std::cin >> file_content;

	pr->setChangedFileContent(auth, file_name, file_content);
	std::cout << "Pull Request details updated.\n";

	cli_await();
	cli_home();
}