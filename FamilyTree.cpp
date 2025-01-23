#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include "ScanFile.h"
#include <ctime>

using namespace std;
constexpr int COUNT = 5;
unordered_map<string, vector<string>> familyTree;
const int numOFChildren = 10;

struct person
{
	string id;
	string name;
	char gender;
	person* father;
	person* mother;
	person* children[numOFChildren];
	person* sibling;
	person* spouse;
	bool visited;
	tm birthdate;
	string birthplace;
	int getAge() const;
	person(string Id = "", string Name = "", char Gender = 'M',
		person* Father = nullptr, person* Mother = nullptr,
		tm Birthdate = tm(), string Birthplace = "")
		: id(Id), name(Name), gender(Gender), father(Father),
		mother(Mother), sibling(nullptr), birthdate(Birthdate),
		birthplace(Birthplace), spouse(nullptr), visited(false)
	{
		for (int i = 0; i < numOFChildren; i++)
			children[i] = nullptr;
	}
};

person* virtualRoot = nullptr;
person* createPerson(string id, string name, char gender, person* father, person* mother, tm birthdate, string birthplace);
person* createPersonUserInput();
person* findPerson(string);
person* rootPerson = nullptr;
void Prinlist();
void removePerson(string personTODelete);
int checkAvailableChild(person* parent);
void displayPerson(string name);
void outputPerson(person* person);
void navigateFamily(string name);
int mainMenuDisplay();
void printPersonList(person* p);
void selectOption(int option);
bool markChild(person* parent, person* child);
bool markSibling(person* sibling1, person* sibling2);
person* check(person* toCheck, string nameTOFind);
bool unMarkParent(person* parent, person* person);
person* unMarkChild(person* p);
void displayTotalTree();
void initializedChild();
bool isValidInput();
bool isValidString(const string& str);
void handleMenu();
void printFamily(person* root);
void outputFamilyTreeToFile(person* p, int space, ofstream& file, unordered_set<person*>& visitedNodes);
void saveFamilyTreeToFile(const string& filename);
void handleDisplayOrSaveTree();
void handleCreateOrLoadFamily();
static void loadFamilyTreeFromFile();
void getSiblings(person* p, person* siblings[], int& siblingCount);
void addSiblingsToArray(person* p, person* parent, person* siblings[], int& siblingCount);
string findRelationship(const string& id1, const string& id2);
string checkNoRelation(person* p1, person* p2);
string checkCousinRelation(person* p1, person* p2);
string checkGrandparentRelation(person* p1, person* p2);
string checkSiblingRelation(person* p1, person* p2);
string checkParentChildRelation(person* p1, person* p2);
string checkSpouseRelation(person* p1, person* p2);
void printTreeWithParentOrder(person* root, int level, bool isLast, unordered_set<person*>& visitedNodes);
void printTree(person* root);
bool checkAndMarkSpouse(person* child);
bool markSpouse(person* person1, person* person2);

int main()
{
	char choice;

	initializedChild();
	virtualRoot = new person("", "", ' ');
	rootPerson = virtualRoot;
	handleMenu();
Menu:
	int option = mainMenuDisplay();
	selectOption(option);

flag:
	cout << " \nPress(Y) to continue or (n) exit ! : ";
	cin >> choice;
	if (!isValidInput())
	{
		cout << "Invalid input try again.. !";
		goto flag;
	}
	choice = toupper(choice);
	if (choice == 'Y')
		goto Menu;
	else if (choice == 'N')
		exit(1);
	else
	{
		cout << "Invalid input.. please try again..!";
		goto flag;
	}
	system("pause");
	return 0;
}

void initializedChild()
{
	person initP;

	for (int i = 0; i < numOFChildren; i++)
		initP.children[i] = NULL;
}

int mainMenuDisplay()
{
	int option;
	system("CLS");

	cout << "__________________________________________________\n"
		<< endl;
	cout << " (1) Create Person" << endl;
	cout << " (2) Find person" << endl;
	cout << " (3) Remove person" << endl;
	cout << " (4) Display relationship" << endl;
	cout << " (5) Navigate family" << endl;
	cout << " (6) Display and print total tree" << endl;
	cout << " (7) Exit " << endl;
	cout << "__________________________________________________" << endl;

	while (true)
	{
		cout << "\nSelect choice from the options above (1-7): ";
		cin >> option;

		if (!isValidInput())
		{
			cout << "Invalid input, try again.. !" << endl;
			continue;
		}

		if (option >= 1 && option <= 7)
		{
			return option;
		}
		else
		{
			cout << "Invalid option, please select between 1-7." << endl;
		}
	}
}

