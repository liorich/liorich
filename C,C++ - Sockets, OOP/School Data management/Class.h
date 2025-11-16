#ifndef CLASS_H
#define CLASS_H
#include <iostream>
using namespace std;
#include "cstring"
#include "vector"
#include "Pupil.h"
#include "Tutor.h"

class Tutor;

class Class {
    char ClassLayer;
    int ClassNumber;
    int NumberOfStudents;
    vector<Pupil> Class_Students;
    Tutor * ClassTutor;
public:
    Class();
    Class(char Layer, int Number);
    ~Class();
    vector<Pupil> * getPupils();
    void setClassTutor(Tutor * newTutor);
    Tutor * getClassTutor();
    bool hasTutor();
    void setClassDetails(char Layer, int Number);
    char getClassLayer();
    int getClassNumber();
    int getNumberOfStudents();
    void addPupil(const Pupil &newPupil);
    Pupil returnPupil(int index);
};
#endif