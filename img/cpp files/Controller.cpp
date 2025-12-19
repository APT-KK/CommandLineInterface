#include "../headers/Controller.h"
#include "../headers/club.h"
#include "../headers/Member.h"
#include "../headers/student.h"
#include "../headers/Admin.h"
#include "../headers/AssingmentReviewer.h"
#include "../headers/vector.h"
#include "../headers/submission.h"
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
using namespace std;

// Controller-managed data will be used instead of globals.

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

// Find a student in this controller's students list
Student* Controller::findStudent(int rollNumber) {
    for (std::size_t i = 0; i < students.getSize(); ++i) {
        if (students.get(i)->getRoll() == rollNumber) return students.get(i);
    }
    return nullptr;
}

// The Controller::findClub method is implemented below and uses the
// Controller instance's `clubs` member.


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



// Helper: find a club within this controller
club* Controller::findClub(const string& clubName) {
    for (std::size_t i = 0; i < clubs.getSize(); ++i) {
        if (clubs.get(i)->getName() == clubName) return clubs.get(i);
    }
    return nullptr;
}

void Controller::runCLI() {
 
    cout << "\n=== Welcome to the Club Management System ===\n";

    while (true) {
        cout << "\n--- MAIN MENU ---\n";
        vector<string> options;
    // `current_user` is stored as a Controller member via `members`
    // but the code uses a single active user variable. We'll store it
    // as a local static pointer so the rest of the logic works the same.
    static Member* current_user = nullptr;

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
                    // add student to both students list and members list (for login/search)
                    students.push_back(s);
                    members.push_back(s);
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
                // search registered members (students) for credentials
                bool loggedIn = false;
                for (std::size_t i = 0; i < members.getSize(); ++i) {
                    // only students are allowed to login via roll/password
                    if (Student* s = dynamic_cast<Student*>(members.get(i))) {
                        if (s->login(roll, to_string(password))) {
                            current_user = s;
                            cout << "Login successful. Welcome, " << current_user->getName() << "!\n";
                            loggedIn = true;
                            break;
                        }
                    }
                }
                if (!loggedIn) cout << "Login failed. Please check your credentials.\n";
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
                if (this->findClub(clubName)) {
                    cout << "Club with this name already exists.\n";
                } else {
                        // Use the Student as the club admin (Member*)
            // create club and set admin
                        club* newClub = new club(clubName, s);
                        // push into this Controller's clubs list
                        clubs.push_back(newClub);

                        // add creator to members of the club (club::addMember already avoids duplicates)
                        newClub->addMember(s);
                        // also add the club to the student's clubs list so they can view it (avoid duplicates)
                        bool studentHasClub = false;
                        for (std::size_t k = 0; k < s->getClubs().getSize(); ++k) {
                            if (s->getClubs().get(k) == newClub) { studentHasClub = true; break; }
                        }
                        if (!studentHasClub) s->getClubs().push_back(newClub);
                        // ensure the student is registered in controller members list
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
    club* c = this->findClub(clubName);
    // trim whitespace
    while (!clubName.empty() && isspace((unsigned char)clubName.back())) clubName.pop_back();
    size_t start = 0; while (start < clubName.size() && isspace((unsigned char)clubName[start])) start++; if (start > 0) clubName = clubName.substr(start);
        if (c) {
            // check if already a member
            c->addMember(current_user);
            if (Student* s = dynamic_cast<Student*>(current_user)) {
                bool already = false;
                for (std::size_t i = 0; i < s->getClubs().getSize(); ++i) {
                    if (s->getClubs().get(i) == c) { already = true; break; }
                }
                if (!already) s->getClubs().push_back(c);
            }
            cout << "Joined club '" << clubName << "' successfully." << std::endl;
            } else {
            cout << "Club not found. Available clubs:" << std::endl;
            for (std::size_t i = 0; i < clubs.getSize(); ++i) cout << " - " << clubs.get(i)->getName() << std::endl;
        }
    } else if (choice == 3) {
        if (Student* s = dynamic_cast<Student*>(current_user)) {
            // Get student's clubs (assumes getClubs() returns Vector<club*>&)
            auto& myClubs = s->getClubs();
            if (myClubs.getSize() == 0) {
                cout << "You are not a member of any clubs.\n";
            } else {
                cout << "Your Clubs:\n";
                for (std::size_t i = 0; i < myClubs.getSize(); ++i) {
                    cout << "[" << (i + 1) << "] " << myClubs.get(i)->getName() << "\n";
                }

                // Prompt user to choose a club to open or 0 to go back
                int sel;
                while (true) {
                    cout << "Enter club number to open or 0 to go back: ";
                    if (!(cin >> sel)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. Please enter a number.\n";
                        continue;
                    }
                    if (sel < 0 || static_cast<std::size_t>(sel) > myClubs.getSize()) {
                        cout << "Please enter a number between 0 and " << myClubs.getSize() << ".\n";
                        continue;
                    }
                    break;
                }

                if (sel == 0) {
                    // go back to main menu
                } else {
                    club* chosen = myClubs.get(sel - 1);
                    // Expanded club menu with submit/add/check features
                    while (true) {
                        vector<string> clubOptions = {
                            "View Club Details",
                            "View all Assignments",
                            "Submit Assignment",
                            "View my Submissions",
                            "View a Member's Submissions",
                            "Promote Member to Assignment Reviewer (Admin)",
                            "Add Assignment (Assignment Reviewer)",
                            "Check submissions (Assignment Reviewer only)",
                            "Back"
                        };
                        display_menu(clubOptions, string("Club: ") + chosen->getName());
                        int cchoice = get_choice(clubOptions.size());
                        if (cchoice == 1) {
                            cout << "Club Name: " << chosen->getName() << "\n";
                            auto membersList = chosen->getMembers();
                            cout << "Members: " << membersList->getSize() << "\n";
                            cout << "Assignments: " << chosen->getAssignments()->getSize() << "\n";
                        } else if (cchoice == 2) {
                            auto assigns = chosen->getAssignments();
                            if (assigns->getSize() == 0) {
                                cout << "No assignments in this club.\n";
                            } else {
                                cout << "Assignments:\n";
                                for (std::size_t ai = 0; ai < assigns->getSize(); ++ai) {
                                    cout << "[" << (ai+1) << "] " << assigns->get(ai)->getTitle() << " (Max: " << assigns->get(ai)->getMaxScore() << ", Deadline: " << assigns->get(ai)->getDeadline() << ")\n";
                                }
                            }
                        } else if (cchoice == 3) {
                            // Submit Assignment (from inside the club view)
                            if (Student* s = dynamic_cast<Student*>(current_user)) {
                                auto myClubs = s->getClubs();
                                bool found = false;
                                for (std::size_t k = 0; k < myClubs.getSize(); ++k) if (myClubs.get(k) == chosen) { found = true; break; }
                                if (!found) { cout << "You are not a member of this club.\n"; }
                                else {
                                    auto assigns = chosen->getAssignments();
                                    if (assigns->getSize() == 0) { cout << "No assignments in this club.\n"; }
                                    else {
                                        cout << "Assignments:\n";
                                        for (std::size_t ai = 0; ai < assigns->getSize(); ++ai) cout << "[" << (ai+1) << "] " << assigns->get(ai)->getTitle() << "\n";
                                        cout << "Enter assignment number to submit: "; int asel; cin >> asel; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                        if (asel <= 0 || static_cast<std::size_t>(asel) > assigns->getSize()) { cout << "Invalid assignment selection.\n"; }
                                        else {
                                            string date; char latec;
                                            cout << "Enter submission date: "; getline(cin, date);
                                            cout << "Is it late? (y/n): "; cin >> latec; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                            bool late = (latec == 'y' || latec == 'Y');
                                            s->submitAssignment(assigns->get(asel-1), date, late);
                                            cout << "Submission recorded.\n";
                                        }
                                    }
                                }
                            } else {
                                cout << "Only Students can submit assignments.\n";
                            }
                        } else if (cchoice == 4) {
                            // View my submissions
                            if (Student* s = dynamic_cast<Student*>(current_user)) {
                                auto myClubs = s->getClubs();
                                bool found = false;
                                for (std::size_t k = 0; k < myClubs.getSize(); ++k) if (myClubs.get(k) == chosen) { found = true; break; }
                                if (!found) { cout << "You are not a member of this club.\n"; }
                                else {
                                    auto assigns = chosen->getAssignments();
                                    for (std::size_t ai = 0; ai < assigns->getSize(); ++ai) {
                                        auto& subs = assigns->get(ai)->getSubmissions();
                                        bool printedAny = false;
                                        for (std::size_t si = 0; si < subs.getSize(); ++si) {
                                            if (subs.get(si)->getStudent() == s) {
                                                if (!printedAny) { cout << "Submissions for assignment [" << (ai+1) << "] " << assigns->get(ai)->getTitle() << ":\n"; printedAny = true; }
                                                cout << "  - Date: " << subs.get(si)->getDate() << ", Score: " << subs.get(si)->getScore() << (subs.get(si)->isLate() ? " (late)" : "") << "\n";
                                            }
                                        }
                                        if (!printedAny) cout << "No submission for assignment [" << (ai+1) << "] " << assigns->get(ai)->getTitle() << "\n";
                                    }
                                }
                            } else {
                                cout << "Only Students have submissions.\n";
                            }
                        } else if (cchoice == 5) {
                            // View a member's submissions
                            auto mems = chosen->getMembers();
                            if (mems->getSize() == 0) { cout << "No members in this club.\n"; }
                            else {
                                cout << "Club Members:\n";
                                for (std::size_t mi = 0; mi < mems->getSize(); ++mi) cout << "[" << (mi+1) << "] " << mems->get(mi)->getName() << "\n";
                                cout << "Enter member number to view their submissions: ";
                                int msel; cin >> msel; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                if (msel <= 0 || static_cast<std::size_t>(msel) > mems->getSize()) { cout << "Invalid member selection.\n"; }
                                else {
                                    Member* target = mems->get(msel-1);
                                    Student* st = dynamic_cast<Student*>(target);
                                    if (!st) { cout << "Selected member is not a student (no submissions).\n"; }
                                    else {
                                        auto assigns = chosen->getAssignments();
                                        for (std::size_t ai = 0; ai < assigns->getSize(); ++ai) {
                                            auto& subs = assigns->get(ai)->getSubmissions();
                                            bool printed = false;
                                            for (std::size_t si = 0; si < subs.getSize(); ++si) {
                                                if (subs.get(si)->getStudent() == st) {
                                                    if (!printed) { cout << "Submissions for assignment [" << (ai+1) << "] " << assigns->get(ai)->getTitle() << ":\n"; printed = true; }
                                                    cout << "  - Date: " << subs.get(si)->getDate() << ", Score: " << subs.get(si)->getScore() << (subs.get(si)->isLate() ? " (late)" : "") << "\n";
                                                }
                                            }
                                            if (!printed) cout << "No submission for assignment [" << (ai+1) << "] " << assigns->get(ai)->getTitle() << "\n";
                                        }
                                    }
                                }
                            }
                        } else if (cchoice == 6) {
                            // Promote a member to Assignment Reviewer (club creator only)
                            auto mems = chosen->getMembers();
                            if (mems->getSize() == 0) {
                                cout << "No members in this club.\n";
                            } else if (mems->get(0) != current_user) {
                                cout << "Only the club admin (creator) can promote members to Assignment Reviewer.\n";
                            } else {
                                cout << "Club Members:\n";
                                for (std::size_t mi = 0; mi < mems->getSize(); ++mi) cout << "[" << (mi+1) << "] " << mems->get(mi)->getName() << " (roll " << mems->get(mi)->getRoll() << ")\n";
                                cout << "Enter member number to promote: ";
                                int msel; cin >> msel; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                if (msel <= 0 || static_cast<std::size_t>(msel) > mems->getSize()) { cout << "Invalid selection.\n"; }
                                else {
                                    Member* target = mems->get(msel-1);
                                    promoteToReviewer(target->getRoll());
                                }
                            }
                        } else if (cchoice == 7) {
                            // Add Assignment (permission: roll recorded in assignmentReviewers)
                            if (!current_user) {
                                cout << "No user logged in.\n";
                            } else {
                                bool isReviewer = false;
                                for (std::size_t ri = 0; ri < assignmentReviewers.getSize(); ++ri) {
                                    if (assignmentReviewers.get(ri) == current_user->getRoll()) { isReviewer = true; break; }
                                }
                                if (isReviewer) {
                                    string title; int maxScore; string deadline;
                                    cout << "Enter assignment title: "; cin.ignore(numeric_limits<streamsize>::max(), '\n'); getline(cin, title);
                                    cout << "Enter max score: "; cin >> maxScore; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    cout << "Enter deadline (string): "; getline(cin, deadline);
                                    Assignment* a = new Assignment(title, maxScore, deadline);
                                    chosen->addAssignment(a);
                                    cout << "Assignment '" << title << "' added to club '" << chosen->getName() << "'.\n";
                                } else {
                                    cout << "Only Assignment Reviewers can add assignments.\n";
                                }
                            }
                        } else if (cchoice == 8) {
                            // Check submissions (permission: roll recorded in assignmentReviewers)
                            if (!current_user) {
                                cout << "No user logged in.\n";
                            } else {
                                bool isReviewer = false;
                                for (std::size_t ri = 0; ri < assignmentReviewers.getSize(); ++ri) {
                                    if (assignmentReviewers.get(ri) == current_user->getRoll()) { isReviewer = true; break; }
                                }
                                if (isReviewer) {
                                    auto assigns = chosen->getAssignments();
                                    if (assigns->getSize() == 0) { cout << "No assignments to check.\n"; }
                                    else {
                                        cout << "Assignments:\n";
                                        for (std::size_t ai = 0; ai < assigns->getSize(); ++ai) cout << "[" << (ai+1) << "] " << assigns->get(ai)->getTitle() << "\n";
                                        cout << "Enter assignment number to check: ";
                                        int asel; cin >> asel; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                        if (asel <= 0 || static_cast<std::size_t>(asel) > assigns->getSize()) { cout << "Invalid assignment selection.\n"; }
                                        else {
                                            auto& subs = assigns->get(asel-1)->getSubmissions();
                                            if (subs.getSize() == 0) { cout << "No submissions for this assignment.\n"; }
                                            else {
                                                cout << "Submissions for assignment '" << assigns->get(asel-1)->getTitle() << "':\n";
                                                for (std::size_t si = 0; si < subs.getSize(); ++si) {
                                                    auto ssub = subs.get(si);
                                                    cout << "[" << (si+1) << "] Student: " << ssub->getStudent()->getName() << ", Date: " << ssub->getDate() << ", Score: " << ssub->getScore() << (ssub->isLate() ? " (late)" : "") << "\n";
                                                }
                                                cout << "Enter submission number to set score (or 0 to cancel): ";
                                                int ssel; cin >> ssel; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                                if (ssel <= 0 || static_cast<std::size_t>(ssel) > subs.getSize()) { cout << "Cancelled or invalid selection.\n"; }
                                                else {
                                                    cout << "Enter score (0-" << assigns->get(asel-1)->getMaxScore() << "): ";
                                                    int score; cin >> score; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                                    subs.get(ssel-1)->setScore(score);
                                                    cout << "Score updated.\n";
                                                }
                                            }
                                        }
                                    }
                                } else {
                                    cout << "Only Assignment Reviewers can check submissions.\n";
                                }
                            }
                        } else if (cchoice == 9) {
                            break; // back to student's club list / main menu
                        }
                    }
                }
            }
        } else {
            cout << "Only Students can view their clubs.\n";
        }
    } else if (choice == 3) {
        if (clubs.empty()) {
            cout << "No clubs available.\n";
        } else {
            cout << "Available Clubs:\n";
            for (std::size_t i = 0; i < clubs.getSize(); ++i) {
                cout << "- " << clubs.get(i)->getName() << "\n";
            }
        }
    }   else if (choice == 4) {
        cout << "Logging out " << current_user->getName() << ".\n";
        current_user = nullptr;
    } else if (choice == 5) {
        cout << "Exiting the system. Goodbye!\n";
        break;
    }
  }
 }

}

void Controller::promoteToReviewer(int roll) {
    // Ensure member exists
    Member* target = nullptr;
    for (std::size_t i = 0; i < members.getSize(); ++i) if (members.get(i)->getRoll() == roll) { target = members.get(i); break; }
    if (!target) { cout << "Member with roll " << roll << " not found.\n"; return; }
    // avoid duplicates
    for (std::size_t i = 0; i < assignmentReviewers.getSize(); ++i) if (assignmentReviewers.get(i) == roll) { cout << "Member is already an Assignment Reviewer.\n"; return; }
    assignmentReviewers.push_back(roll);
    cout << "Member roll " << roll << " promoted to Assignment Reviewer.\n";
}