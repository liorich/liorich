#ifndef TUTOR_H
#define TUTOR_H
#include <iostream>
using namespace std;
#include "Teacher.h"
#include "Class.h"

class Class;

class Tutor: public Teacher{
protected:
    Class * TutorClass;
public:
    Tutor();
    Tutor(string pname, string fname, int years);
    ~Tutor();
    void setClass(Class * newClass);
    double returnSalary();
    bool isExcellent();
    void printData();
};

#endif