void handleMenu()
{
	int option;
	do
	{
		option = mainMenuDisplay();

		if (option == 0)
		{
			cout << "Returning to the menu..." << endl;
			continue;
		}

		selectOption(option);
		cout << "\nPress Enter to return to the menu...";
		cin.ignore();
		cin.get();

	} while (option != 8);
}

void selectOption(int option)
{
	string name, id;
	person* pp;

	switch (option)
	{
	case 1:
		handleCreateOrLoadFamily();
		break;
	case 2:
	{
		if (rootPerson == nullptr)
		{
			cout << "The family tree is empty.\n";
			break;
		}

		while (true)
		{

			Prinlist();
			cout << "\nEnter the person's ID to find (or 0 to go back to the menu): ";
			cin >> id;
			if (!isValidInput())
			{
				cout << "Invalid input, try again.. !" << endl;
				continue;
			}
			if (id == "0")
				break;
			pp = findPerson(id);
			if (pp == NULL)
				cout << "Person isn't found!" << endl;
			else
				cout << "Person is found!" << endl;
			break;
		}
		break;
	}
	case 3:
	{
		while (true)
		{

			Prinlist();
			cout << "Please enter the person's ID to remove (or 0 to go back to the menu): ";
			cin >> id;
			if (!isValidInput())
			{
				cout << "Invalid input, try again.. !" << endl;
				continue;
			}
			if (id == "0")
				break;
			removePerson(id);
			break;
		}
		break;
	}
	case 4:
	{
		while (true)
		{
			string id1, id2;
			cout << "Please enter the person's ID to find relationship (or 0 to go back to the menu): ";
			cout << "\nId first person :   ";
			cin >> id1;
			if (id1 == "0")
				break;
			if (!isValidInput())
			{
				cout << "Invalid input, try again.. !" << endl;
				continue;
			}
			cout << "Id second person :   ";
			cin >> id2;
			if (!isValidInput())
			{
				cout << "Invalid input, try again.. !" << endl;
				continue;
			}
			cout << findRelationship(id1, id2);
			break;
		}
		break;
	}
	case 5:
	{
		while (true)
		{
			cout << "Please enter the person's ID to navigate family (or 0 to go back to the menu): ";
			cin >> id;
			if (!isValidInput())
			{
				cout << "Invalid input, try again.. !" << endl;
				continue;
			}
			if (id == "0")
				break;
			navigateFamily(id);
			break;
		}
		break;
	}
	case 6:
		handleDisplayOrSaveTree();
		break;
	case 7:
		cout << "Exiting program. Goodbye!" << endl;
		exit(0);
		break;
	default:
		cout << "Invalid option. Please try again." << endl;
	}
}
void Prinlist()
{
	cout << "Id\tName\tGender\n";
	cout << "---------------------\n";
	printPersonList(rootPerson);
}
void printPersonList(person* p)
{

	if (p == nullptr)
		return;

	static unordered_map<string, bool> printed;

	cout << p->id << "\t" << p->name << "\t" << p->gender << endl;
	printed[p->id] = true;

	for (int i = 0; i < numOFChildren; i++)
	{
		if (p->children[i] != nullptr)
		{
			printPersonList(p->children[i]);
		}
	}
}

