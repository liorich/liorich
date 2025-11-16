#include <iostream>
using namespace std;
#include <cstring>
#include "AdministrationPersonal.h"

class Secretary: public AdministrationPersonal{
    int numberOfChildrens;
public:
    Secretary();
    Secretary(string pname, string fname, int years, int Children);
    ~Secretary();
    double returnSalary();
    bool isExcellent();
    void printData();
};