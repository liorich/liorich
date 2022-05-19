#ifndef WORKER_H
#define WORKER_H
#include <iostream>
using namespace std;
#include "Person.h"

class Worker: virtual public Person{
protected:
    int Worker_Years;
    int Basis;
public:
    Worker();
    Worker(int years);
    virtual ~Worker();
    void setYears(int years);
    int getYears();
    virtual double returnSalary();
    virtual void printData();
};
#endif