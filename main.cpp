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
		"Wrong titlebar color",
		"On devices running iOS 7 and up, the title bar doesn't show the correct color."
	));
	db_tickets.push_back(new Ticket(
		db_users[1],
		db_projects[0],
		"Database may be insecure", 
		"We're storing all of the students' passwords in plain-text? Really? This needs to be addressed ASAP!"
	));
	db_tickets.push_back(new Ticket(
		db_users[3],
		db_projects[1],
		"Missing class courses", 
		"We haven't updated our course lists to include the newly-added Cybersec course."
	));
	db_tickets.push_back(new Ticket(
		db_users[4],
		db_projects[1],
		"No dark mode", 
		"I'm gonna have to fix this soon, it hurts to look at."
	));

	// Generate Pull Requests
	std::cout << "Creating Pull Requests...\n";

	db_prs.push_back(new PullRequest(
		db_users[2], db_projects[0], 
		"Fix titlebar color issue", 
		"Updated titlebar.css follow iOS 7 website specifications.",
		{ db_tickets[0]}, 
		{{"titlebar.css", "ios_color_tag = black"}}
	));

	db_prs.push_back(new PullRequest(
		db_users[1],
		db_projects[1],
		"Add 'secure' field to database", 
		"Surely this fixes all of our problems!",
		{ db_tickets[1]},
		{ {"database.oxl", "is_secure = true"} }
	));

	db_prs.push_back(new PullRequest(
		db_users[4],
		db_projects[1],
		"Add dark mode", 
		"Needs more testing",
		{ db_tickets[3]},
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
void cli_ticket_create();
void cli_ticket_update(Ticket*);

void cli_pr_view(PullRequest*);
void cli_pr_view_vector(std::vector<PullRequest*>);
void cli_pr_view_in_user(User*);
void cli_pr_view_in_project(Project*);
void cli_pr_create();
void cli_pr_update(PullRequest*);

int main()
{
	generate_sample_database_entries();
	cli_home();
	return 0;
}

// CLI Implementations
User* auth = nullptr;

void cli_clear() {
	//system("CLS");
}

void cli_await() {
	int x; std::cout << '\n'; std::cin >> x;
}

int cli_menu(std::vector<std::string> options)
{
	std::cout << "Menus:\n";

	for (int i = 0; i < options.size(); i++) {
		std::cout << "\t" << i + 1 << ". " << options[i] << "\n";
	}

	std::cout << "\nOption: #";
	int option = 0;

	while (option < 1 || option > options.size()) 
		std::cin >> option;

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
	std::cout << "(TODO)\nUser View Page for " << user->getUsername();

	int op = cli_menu({ "Update User Details", "View User's Projects", "View User's Tickets", "View User's Pull Requests", "Force Login" });

	switch (op) {
	case 1: cli_user_update(user); break;
	case 2: cli_project_view_in_user(user); break;
	case 3: cli_ticket_view_in_user(user); break;
	case 4: cli_pr_view_in_user(user); break;
	case 5: auth = user; cli_home(); break;
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
	std::cout << "(TODO)\nUser Update Page for " << user->getUsername();
	cli_await();
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
	std::cout << "(TODO)\nUser Signup Page";
	cli_await();
	cli_home();
}

void cli_user_logout() {
	cli_clear();
	std::cout << "(TODO)\nUser Logout Page";
	auth = nullptr;
	cli_await();
	cli_home();
}

// Project
void cli_project_view(Project* project) {
	cli_clear();
	std::cout << "(TODO)\nProject View Page for " << project->getTitle(auth) << '\n';

	int op = cli_menu({ "Update Project Details", "View Project's Users", "View Project's Tickets", "View Project's Pull Requests" });

	switch (op) {
	case 1: cli_project_update(project); break;
	case 2: cli_user_view_in_project(project); break;
	case 3: cli_ticket_view_in_project(project); break;
	case 4: cli_pr_view_in_project(project); break;
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
		cli_project_view(projects[idx]);
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
	std::cout << "(TODO)\nProject Create Page";
	cli_await();
	cli_home();
}

void cli_project_update(Project* project) {
	cli_clear();
	std::cout << "(TODO)\nProject Update Page for " << project->getTitle(auth);
	cli_await();
	cli_home();
}

void cli_project_modify_users(Project* project) {
	cli_clear();
	std::cout << "(TODO)\nProject Modify Users Page for " << project->getTitle(auth);
	cli_await();
	cli_home();
}

// Ticket
void cli_ticket_view(Ticket* ticket) {
	cli_clear();
	std::cout << "(TODO)\nTicket View Page for " << ticket->getTitle(auth) << '\n';

	int op = cli_menu({ "Update Ticket Details", "View Ticket Owner", "View Ticket Project" });

	switch (op) {
	case 1: cli_ticket_update(ticket); break;
	case 2: cli_user_view(ticket->getOwner()); break;
	case 3: cli_project_view(ticket->getProject()); break;
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
		cli_ticket_view(tickets[idx]);
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

void cli_ticket_create() {
	cli_clear();
	std::cout << "(TODO)\nTicket Create Page";
	cli_await();
	cli_home();
}

void cli_ticket_update(Ticket* ticket) {
	cli_clear();
	std::cout << "(TODO)\nTicket Update Page for " << ticket->getTitle(auth);
	cli_await();
	cli_home();
}

// PR
void cli_pr_view(PullRequest* pr) {
	cli_clear();
	std::cout << "(TODO)\nPR View Page for " << pr->getTitle(auth) << '\n';

	int op = cli_menu({ "Update PR Details", "View PR Owner", "View PR Project" });

	switch (op) {
	case 1: cli_pr_update(pr); break;
	case 2: cli_user_view(pr->getOwner()); break;
	case 3: cli_project_view(pr->getProject()); break;
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
		cli_pr_view(prs[idx]);
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

void cli_pr_create() {
	cli_clear();
	std::cout << "(TODO)\nPR Create Page";
	cli_await();
	cli_home();
}

void cli_pr_update(PullRequest* pr) {
	cli_clear();
	std::cout << "(TODO)\nPR Update Page for " << pr->getTitle(auth);
	cli_await();
	cli_home();
}
