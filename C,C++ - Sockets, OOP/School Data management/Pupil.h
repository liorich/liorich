#ifndef PUPIL_H
#define PUPIL_H
#include <iostream>
using namespace std;
#include "Person.h"
#include <vector>

class Pupil: public Person{
    vector<int> Grades;
    char Layer;
    int Class;
    double avg=0;
public:
    Pupil();
    Pupil(string pname, string fname, char newLayer, int newClass);
    virtual ~Pupil();
    void setGrades(); // Set grades by input
    bool isExcellent(); // checks if the pupil is excellent according to his type
    void printData();
    double returnSalary();
};
#endif