person* createPersonUserInput()
{
	string id, name, fatherId, motherId;
	person* pn = nullptr;
	char gender;
	int year, month, day;
	string birthplace;

	while (true)
	{
		cout << "Enter father ID (enter 'e' if not applicable, or '0' to cancel): ";
		cin >> fatherId;

		if (fatherId == "0")
		{
			cout << "Creation process canceled. Returning to menu...\n";
			return nullptr;
		}

		cout << "Enter mother ID (enter 'e' if not applicable, or '0' to cancel): ";
		cin >> motherId;

		if (motherId == "0")
		{
			cout << "Creation process canceled. Returning to menu...\n";
			return nullptr;
		}

		if ((isValidString(fatherId) || fatherId == "e") && (isValidString(motherId) || motherId == "e"))
		{
			person* father = (fatherId == "e") ? nullptr : findPerson(fatherId);
			person* mother = (motherId == "e") ? nullptr : findPerson(motherId);

			if ((father != nullptr || fatherId == "e") && (mother != nullptr || motherId == "e"))
			{
				while (true)
				{
					cout << "\nEnter unique ID for the person: ";
					cin >> id;

					if (id == "0")
					{
						cout << "Creation process canceled. Returning to menu...\n";
						return nullptr;
					}

					if (!id.empty() && isValidString(id) && findPerson(id) == nullptr)
					{
						break;
					}
					else
					{
						cout << "Invalid or duplicate ID. Please try again.\n";
					}
				}
				while (true)
				{
					cout << "\nEnter person name (or enter '0' to cancel): ";
					cin.ignore();
					getline(cin, name);

					if (name == "0")
					{
						cout << "Creation process canceled. Returning to menu...\n";
						return nullptr;
					}

					if (!name.empty() && isValidString(name))
					{
						break;
					}
					else
					{
						cout << "Invalid name, please try again.\n";
					}
				}

				while (true)
				{
					cout << "\nEnter gender (M for male, F for female, or '0' to cancel): ";
					cin >> gender;

					if (gender == '0')
					{
						cout << "Creation process canceled. Returning to menu...\n";
						return nullptr;
					}

					gender = toupper(gender);

					if (gender == 'M' || gender == 'F')
					{
						break;
					}
					else
					{
						cout << "Invalid gender, please try again.\n";
					}
				}
				while (true)
				{
					cout << "Enter birthdate (YYYY MM DD, or 0 to cancel): ";
					cin >> year >> month >> day;

					if (year == 0)
					{
						cout << "Creation process canceled. Returning to menu...\n";
						return nullptr;
					}

					if (cin.fail() || year < 0 || month < 1 || month > 12 || day < 1 || day > 31)
					{
						cin.clear();
						cin.ignore(512, '\n');
						cout << "Invalid date format. Please try again.\n";
					}
					else
					{
						tm birthdate = {};
						birthdate.tm_year = year - 1900;
						birthdate.tm_mon = month - 1;	
						birthdate.tm_mday = day;	

						cout << "Enter birthplace: ";
						cin.ignore(); 
						getline(cin, birthplace);

						pn = createPerson(id, name, gender, father, mother, birthdate, birthplace);
						break;
					}
				}
				if (pn != nullptr)
				{
					cout << "\nPerson created successfully!\n";
					return pn;
				}
				else
				{
					cout << "Failed to create person. Please try again.\n";
				}
			}
			else
			{
				cout << "\nFather or mother doesn't exist, please try again.\n";
			}
		}
		else
		{
			cout << "Invalid input for father or mother ID, please try again.\n";
		}
	}
}

void handleDisplayOrSaveTree()
{
	int subOption;
	cout << "Select an option:\n";
	cout << "(1) Display total family tree\n";
	cout << "(2) Save family tree to file\n";
	cout << "Enter your choice (1 or 2): ";
	cin >> subOption;

	if (cin.fail() || (subOption != 1 && subOption != 2))
	{
		cin.clear();
		cin.ignore(512, '\n');
		cout << "Invalid input, please enter 1 or 2.\n";
		return;
	}

	switch (subOption)
	{
	case 1:
		displayTotalTree();
		break;
	case 2:
	{
		cout << "Enter filename to save family tree: ";
		string filename;
		cin >> filename;
		saveFamilyTreeToFile(filename);
		break;
	}
	default:
		cout << "Invalid option selected. Please try again.\n";
		break;
	}
}

person* createPerson(string id, string name, char gender, person* father, person* mother, tm birthdate, string birthplace)
{
	if (findPerson(id) != nullptr)
	{
		cout << "Person ID already exists.\n";
		return nullptr;
	}

	person* newPerson = new person(id, name, gender, father, mother, birthdate, birthplace);

	if (father != nullptr)
	{
		if (!markChild(father, newPerson))
		{
			return nullptr;
		}
	}
	if (mother != nullptr)
	{
		if (!markChild(mother, newPerson))
		{
			return nullptr;
		}
	}
	else if (father == nullptr && mother == nullptr)
	{
		if (!markChild(virtualRoot, newPerson))
		{
			return nullptr;
		}

		if (rootPerson == nullptr)
		{
			rootPerson = newPerson;
		}
	}

	return newPerson;
}

