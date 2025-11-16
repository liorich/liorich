#include "Pupil.h"

Pupil::Pupil(){}

Pupil::Pupil(string pname, string fname, char newLayer, int newClass) {
    setName(pname,fname);
    Layer=newLayer;
    Class=newClass;
}

Pupil::~Pupil(){
    Grades.clear();
    //cout << "Pupil Dtor Called" << endl;
}

void Pupil::setGrades() {
    cout << "Insert the student's grades, one by one. To finish, enter -1." << endl;
    int tmp=0;
    while(tmp!=-1){
        std::cin >> tmp;
        if(tmp == -1) break;
        if(tmp < 0 || tmp > 100){
            cout << "Invalid grade. Enter a grade between 0 and 100." << endl;
        }
        else {
            Grades.push_back(tmp);
            cout << "Grade added." << endl;
            cin.clear();
            cin.ignore();
        }
    }
}
bool Pupil::isExcellent() {
    // Check if the pupil is exceptional according the the rules
    bool x=true;
    double sum=0;
    double number_of_grades=0;
    for (auto it = Grades.begin(); it != Grades.end(); it++) {
        if(*it < 65) x=false;
        sum+=*it;
        number_of_grades++;
    }
    if(number_of_grades==0) return false;
    avg=sum/number_of_grades;
    if(avg <= 85) return false;
    return x;
}
void Pupil::printData() {
    cout << "Name: " << getName('P') << " " << getName('F') << "," << Class << "-" << Layer << ", ";
    cout << "Grades: ";
    for (auto it = Grades.begin(); it != Grades.end(); it++) {
        cout << *it << ", ";
    }
    cout << endl;
    if(isExcellent()){ //If Pupil is exceptional
        cout << "Exceptional Pupil, Avg: " << avg << endl;
    }
    else cout << "Not Exceptional Pupil, Avg: " << avg << endl;
}
double Pupil::returnSalary() {
    // the name of this function is returnSalary, even though a pupil has no salary.
    // This is simply because we want to use upcasting for VecAnalyser<T>.
    // It calculates the average of this Pupil's grades.
    double sum=0;
    double number_of_grades=0;
    for (auto it = Grades.begin(); it != Grades.end(); it++) {
        sum+=*it;
        number_of_grades++;
    }
    if(number_of_grades==0) return 0;
    avg=sum/number_of_grades;
    return avg;
}