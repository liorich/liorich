#include "Person.h"

Person::Person() {}

// Pname = Private name, Fname = Family Name
Person::Person(string pname, string fname) {
    P_name=pname;
    F_name=fname;
}
Person::~Person(){
    //cout << "Person Dtor Called" << endl;
}

string Person::getName(char type) {
    if(type=='P' || type=='p'){ return P_name; }
    if(type=='F' || type=='f'){ return F_name; }
    return "";
}

void Person::setName(const string& pname, const string& fname) {
    P_name = pname;
    F_name = fname;
}

void Person::printData() { // Virtual
    cout << "Name: "<< P_name << " " << F_name;
}

bool Person::isExcellent() { // Virtual
    cout << "Called for Person::isExcellent! Returning False" << endl;
    return false;
}
double Person::returnSalary() { // Virtual
    cout << "Called for Person::returnSalary! Returning 0" << endl;
    return 0;
}