bool markChild(person* parent, person* child) {
	if (parent == nullptr || child == nullptr) {
		std::cout << "Error: parent or child is null." << std::endl;
		return false;
	}
	int availChild = checkAvailableChild(parent);
	if (availChild == -1) {
		std::cout << "This parent already has " << numOFChildren << " children. Please enter another parent name." << std::endl;
		return false;
	}
	parent->children[availChild] = child;
	std::cout << "Added " << child->id << " as child of " << parent->id << " at index " << availChild << std::endl;

	if (parent->gender == 'M') {
		child->father = parent;
	}
	else if (parent->gender == 'F') {
		child->mother = parent;
	}

	if (checkAndMarkSpouse(child)) {
		std::cout << "Parents of " << child->name << " have been marked as spouses." << std::endl;
	}

	for (int i = 0; i < numOFChildren; i++) {
		if (i != availChild && parent->children[i] != nullptr) {
			markSibling(parent->children[i], child);
		}
	}

	return true;
}

bool checkAndMarkSpouse(person* child) {
	if (child == nullptr) {
		std::cout << "Error: Child is null." << std::endl;
		return false;
	}

	if (child->father != nullptr && child->mother != nullptr) {
		if (child->father->spouse == nullptr && child->mother->spouse == nullptr) {
			return markSpouse(child->father, child->mother);
		}
		else {
			std::cout << "Parents of " << child->name << " are already marked as spouses." << std::endl;
		}
	}
	else {
		std::cout << "Child " << child->name << " does not have both parents set." << std::endl;
	}

	return false;
}
bool markSpouse(person* person1, person* person2) {
	if (person1 == nullptr || person2 == nullptr) {
		std::cout << "Error: One or both persons are null." << std::endl;
		return false;
	}

	if (person1->spouse != nullptr || person2->spouse != nullptr) {
		std::cout << "Error: One or both persons already have a spouse." << std::endl;
		return false;
	}

	person1->spouse = person2;
	person2->spouse = person1;

	std::cout << "Marked " << person1->name << " and " << person2->name << " as spouses." << std::endl;
	return true;
}
int checkAvailableChild(person* parent)
{
	for (int i = 0; i < numOFChildren; i++)
		if (parent->children[i] == nullptr)
			return i;
	return -1;
}

bool markSibling(person* sibling1, person* sibling2)
{
	if (sibling1 == nullptr || sibling2 == nullptr)
	{
		cout << "Error: One of the siblings is null." << endl;
		return false;
	}

	if ((sibling1->father != sibling2->father) || (sibling1->mother != sibling2->mother))
	{
		cout << "Error: The two people do not share the same parents, so they cannot be siblings." << endl;
		return false;
	}

	sibling1->sibling = sibling2;
	sibling2->sibling = sibling1;

	return true;
}

person* findPerson(string id)
{
	if (rootPerson == nullptr)
	{
		cout << "The family tree is empty.\n";
		return nullptr;
	}
	return check(rootPerson, id);
}

person* check(person* toCheck, string id)
{
	if (toCheck == nullptr)
		return nullptr;

	if (toCheck->id == id)
		return toCheck;

	for (int i = 0; i < numOFChildren; i++)
		if (toCheck->children[i] != nullptr)
		{
			person* p = check(toCheck->children[i], id);
			if (p != nullptr)
				return p;
		}
	return nullptr;
}

void removePerson(string id)
{
	person* pp = findPerson(id);

	while (pp == nullptr)
	{
		cout << "Person ID you entered doesn't exist. Try again!" << endl;
		cout << "\nEnter the person ID to remove: ";
		cin >> id;

		if (cin.fail() || cin.peek() != '\n')
		{
			cin.clear();
			cin.ignore(512, '\n');
			cout << "Invalid person ID. Try again!" << endl;
		}
		else
			pp = findPerson(id);
	}

	if (pp->father != nullptr)
	{
		bool result = unMarkParent(pp->father, pp);
		if (!result)
		{
			cout << "Something went wrong when removing from father. Try again!" << endl;
			return;
		}
	}

	if (pp->mother != nullptr)
	{
		bool result = unMarkParent(pp->mother, pp);
		if (!result)
		{
			cout << "Something went wrong when removing from mother. Try again!" << endl;
			return;
		}
	}

	person* p = unMarkChild(pp);

	if (p != nullptr)
	{
		delete p;
		cout << "Person removed successfully!" << endl;
	}
}

