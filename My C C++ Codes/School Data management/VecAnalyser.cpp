#include "VecAnalyser.h"

void VecAnalyser::getVector(vector<Person *> newVector) {
    Vector=newVector;
}

Person * VecAnalyser::returnElement(int index) { // returns the element at [index].
    if(index > Vector.size()) {
        cout << "Out of bounds. Insert an index between 0-" << Vector.size() << ". Inserted:" << index << endl;
        return NULL;
    }
    return Vector[index];
}
void VecAnalyser::vectorSwap(int index1, int index2) { // swaps the place of two elements at [index].
    if(index1 > Vector.size() || index2 > Vector.size()) {
        cout << "Out of bounds. Insert an index between 0-" << Vector.size() << ". Inserted:" << index1 << "," << index2 << endl;
        return;
    }
    swap(Vector[index1],Vector[index2]);
}
void VecAnalyser::printElement(int index) { // print the element at [index]. Use upcasting.
    if(index > Vector.size()) {
        cout << "Out of bounds. Insert an index between 0-" << Vector.size() << ". Inserted:" << index << endl;
        return;
    }
    Vector[index]->printData();
}
void VecAnalyser::printAll() { // print all the data using upcasting.
    for (auto it = Vector.begin(); it != Vector.end(); it++) {
        (*it)->printData();
    }
}
void VecAnalyser::printMax() {
    // Print the person with the highest score: Pupils -> Highest average, Workers -> Highest salary.
    Person* BestPerson;
    double Score=0;
    for (auto it = Vector.begin(); it != Vector.end(); it++) {
        if((*it)->returnSalary()>Score){ // If a greater person was found
            Score=(*it)->returnSalary();
            BestPerson=*it;
        }
    }
    cout << "Best Person: " << endl;
    BestPerson->printData();
}
bool VecAnalyser::RTTI(int index) { // Looks for manager in the vector.
    if(index > Vector.size()) {
        cout << "Out of bounds. Insert an index between 0-" << Vector.size() << ". Inserted:" << index << endl;
        return false;
    }
    if(typeid(*Vector[index])==typeid(Manager)){
        cout << "Found Manager" << endl;
        return true;
    }
    return false;
}