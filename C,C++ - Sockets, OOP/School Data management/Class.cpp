#include "Class.h"

Class::Class(){
    ClassTutor=NULL;
    ClassLayer='0';
    ClassNumber=0;
    NumberOfStudents=0;
}
Class::Class(char Layer, int Number) {
    ClassLayer=Layer; ClassNumber=Number;
    NumberOfStudents=0;
    ClassTutor=NULL;
}

Class::~Class() {
    //cout << "Class Dtor Called" << endl;
}
bool Class::hasTutor() { // Checks for existing Tutor
    if(ClassTutor==NULL) return false;
    return true;
}
vector<Pupil> * Class::getPupils() {
    return &Class_Students;
}
void Class::setClassDetails(char Layer, int Number) {
    ClassLayer=Layer; ClassNumber=Number;
}
// Get class details
char Class::getClassLayer() { return ClassLayer; }
int Class::getClassNumber() { return ClassNumber; }
int Class::getNumberOfStudents() { return NumberOfStudents; }

void Class::addPupil(const Pupil &newPupil) {
    NumberOfStudents++;
    Class_Students.push_back(newPupil);
}
void Class::setClassTutor(Tutor *newTutor) {
    ClassTutor=newTutor;
}
Tutor * Class::getClassTutor() {
    return ClassTutor;
}

Pupil Class::returnPupil(int index) {
    if(NumberOfStudents==0){
        cout << "This class is empty. Returning empty Pupil profile." << endl;
        Pupil tmp;
        return tmp;
    }
    while(true){
        if(index<0 || index>NumberOfStudents-1) {
            cout << "Wrong input, out of range. Insert an index between 0 and " << NumberOfStudents << "." << endl;
            cin.clear();
            cin.ignore();
            cin >> index;
        }
        else break;
    }
    return Class_Students[index];
}