bool unMarkParent(person* parent, person* child)
{
	if (parent == nullptr || child == nullptr)
	{
		cout << "Error: parent or child is null." << endl;
		return false;
	}

	for (int i = 0; i < numOFChildren; i++)
	{
		if (parent->children[i] == child)
		{
			parent->children[i] = nullptr;
			return true;
		}
	}

	return false;
}

person* unMarkChild(person* p)
{
	if (p == nullptr)
		return nullptr;

	for (int i = 0; i < numOFChildren; i++)
	{
		if (p->children[i] != nullptr)
		{
			person* child = p->children[i];
			p->children[i] = nullptr;
			delete child;
		}
	}

	return p;
}

void displayPerson(string id)
{
	char choice;
	person* pp = findPerson(id);

	while (pp == nullptr)
	{
		cout << "Person doesn't exist. Would you like to try again? Enter (y) or (n): ";
		cin >> choice;
		cout << endl;

		if (!isValidInput())
		{
			cout << "Invalid input. Try again!" << endl;
			continue;
		}

		choice = toupper(choice);
		if (choice == 'Y')
		{
			cout << "Enter the person name to search: ";
			cin >> id;
			pp = findPerson(id);
		}
		else if (choice == 'N')
			exit(1);
		else
			cout << "Invalid input. Please enter 'y' or 'n'!" << endl;
	}

	outputPerson(pp);
}

void outputPerson(person* person)
{
	if (!person)
	{
		std::cout << "This person doesn't exist.\n";
		return;
	}

	std::cout << "ID: " << person->id << "\n";
	std::cout << "Name: " << person->name << "\n";
	std::cout << "Gender: " << (person->gender == 'M' ? "Male" : "Female") << "\n";
	char buffer[11];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d", &(person->birthdate));
	std::cout << "Birthdate: " << buffer << "\n";
	std::cout << "Birthplace: " << person->birthplace << "\n";
	std::cout << "Age: " << person->getAge() << "\n";

	if (person->father != nullptr)
	{
		std::cout << "Father's ID: " << person->father->id << "\n";
	}
	else
	{
		std::cout << "Father's information not available.\n";
	}

	if (person->mother != nullptr)
	{
		std::cout << "Mother's ID: " << person->mother->id << "\n";
	}
	else
	{
		std::cout << "Mother's information not available.\n";
	}
}
void outputFamilyTreeToFile(person* p, int space, ofstream& file, unordered_set<person*>& visitedNodes) {
	if (p == nullptr || visitedNodes.count(p)) {
		return;
	}
	visitedNodes.insert(p);
	for (int i = 5; i < space; i++) {
		file << " ";
	}

	file << "--> " << p->id << " - " << p->name << " (" << ((p->gender == 'M') ? "Male" : "Female") << ")";

	if (p->spouse) {
		file << " & " << p->spouse->name << " (" << p->spouse->id << ")";
	}

	int numChildren = 0;
	for (int i = 0; i < numOFChildren; i++) {
		if (p->children[i]) numChildren++;
	}

	int childCount = 0;
	for (int i = 0; i < numOFChildren; i++) {
		if (p->children[i]) {
			outputFamilyTreeToFile(p->children[i], space + 5, file, visitedNodes);
		}
	}
}
void outputFamilyTree(person* p, ofstream& file) {
	unordered_set<person*> visitedNodes;
	outputFamilyTreeToFile(p, 0, file, visitedNodes);
}

