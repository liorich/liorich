#include "Secretary.h"

Secretary::Secretary() { numberOfChildrens=0; }
Secretary::Secretary(string pname, string fname, int years, int Children) {
    setName(pname,fname);
    setYears(years);
    numberOfChildrens=Children;
}
Secretary::~Secretary() { //cout << "Secretary Dtor Called" << endl;
    }
double Secretary::returnSalary() {
    return Basis+numberOfChildrens*200;
}
bool Secretary::isExcellent() {
    if(Worker_Years > 10) return true;
    return false;
}
void Secretary::printData() {
    Worker::printData();
    if(isExcellent()){ //If Secretary is exceptional
        cout << ", Exceptional Secretary, ";
    }
    else cout << ", Not Exceptional Secretary, ";
    cout << "Salary: " << returnSalary() << endl;
    cout << "Number of children studying in this school: " << numberOfChildrens;
    cout << ", Office Number: " << getOfficeNumber() << endl;
}