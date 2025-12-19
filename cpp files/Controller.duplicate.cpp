#include "controller.h"
#include "club.h"
#include "Member.h"
#include "student.h"
#include "Admin.h"
#include "vector.h"
#include <iostream>
#include <string>
#include <vector>
#include <limits>
using namespace std;

    Vector<Student*> students;
    Vector<club*> clubs;
    Member* current_user = nullptr;

    //getchoice_function
    int get_choice(int num_choices) {
        while (true) {
            cout << "Enter choice: ";
            int choice;
            cin >> choice;
            if(cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Please enter a number." << endl;
                continue;
            }
            if (choice >= 1 && choice <= num_choices) {
                return choice;
            }
            cout << "Invalid choice! Please enter a number between 1 and " << num_choices << "." << endl;
        }
    }

    //find student function
    Student* findStudent(int rollNumber){
        for(int i=0 ; i < students.getSize(); i++){
            if(students.get(i)->getRoll() == rollNumber){
                return students.get(i);

            }
        }
            return nullptr;
    }

    //find club function
    club* findClub(const string& clubName){
        for(int i=0 ; i < clubs.getSize(); i++){
            if(clubs.get(i)->getName() == clubName){
                return clubs.get(i);
            }
        }
        return nullptr;
    }


    //display menu
    void display_menu(const vector<string>& options, const string& header="Choose an option:") {
        cout << "\n" << string(header.size(), '=') << endl;
        cout << header << endl;
        cout << string(header.size(), '=') << endl;
        for (size_t i = 0; i < options.size(); ++i) {
            cout << "[" << (i+1) << "] " << options[i] << endl;
        }
        cout << endl;
    }



void Controller::runCLI() {
 
    cout << "\n=== Welcome to the Club Management System ===\n";

    while (true) {
        cout << "\n--- MAIN MENU ---\n";
        vector<string> options;
        if(!current_user){
            options = {
                "Register as Member",
                "Login as Member",
                "Exit"
            };
        } else {
            options = {
                "Create a Club (Admin)",
                "Join a Club",
                "View my Clubs",
                "List all Clubs",
                // "Add Assignment (Assignment Reviewer)",
                // "View Assignments",
                // "Submit Assignment",
                "Logout",
                "Exit"
            };
        }
        display_menu(options);
        int choice = get_choice(options.size());

        if(!current_user){
            if(choice == 1){
                int roll;
                string name;
                int password;
                cout << "Enter roll number: ";
                cin >> roll;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter name: ";
                getline(cin, name);
                cout << "Enter password: ";
                cin >> password;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                try{
                    Student* s = new Student(roll, name, to_string(password));
                    students.push_back(s);
                    cout << "Student registered successfully.\n";
                } catch (exception& e){
                    cout << "Error registering student. Please try again.\n";
                }

            } else if(choice == 2){
                int roll;
                int password;
                cout << "Enter roll number: ";
                cin >> roll;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter password: ";
                cin >> password;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                for(int i=0; i<students.getSize(); i++){
                    if(students.get(i)->login(roll, to_string(password))){
                        current_user = students.get(i);
                        cout << "Login successful. Welcome, " << current_user->getName() << "!\n";
                        break;
                    } else {
                        cout << "Login failed. Please check your credentials.\n";
                    }
                }
            } else if (choice == 3) {
                cout << "Exiting the system. Goodbye!\n";
                break;
            } 
  } else {
    if (choice == 1) {
            // Allow Students to create a club and become its Admin
            if (Student* s = dynamic_cast<Student*>(current_user)) {
                string clubName;
                cout << "Enter club name: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, clubName);
                if (findClub(clubName)) {
                    cout << "Club with this name already exists.\n";
                } else {
                        // Use the Student as the club admin (Member*)
                                std::cout << "(debug) student ptr: " << s << std::endl;
                                std::cout << "(debug) student clubs size before: " << s->getClubs().getSize() << std::endl;
                        club* newClub = new club(clubName, s);
                        clubs.push_back(newClub);
                                std::cout << "(debug) student clubs size after: " << s->getClubs().getSize() << std::endl;
                                std::cout << "(debug) newClub ptr: " << newClub << std::endl;
                        // add creator to members of the club
                        newClub->addMember(s);
                        // also add the club to the student's clubs list so they can view it
                        s->getClubs().push_back(newClub);
                        cout << "(debug) student's clubs size after add: " << s->getClubs().getSize() << "\n";
                        // also register the student in global members list if not already
                        members.push_back(s);
                        cout << "Club '" << clubName << "' created successfully and " << s->getName() << " is now the admin.\n";
                }
            } else {
                cout << "Only logged-in Students can create clubs.\n";
            }
    } else if (choice == 2) {
        string clubName;
        cout << "Enter club name to join: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, clubName);
        // trim whitespace
        while (!clubName.empty() && isspace((unsigned char)clubName.back())) clubName.pop_back();
        size_t start = 0; while (start < clubName.size() && isspace((unsigned char)clubName[start])) start++; if (start > 0) clubName = clubName.substr(start);
        club* c = findClub(clubName);
        if (c) {
            // check if already a member
            c->addMember(current_user);
            if (Student* s = dynamic_cast<Student*>(current_user)) {
                s->getClubs().push_back(c);
            }
            cout << "Joined club '" << clubName << "' successfully." << std::endl;
        } else {
            cout << "Club not found. Available clubs:" << std::endl;
            for (int i = 0; i < clubs.getSize(); i++) cout << " - " << clubs.get(i)->getName() << std::endl;
        }
    } else if (choice == 3) {
        if (Student* s = dynamic_cast<Student*>(current_user)) {
            s->listAllClubs();
        } else {
            cout << "Only Students can view their clubs.\n";
        }
    } else if (choice == 4) {
        if (clubs.empty()) {
            cout << "No clubs available.\n";
        } else {
            cout << "Available Clubs:\n";
            for (int i = 0; i < clubs.getSize(); i++) {
                cout << "- " << clubs.get(i)->getName() << "\n";
            }
        }
    } else if (choice == 5) {
        cout << "Logging out " << current_user->getName() << ".\n";
        current_user = nullptr;
    } else if (choice == 6) {
        cout << "Exiting the system. Goodbye!\n";
        break;
    }
  }
 }

}