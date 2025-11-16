#include "Teacher.h"

Teacher::Teacher() {
    Number_of_Subjects=0;
}
Teacher::Teacher(string pname, string fname, int years) {
    setName(pname,fname);
    setYears(years);
    Number_of_Subjects=0;
}
Teacher::~Teacher() {
    Subjects.clear();
    //cout << "Teacher Dtor Called" << endl;
}
int Teacher::getNumberofSubjects() {
    return Number_of_Subjects;
}
void Teacher::setTeacherSubjects() {
    string tmp;
    cout << "Insert the teacher's subjects. To finish, enter -1." << endl;
    while(tmp!="-1"){
        std::cin >> tmp;
        if(tmp == "-1") break;
        Subjects.push_back(tmp);
        Number_of_Subjects++;
        cout << "Subject added." << endl;
    }
}
double Teacher::returnSalary() { // Virtual
    double Salary;
    Salary=Basis*(1+((double)Number_of_Subjects)/10)+300*Worker_Years;
    return Salary;
}
bool Teacher::isExcellent() { // Virtual
    if(Number_of_Subjects>=5) return true;
    else return false;
}
void Teacher::printTeacherSubjects() {
    // Prints all the teacher's subjects in one line.
    if(Subjects.empty()) return;
    cout << "Subjects: ";
    for (auto it = Subjects.begin(); it != Subjects.end(); it++) {
        cout << *it << ", ";
    }
    cout << endl;
}
void Teacher::printData() { // Virtual
    Person::printData(); // Full name, Number of working years
    cout << ", Number of Teaching years: " << getYears() << ", ";
    printTeacherSubjects(); // Subjects
    if(isExcellent()){ //If Teacher is exceptional
        cout << "Exceptional Teacher, ";
    }
    else cout << "Not Exceptional Teacher, ";
    cout << "Salary: " << returnSalary() << endl;
}
