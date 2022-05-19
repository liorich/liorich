#include "Worker.h"

Worker::Worker() {
    Worker_Years=0;
    Basis=25;
}
Worker::Worker(int years) {
    Worker_Years=years;
    Basis=25;
}
Worker::~Worker() {
    //cout << "Worker Dtor Called" << endl;
}
void Worker::setYears(int years) {
    Worker_Years=years;
}
int Worker::getYears() {
    return Worker_Years;
}
double Worker::returnSalary() { // Virtual
    cout << "Called for Worker::returnSalary! Returning 25" << endl;
    return 0;
}
void Worker::printData() { // Virtual
    Person::printData();
    cout << ", Worker years: " << Worker_Years;
}