void navigateFamily(string id)
{
	string parentId, childId;
	char choice;
	int cnt;

	person* currentPerson = findPerson(id);

	if (currentPerson == nullptr)
	{
		cout << "Person not found!" << endl;
		return;
	}

	displayPerson(currentPerson->id);

	while (true)
	{
		cout << "To navigate other persons enter (1) or to quit enter (2): ";
		cin >> cnt;
		cout << endl;

		if (cnt == 2)
			return;
		else if (cnt != 1)
		{
			cout << "Invalid input.. please try again..!" << endl;
			continue;
		}

		cout << "Please enter (1) to navigate parents, (2) to navigate children, or (3) to navigate siblings: ";
		cin >> choice;

		if (choice != '1' && choice != '2' && choice != '3')
		{
			cout << "Invalid input, please try again!" << endl;
			continue;
		}

		cout << endl;

		if (choice == '1')
		{
			if (currentPerson->father != nullptr)
			{
				cout << "Father's ID: " << currentPerson->father->id << endl;
			}
			else
			{
				cout << "Father's information not available." << endl;
			}
			if (currentPerson->mother != nullptr)
			{
				cout << "Mother's ID: " << currentPerson->mother->id << endl;
			}
			else
			{
				cout << "Mother's information not available." << endl;
			}

			cout << "Enter the ID of the parent you want to navigate to: ";
			cin >> parentId;
			cout << endl;

			if ((currentPerson->father != nullptr && currentPerson->father->id == parentId) ||
				(currentPerson->mother != nullptr && currentPerson->mother->id == parentId))
			{
				displayPerson(parentId);
				currentPerson = findPerson(parentId);
			}
			else
			{
				cout << "Invalid parent's ID..! Try again." << endl;
			}
		}
		else if (choice == '2')
		{
			if (currentPerson)
			{
				cout << "Please enter child ID: ";
				cin >> childId;
				cout << endl;

				bool childFound = false;
				for (int i = 0; i < numOFChildren; i++)
				{
					if (currentPerson->children[i] != nullptr && currentPerson->children[i]->id == childId)
					{
						displayPerson(childId);
						currentPerson = findPerson(childId);
						childFound = true;
						break;
					}
				}

				if (!childFound)
				{
					cout << "This child ID doesn't exist. Please try again..!" << endl;
				}
			}
			else
			{
				cout << "Person not found!" << endl;
			}
		}
		else if (choice == '3')
		{
			if (currentPerson == nullptr)
			{
				cout << "Person not found!" << endl;
				return;
			}

			person* siblings[numOFChildren];
			int siblingCount = 0;
			getSiblings(currentPerson, siblings, siblingCount);

			if (siblingCount == 0)
			{
				cout << "No siblings found for " << currentPerson->name << " (" << currentPerson->id << ")." << endl;
			}
			else
			{
				cout << "Siblings of " << currentPerson->name << " (" << currentPerson->id << "):" << endl;
				for (int i = 0; i < siblingCount; i++)
				{
					cout << "- " << siblings[i]->name << " (" << siblings[i]->id << ")" << endl;
				}
			}
		}
	}
}

void displayTotalTree()
{
	if (rootPerson != nullptr)
	{
		cout << "Displaying family tree : \n";
		printTree(rootPerson);
	}
	else
	{
		cout << "No family tree found.\n";
	}
}
void printTreeWithParentOrder(person* root, int level, bool isLast, unordered_set<person*>& visitedNodes) {
	if (root == nullptr || visitedNodes.count(root)) return;

	visitedNodes.insert(root);

	for (int i = 0; i < level - 1; ++i) {
		cout << "    ";
	}
	if (level > 0) {
		cout << "|-- ";
	}

	cout << root->name<<"("<<root->id<<")";

	if (root->spouse && !visitedNodes.count(root->spouse)) {
		cout << " & " << root->spouse->name << "(" << root->spouse->id << ")";
		visitedNodes.insert(root->spouse);
	}
	cout << "\n";

	int numChildren = 0;
	for (int i = 0; i < numOFChildren; i++) {
		if (root->children[i]) numChildren++;
	}

	int childCount = 0;
	for (int i = 0; i < numOFChildren; i++) {
		if (root->children[i]) {
			printTreeWithParentOrder(root->children[i], level + 1, ++childCount == numChildren, visitedNodes);
		}
	}
}




void printTree(person* root) {
	unordered_set<person*> visitedNodes;
	printTreeWithParentOrder(root, 0, true, visitedNodes);
}

void printFamily(person* root)
{
	if (root == nullptr)
		return;
	std::cout << "Name: " << root->name << "\n";
	if (root->sibling)
	{
		std::cout << "Sibling: " << root->sibling->name << "\n";
	}
}

bool isValidInput()
{
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(512, '\n');
		return false;
	}
	cin.ignore(512, '\n');
	return true;
}

