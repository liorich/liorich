#ifndef PERSON_H
#define PERSON_H
#include <iostream>
using namespace std;
#include "cstring"
#include "vector"

class Person{
    string P_name;
    string F_name;
public:
    Person();
    Person(string pname, string fname);
    virtual ~Person();
    string getName(char type); // get Private or Family Name
    void setName(const string& pname, const string& fname); // set Private or Family Name
    virtual void printData();
    virtual bool isExcellent(); // checks if the person is excellent according to his type
    virtual double returnSalary();
};
#endif