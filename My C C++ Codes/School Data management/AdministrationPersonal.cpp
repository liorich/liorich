#include "AdministrationPersonal.h"

AdministrationPersonal::AdministrationPersonal() {
    OfficeNumber="-1";
}
AdministrationPersonal::AdministrationPersonal(string OfficeNum) {
    OfficeNumber=OfficeNum;
}
AdministrationPersonal::~AdministrationPersonal() {
    //cout << "AdministrationPersonal Dtor Called" << endl;
}
void AdministrationPersonal::setOfficeNumber(string OfficeNum) {
    OfficeNumber=OfficeNum;
}

string AdministrationPersonal::getOfficeNumber() {
    return OfficeNumber;
}
double AdministrationPersonal::returnSalary() { // Virtual
    cout << "Called for AdministrationPersonal::returnSalary! Returning 0" << endl;
    return 0;
}
bool AdministrationPersonal::isExcellent() { // Virtual
    cout << "Called for AdministrationPersonal::isExcellent! Returning False" << endl;
    return false;
}
void AdministrationPersonal::printData() { // Virtual
    cout << "Called for AdministrationPersonal::PrintData!" << endl;
}
