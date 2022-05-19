#include <iostream>
using namespace std;
#include <vector>
#include "Pupil.h"
#include "Manager.h"

class VecAnalyser{
    vector<Person*> Vector;
public:
    void getVector(vector<Person*> newVector);
    Person* returnElement(int index);
    void vectorSwap(int index1, int index2);
    void printElement(int index);
    void printAll();
    void printMax();
    bool RTTI(int index);
};
