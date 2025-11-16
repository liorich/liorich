#include "Manager.h"

Manager * Manager::manager=NULL;
Manager::Manager() {
}
Manager::Manager(string pname, string fname, int TeacherYears, int ManagerYears) {
    setName(pname,fname);
    Teacher::setYears(TeacherYears);
    AdministrationPersonal::setYears(ManagerYears);
}
Manager::~Manager() {
    //cout << "Manager Dtor Called" << endl;
    if (!manager) delete manager;
}
Manager * Manager::getManager() { // "Static constructor" : If a manager exists, returns current one
    if (!manager)
        manager = new Manager;
    return manager;
}
bool Manager::hasManager() { // Checks if said manager already exists
    if (!manager) return false;
    return true;
}

double Manager::returnSalary() {
    double ManagerSalary=AdministrationPersonal::Basis*2+AdministrationPersonal::getYears()*500;
    if(Teacher::Number_of_Subjects==0) return ManagerSalary;
    return Teacher::returnSalary()+ManagerSalary;
}
bool Manager::isExcellent() {
    if(AdministrationPersonal::getYears() >3 ) return true;
    return false;
}
void Manager::printData() {
    Person::printData();
    cout << ", Salary: " << returnSalary() << ", Years as a manager: " << AdministrationPersonal::getYears() << ", ";
    if(isExcellent()){ //If Manager is exceptional
        cout << "Exceptional Manager" << endl;
    }
    else cout << "Not Exceptional Manager" << endl;
    if(Teacher::Number_of_Subjects==0) return; // If this manager does not teach, don't print his teacher data.
    cout << "Years as a teacher: " << Teacher::getYears() << ", ";
    Teacher::printTeacherSubjects();
    if(Teacher::isExcellent()){ //If Teacher is exceptional
        cout << "Exceptional Teacher" << endl;
    }
    else cout << "Not Exceptional Teacher" << endl;
}