bool isValidString(const string& str)
{
	for (char c : str)
	{
		if (!isalnum(c) && c != ' ')
		{
			return false;
		}
	}
	return true;
}
void saveFamilyTreeToFile(const string& filename)
{
	string fileNameWithExtension = filename + ".txt";
	ofstream file(fileNameWithExtension);

	if (!file.is_open())
	{
		cout << "Error: Unable to open file for writing!" << endl;
		return;
	}

	person* p = rootPerson;

	if (p != nullptr)
	{
		file << "Family Tree of " << p->id << ":\n";
		unordered_set<person*> visitedNodes;
		outputFamilyTreeToFile(p, 0, file, visitedNodes);
		cout << "Family tree has been saved to " << fileNameWithExtension << endl;
	}
	else
	{
		file << "No family tree found.\n";
		cout << "No family tree to save!" << endl;
	}

	file.close();
}
static void loadFamilyTreeFromFile()
{
	stringvec txtFiles;

	std::string folder = ".";
	read_directory(folder, txtFiles, ".txt");

	if (txtFiles.empty())
	{
		std::cout << "No .txt files found in the folder.\n";
		return;
	}

	std::cout << "Available .txt files:\n";
	for (size_t i = 0; i < txtFiles.size(); ++i)
	{
		std::cout << i + 1 << ". " << txtFiles[i] << std::endl;
	}

	int choice = 0;
	do
	{
		std::cout << "Enter the number of the file you want to load: ";
		std::cin >> choice;
	} while (choice < 1 || choice > static_cast<int>(txtFiles.size()));

	std::string filename = txtFiles[choice - 1];

	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cout << "Error: Unable to open file \"" << filename << "\". Please check the file path and try again." << std::endl;
		return;
	}

	std::string id, name, fatherId, motherId, birthdateStr, birthplace;
	char gender;
	int year, month, day;
	person* newPerson = nullptr;

	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string token;
		std::vector<std::string> tokens;

		while (std::getline(ss, token, ','))
		{
			if (!token.empty())
			{
				tokens.push_back(token);
			}
		}

		if (tokens.size() >= 7)
		{
			fatherId = tokens[0];
			motherId = tokens[1];
			id = tokens[2];
			name = tokens[3];
			gender = tokens[4][0];
			birthdateStr = tokens[5];
			birthplace = tokens[6];

			try
			{
				year = stoi(birthdateStr.substr(0, 4));
				month = stoi(birthdateStr.substr(5, 2));
				day = stoi(birthdateStr.substr(8, 2));

				tm birthdate = {};
				birthdate.tm_year = year - 1900;
				birthdate.tm_mon = month - 1;
				birthdate.tm_mday = day;

				person* father = (fatherId == "e") ? nullptr : findPerson(fatherId);
				person* mother = (motherId == "e") ? nullptr : findPerson(motherId);

				if (findPerson(id) == nullptr)
				{
					newPerson = createPerson(id, name, gender, father, mother, birthdate, birthplace);
					if (newPerson != nullptr)
					{
						std::cout << "Loaded person: " << name << " with ID: " << id << std::endl;
					}
				}
			}
			catch (const std::exception& e)
			{
				std::cout << "Error parsing birthdate: " << e.what() << std::endl;
			}
		}
		else
		{
			std::cout << "Invalid line format in the file.\n";
		}
	}

	file.close();
}

void handleCreateOrLoadFamily()
{
	int subOption;
	cout << "---- Create or Load Family ----" << endl;
	cout << " (1) Create a person (user input)" << endl;
	cout << " (2) Load family tree from file (custom filename)" << endl;
	cout << "--------------------------------" << endl;

	while (true)
	{
		cout << "Enter your choice (1-2): ";
		cin >> subOption;

		if (!isValidInput())
		{
			cout << "Invalid input, try again..!" << endl;
			continue;
		}

		if (subOption == 1)
		{
			createPersonUserInput();
			break;
		}
		else if (subOption == 2)
		{
			loadFamilyTreeFromFile();
			break;
		}
		else
		{
			cout << "Invalid option, please select 1 or 2." << endl;
		}
	}
}

void getSiblings(person* p, person* siblings[], int& siblingCount)
{
	siblingCount = 0;

	if (p == nullptr)
	{
		return;
	}

	if (p->father != nullptr)
	{
		addSiblingsToArray(p, p->father, siblings, siblingCount);
	}
	if (p->mother != nullptr)
	{
		addSiblingsToArray(p, p->mother, siblings, siblingCount);
	}
}

