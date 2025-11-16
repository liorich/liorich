#include <iostream>
using namespace std;
#include <vector>
#include "Tutor.h"
#include "Secretary.h"
#include "Layer.h"
#include "VecAnalyser.h"

class School{
    static School* school;
    vector<Person*> Workers;
    vector<Pupil*> Pupils;
    int Num_of_Layers=0;
    vector<Layer*> Layer_Vector;
    School();
public:
    ~School();
    static School * getSchool();
    void menu();
    void printMenu();
    Layer* returnLayer(char charLayer);
    bool hasPerson(string pname, string fname); // Check if the same person exists, by name
};