#include "Tutor.h"

Tutor::Tutor() { // Placeholder
    TutorClass=NULL;
}
Tutor::Tutor(string pname, string fname, int years) {
    setName(pname,fname);
    setYears(years);
    TutorClass=NULL;
}
Tutor::~Tutor() {
    //cout << "Tutor Dtor Called" << endl;
}
void Tutor::setClass(Class *newClass) {
    // Set this tutor's class.
    TutorClass=newClass;
}
double Tutor::returnSalary() {
    return 1000+Teacher::returnSalary();
}
bool Tutor::isExcellent() {
    if(TutorClass==NULL) return false; // If class is empty, return false
    double Average=0; // Number of Exceptional Pupils compared to everyone in the class.
    vector<Pupil> *Class=TutorClass->getPupils();
    int NumberOfStudents=TutorClass->getNumberOfStudents(); // Get number of Pupils
    for (auto it = Class->begin(); it != Class->end(); it++) { // Check if at least half of them are exceptional.
        if((*it).isExcellent()) Average++;
    }
    if(Average==0 || NumberOfStudents==0 ) return false;
    Average = Average / (double)NumberOfStudents;
    if(Average >= 0.5) return true;
    return false;
}
void Tutor::printData() {
    Worker::printData();
    if(isExcellent()){ //If Tutor is exceptional
        cout << ", Exceptional Tutor, ";
    }
    else cout << ", Not Exceptional Tutor, ";
    cout << "Salary: " << returnSalary() << endl;
    printTeacherSubjects();
    if(TutorClass==NULL) return;
    cout << "Class: " << TutorClass->getClassNumber() << "-" << TutorClass->getClassLayer() << endl;
    vector<Pupil> *Class=TutorClass->getPupils();
    cout << "Pupils: ";
    for (auto it = Class->begin(); it != Class->end(); it++) {
        (*it).Pupil::printData();
    }
    cout << endl;
}