void addSiblingsToArray(person* p, person* parent, person* siblings[], int& siblingCount)
{
	for (int i = 0; i < numOFChildren; i++)
	{
		person* sibling = parent->children[i];
		if (sibling != nullptr && sibling != p)
		{
			siblings[siblingCount++] = sibling;
		}
	}
}
string checkSpouseRelation(person* p1, person* p2)
{
	for (int i = 0; i < numOFChildren; i++)
	{
		for (int j = 0; j < numOFChildren; j++)
		{
			if (p1->children[i] != nullptr && p1->children[i] == p2->children[j])
			{
				if ((p1 == p1->children[i]->father || p1 == p1->children[i]->mother) &&
					(p2 == p1->children[i]->father || p2 == p1->children[i]->mother))
				{
					return "spouse - spouse";
				}
			}
		}
	}

	return "";
}

string checkParentChildRelation(person* p1, person* p2)
{
	if (p1->father == p2)
		return "father - child";
	if (p1->mother == p2)
		return "mother - child";
	if (p2->father == p1)
		return p2->gender == 'M' ? "parent - son" : "parent - daughter";
	if (p2->mother == p1)
		return p2->gender == 'M' ? "parent - son" : "parent - daughter";
	return "";
}

string checkSiblingRelation(person* p1, person* p2)
{
	if (p1 != p2 &&
		p1->father == p2->father &&
		p1->mother == p2->mother &&
		((p1->father != nullptr && p1->father != virtualRoot) ||
			(p1->mother != nullptr && p1->mother != virtualRoot)))
	{
		return p1->gender == 'M' ? "brother - sibling" : "sister - sibling";
	}
	return "";
}

string checkNoRelation(person* p1, person* p2)
{
	if (p1 == p2)
	{
		return "same person";
	}
	if (checkParentChildRelation(p1, p2) != "" ||
		checkSpouseRelation(p1, p2) != "" ||
		checkSiblingRelation(p1, p2) != "" ||
		checkGrandparentRelation(p1, p2) != "" ||
		checkCousinRelation(p1, p2) != "")
	{
		return "";
	}
	return "no relation";
}

string checkGrandparentRelation(person* p1, person* p2)
{
	if ((p1->father && (p1->father->father == p2 || p1->father->mother == p2)) ||
		(p1->mother && (p1->mother->father == p2 || p1->mother->mother == p2)))
	{
		return "grandparent - grandchild";
	}
	if ((p2->father && (p2->father->father == p1 || p2->father->mother == p1)) ||
		(p2->mother && (p2->mother->father == p1 || p2->mother->mother == p1)))
	{
		return "grandparent - grandchild";
	}
	return "";
}
string checkCousinRelation(person* p1, person* p2)
{
	if ((p1->father == nullptr && p1->mother == nullptr) || (p2->father == nullptr && p2->mother == nullptr))
	{
		return "";
	}

	if (p1->father && p2->father && (p1->father->father == p2->father->father || p1->father->mother == p2->father->mother))
	{
		return "cousin - cousin";
	}
	if (p1->mother && p2->mother && (p1->mother->father == p2->mother->father || p1->mother->mother == p2->mother->mother))
	{
		return "cousin - cousin";
	}

	return "";
}

string findRelationship(const string& id1, const string& id2)
{
	person* p1 = findPerson(id1);
	person* p2 = findPerson(id2);

	if (p1 == nullptr || p2 == nullptr)
		return "Could not find either person.";

	using RelationCheck = string(*)(person*, person*);
	std::vector<RelationCheck> relationChecks = {
		checkParentChildRelation,
		checkSpouseRelation,
		checkSiblingRelation,
		checkGrandparentRelation,
		checkCousinRelation,
		checkNoRelation };

	for (RelationCheck check : relationChecks)
	{
		string relation = check(p1, p2);
		if (relation != "")
		{
			return p1->name + ": " + relation + ": " + p2->name;
		}
	}

	return p1->name + ": relative - relative: " + p2->name;
}

int person::getAge() const
{
	time_t now = time(0);
	tm current;

	localtime_s(&current, &now);

	int age = current.tm_year  - birthdate.tm_year;
	if (current.tm_mon < birthdate.tm_mon ||
		(current.tm_mon == birthdate.tm_mon && current.tm_mday < birthdate.tm_mday))
	{
		age--;
	}
	return age;
}

