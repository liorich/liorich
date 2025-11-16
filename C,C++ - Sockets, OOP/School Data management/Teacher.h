#ifndef TEACHER_H
#define TEACHER_H
#include <iostream>
using namespace std;
#include "Worker.h"

class Teacher: public Worker{
protected:
    int Number_of_Subjects;
    vector<string> Subjects;
public:
    Teacher();
    Teacher(string pname, string fname, int years);
    virtual ~Teacher();
    void setTeacherSubjects();
    int getNumberofSubjects();
    virtual double returnSalary();
    virtual bool isExcellent();
    void printTeacherSubjects();
    virtual void printData();
};
#endif