#ifndef CLUB_H
#define CLUB_H

#include <string>
#include "Member.h"
#include "Assignment.h"
#include "Vector.h"

class Member;
class Admin;
class Assignment;

class club {
    private:
        std::string Clubname;
    Member* admin;
        Vector<Member*> members;
        Vector<Assignment*> assignments;

    public:
    club(std::string name, Member* a);

        std::string getName() const { return Clubname; }
        Vector<Assignment*>* getAssignments() { return &assignments; }
    Vector<Member*>* getMembers() { return &members; }

        void listallmembers();
        void addMember(Member* m);
        void removeMember(Member* m);
        void addAssignment(Assignment* a);
        void listallassignments();
};

#endif
