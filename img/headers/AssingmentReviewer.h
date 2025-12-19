#ifndef ASSINGMENTREVIEWER_H
#define ASSINGMENTREVIEWER_H

#include "Member.h"
#include <string>
using namespace std;

class club;   
class Student; 

class AssingmentReviewer : public Member {
    public:
        AssingmentReviewer(string n, int r, string p);
        // allow external code (Controller) to create assignments via the reviewer
        void createAssignment(class club* c, string title, int maxScore, string deadline);
    
};

#endif

