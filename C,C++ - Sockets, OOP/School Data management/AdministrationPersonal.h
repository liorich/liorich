#ifndef ADMINISTRATIONPERSONAL_H
#define ADMINISTRATIONPERSONAL_H
#include <iostream>
using namespace std;
#include <cstring>
#include "Worker.h"

class AdministrationPersonal: public Worker{
    string OfficeNumber;
public:
    AdministrationPersonal();
    AdministrationPersonal(string OfficeNum);
    void setOfficeNumber(string OfficeNum);
    string getOfficeNumber();
    virtual ~AdministrationPersonal();
    virtual double returnSalary();
    virtual bool isExcellent();
    virtual void printData();
};
#endif
