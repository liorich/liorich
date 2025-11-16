#include "School.h"

School * School::school=NULL;
School::School() { Num_of_Layers=0; }
School::~School() {
    //cout << "School Dtor Called" << endl;
    if(!school) delete school;
}
School* School::getSchool(){
    if (!school) {
        school = new School;
        return school;
    }
    else cout << "School already exists" << endl;
    return school;
}
void School::menu() {
    /* School Main Menu
     */
    bool flag; // General useful flag
    int x; // Menu input

    // Holders for new data:
    string pname;
    string fname;
    char charLayer;
    char input;
    int  classnumber;
    int Years,Children;
    int Excellent=0;
    string tmp;

    // Holders for new objects:
    Layer *newLayer;
    Class *newClass;
    Tutor *newTutor;
    Teacher *newTeacher;
    Pupil *newPupil;
    Secretary *newSecretary;
    Manager *newManager;
    VecAnalyser Analyser;

    printMenu();

    while (true) {
        std::cin >> x;
        switch (x) {
            case 0: // Print the menu again
                printMenu();
                break;
            case 1: // Add a new pupil
                cout << "Enter the student's First Name:" ;
                cin.clear();
                cin.ignore();
                cin >> pname;
                cout << "Enter the student's Last Name:" ;
                cin.clear();
                cin.ignore();
                cin >> fname;
                // Check for identical person.
                if(hasPerson(pname,fname)) break;
                cout << "Enter the student's Layer (Character a-f):" <<endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> charLayer;
                    if (charLayer < 'a' || charLayer > 'f') {
                        std::cout << "Wrong Input. Layer range is between f and a:" <<endl;
                    } else break;
                }
                cout << "Enter the student's Class (Digit 1-3):"<<endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> classnumber;
                    if (classnumber < 1 ||  classnumber > 3) {
                        std::cout << "Wrong Input. Class range is between 1 and 3:"<<endl;
                    }
                    else break;
                }
                // Get the layer, or create it in the function
                newLayer=returnLayer(charLayer);
                // 2) Check if class exists
                newClass=newLayer->returnClassptr(classnumber); // Get me the class, if it exists
                // 2.a) Class doesn't exist yet
                if(newClass==NULL){
                    newClass=new Class(charLayer,classnumber);
                    newLayer->addClass(newClass,classnumber);
                }
                // 3) Add student to student vector
                newPupil=new Pupil(pname,fname,charLayer,classnumber);
                newPupil->setGrades(); // Add grades for the student;
                newClass->addPupil(*newPupil); // Add pupil by value to the relevant class.
                Pupils.push_back(newPupil); // Save pupil pointer in the School's Pupil Vector.
                std::cout << "Added student." << std::endl;
                break;
            case 2: // Add Teacher
                cout << "Enter the Teacher's First Name:" ;
                cin.clear();
                cin.ignore();
                cin >> pname;
                cout << "Enter the Teacher's Last Name:" ;
                cin.clear();
                cin.ignore();
                cin >> fname;
                // Check for identical person.
                if(hasPerson(pname,fname)) break;
                cout << "Enter the Teacher's working years:";
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> Years;
                    if (Years < 0) {
                        std::cout << "Cannot enter negative amount. Please enter a positive integer."<< endl;
                    }
                    else break;
                }
                cout << endl;
                newTeacher=new Teacher(pname,fname,Years);
                newTeacher->setTeacherSubjects();
                Workers.push_back(newTeacher);
                std::cout << "Added Teacher." << std::endl;
                break;
            case 3: //Add Tutor
                cout << "Enter the Tutor's First Name:" ;
                cin.clear();
                cin.ignore();
                cin >> pname;
                cout << "Enter the Tutor's Last Name:" ;
                cin.clear();
                cin.ignore();
                cin >> fname;
                // Check for identical person.
                flag=false;
                for (auto it = Workers.begin(); it != Workers.end(); it++) {
                    if ((*it)->getName('P') == pname && (*it)->getName('F') == fname) {
                        if(typeid(**it)==typeid(Tutor)) {
                            cout << "A tutor with the same name already exists. Returning to Menu." << endl;
                            flag=true;
                            break;
                        }
                    }
                }
                if(flag) break; // If tutor with same name already exists.
                cout << "Enter the Tutor's working years:" << endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> Years;
                    if (Years < 0) {
                        std::cout << "Cannot enter negative amount. Please enter a positive integer."<< endl;
                    }
                    else break;
                }
                cout << "Enter the Tutor's Layer (Character a-f):" << endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> charLayer;
                    if (charLayer < 'a' ||  charLayer > 'f') {
                        std::cout << "Wrong Input. Layer range is between f and a:"<< endl;
                    }
                    else break;
                }
                cout << "Enter the Tutor's Class (Digit 1-3):" << endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> classnumber;
                    if (classnumber < 1 ||  classnumber > 3) {
                        std::cout << "Wrong Input. Class range is between 1 and 3:"<< endl;
                    }
                    else break;
                }
                // Get the layer, or create it in the function
                newLayer=returnLayer(charLayer);
                // 2) Check if class exists
                newClass=newLayer->returnClassptr(classnumber); // Get me the class, if it exists
                // 2.a) Class doesn't exist yet
                if(newClass==NULL){
                    newClass=new Class(charLayer,classnumber);
                    newLayer->addClass(newClass,classnumber);
                }
                // 3) Add Tutor to class, and add the Tutor to the School's working people vector
                newTutor=new Tutor(pname,fname,Years);
                // Check if Tutor already exists, if yes, don't add;
                if(newClass->hasTutor()){
                    std::cout << "Tutor already exists here, Replacing with new one." << std::endl;
                    newClass->getClassTutor()->setClass(NULL);
                    newClass->setClassTutor(NULL);
                }
                newTutor->setClass(newClass);
                newTutor->setTeacherSubjects();
                newClass->setClassTutor(newTutor);
                Workers.push_back(newTutor);
                std::cout << "Tutor registered." << std::endl;
                break;
            case 4: // Add Manager
                // Check if static manager already exists
                if(newManager->hasManager()){
                    cout << "A Manager already exists in this school. Returning to Menu." << endl;
                    break;
                }
                newManager=newManager->getManager();
                cout << "Enter the Manager's First Name:" ;
                cin.clear();
                cin.ignore();
                cin >> pname;
                cout << "Enter the Manager's Last Name:" ;
                cin.clear();
                cin.ignore();
                cin >> fname;
                // Check for identical person.
                if(hasPerson(pname,fname)) break;
                cout << "Enter the Manager's working years (as a Manager):"<< endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> Years;
                    if (Years < 0) {
                        std::cout << "Cannot enter negative amount. Please enter a positive integer."<< endl;
                    }
                    else break;
                }
                cout << "Enter the Manager's office number:"<< endl;
                cin >> tmp;
                newManager->setName(pname,fname);
                newManager->Teacher::setYears(0);
                newManager->AdministrationPersonal::setYears(Years);
                newManager->setOfficeNumber(tmp);
                cout << "Is this manager also a teacher? Enter Y/N:";
                while(true) {
                    cin >> input;
                    if (input != 'Y' &&  input != 'N') {
                        std::cout << "Wrong Input. Choose Y/N: ";
                    }
                    else break;
                }
                if(input=='N'){
                    Workers.push_back(newManager);
                    std::cout << "Manager registered. " << std::endl;
                    break;
                }
                // else, add teacher data
                cout << "Enter the Manager's teaching years:"<< endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> Years;
                    if (Years < 0) {
                        std::cout << "Cannot enter negative amount. Please enter a positive integer."<< endl;
                    }
                    else break;
                }
                newManager->Teacher::setYears(Years);
                while(true) {
                    newManager->setTeacherSubjects();
                    if(newManager->getNumberofSubjects()!=0) break;
                    cout << "A manager cannot be a teacher and also not teach!" << endl;
                }
                Workers.push_back(newManager);
                std::cout << "Manager registered. " << std::endl;
                break;
            case 5: // Add Secretary
                cout << "Enter the Secretary's First Name:" ;
                cin.clear();
                cin.ignore();
                cin >> pname;
                cout << "Enter the Secretary's Last Name:" ;
                cin.clear();
                cin.ignore();
                cin >> fname;
                // Check for identical person.
                if(hasPerson(pname,fname)) break;
                cout << "Enter the Secretary's office number:"<< endl;
                cin >> tmp;
                cout << "Enter the Secretary's working years:"<< endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> Years;
                    if (Years < 0) {
                        std::cout << "Cannot enter negative amount. Please enter a positive integer."<< endl;
                    }
                    else break;
                }
                cout << "Enter the Secretary's studying children in this school:"<< endl;
                while(true) {
                    cin.clear();
                    cin.ignore();
                    cin >> Children;
                    if (Children < 0) {
                        std::cout << "Cannot enter negative amount. Please enter a positive integer."<< endl;
                    }
                    else break;
                }
                newSecretary = new Secretary(pname,fname,Years,Children);
                newSecretary->setOfficeNumber(tmp);
                Workers.push_back(newSecretary);
                std::cout << "Secretary registered. " << std::endl;
                break;
            case 6: // Print Everyone's Details
                std::cout << "Workers:" << std::endl;
                for (auto it = Workers.begin(); it != Workers.end(); it++) {
                    (*it)->printData();
                }
                std::cout << "==================" << std::endl;
                std::cout << "Pupils:" << std::endl;
                for (auto it = Pupils.begin(); it != Pupils.end(); it++) {
                    (*it)->printData();
                }
                std::cout << "Done. " << std::endl;
                break;
            case 7: // Print Outstanding People
                std::cout << "Outstanding Workers:" << std::endl;
                for (auto it = Workers.begin(); it != Workers.end(); it++) {
                    if((*it)->isExcellent()){
                        (*it)->printData();
                        Excellent++;
                    }
                }
                if(Excellent==0) std::cout << "There are no outstanding workers in this school. :(" << std::endl;
                Excellent=0;
                std::cout << "==================" << std::endl;
                std::cout << "Outstanding Pupils:" << std::endl;
                for (auto it = Pupils.begin(); it != Pupils.end(); it++) {
                    if((*it)->isExcellent()){
                        (*it)->printData();
                        Excellent++;
                    }
                }
                if(Excellent==0) std::cout << "There are no outstanding pupils in this school. :(" << std::endl;
                std::cout << "Done. " << std::endl;
                break;
            case 8: // Print Tutor's class
                cout << "Enter the Tutor's First Name:" ;
                cin.clear();
                cin.ignore();
                cin >> pname;
                cout << "Enter the Tutor's Last Name:" ;
                cin.clear();
                cin.ignore();
                cin >> fname;
                flag=false;
                for (auto it = Workers.begin(); it != Workers.end(); it++) {
                    if(typeid(**it)==typeid(Tutor)){
                        if(pname==(*it)->getName('P') && fname==(*it)->getName('F')){
                            // Found the tutor
                            (*it)->printData();
                            cout << "Done." << endl;
                            flag=true;
                            break;
                        }
                    }
                }
                if(!flag) cout << "Tutor not found." << endl;
                break;
            case 9: // Highest paid worker
                Analyser.getVector(Workers);
                Analyser.printMax();
                break;
            case 10:
                std::cout << "Goodbye!" << std::endl;
                for (auto it = Workers.begin(); it != Workers.end(); it++) {
                    delete (*it);
                }
                for (auto it = Pupils.begin(); it != Pupils.end(); it++) {
                    delete (*it);
                }
                return;
        }
        if (x < 0 || x > 10) std::cout << "Invalid Input. Please enter an integer between 0 and 10";
    }
}

void School::printMenu() {
    std::cout << "==================================================" << std::endl;
    std::cout << "(0) Print this Menu again." << std::endl;
    std::cout << "(1) Add a pupil." << std::endl;
    std::cout << "(2) Add a Teacher." << std::endl;
    std::cout << "(3) Add a Tutor." << std::endl;
    std::cout << "(4) Add a Manager." << std::endl;
    std::cout << "(5) Add a Secretary." << std::endl;
    std::cout << "(6) Print Everyone's Details" << std::endl;
    std::cout << "(7) Print Outstanding People." << std::endl;
    std::cout << "(8) Print Tutor's Class." << std::endl;
    std::cout << "(9) Print Highest Paid Worker." <<std::endl;
    std::cout << "(10) Exit." <<std::endl;
    std::cout << "==================================================" << std::endl;
}

Layer * School::returnLayer(char charLayer) {
    bool flag;
    Layer *newLayer;
    // 1) Check if Layer exists
    newLayer=NULL;
    for (auto it = Layer_Vector.begin(); it != Layer_Vector.end(); it++) {
        if((*it)->getChar()==charLayer) newLayer=*it;
    }
    // 1.a) Layer does not yet exist
    if(newLayer==NULL){
        newLayer=new Layer(charLayer);
        // Add in a sorted way:
        flag=false;
        if(Layer_Vector.empty()) { flag=true; Layer_Vector.push_back(newLayer); }
        else{
            for (auto it = Layer_Vector.begin(); it != Layer_Vector.end(); it++) {
                if ((*it)->getChar() > charLayer) {
                    Layer_Vector.insert(it, newLayer);
                    flag=true;
                    break;
                }
            }
        }
        if(!flag) Layer_Vector.push_back(newLayer);
        Num_of_Layers++;
    }
    return newLayer;
}

bool School::hasPerson(string pname, string fname) {
    // Used to check if a person with the same name exists in the database.
    // True : Do not add the same person
    // False : OK to add the same person
    char input;
    for (auto it = Pupils.begin(); it != Pupils.end(); it++) {
        if ((*it)->getName('P') == pname && (*it)->getName('F') == fname) {
            cout << "A person already exists in the database under the same name. Continue? Y/N:" << endl;
            while(true) {
                cin >> input;
                if (input != 'Y' &&  input != 'N') {
                    std::cout << "Wrong Input. Choose Y/N: ";
                }
                else break;
            }
            switch(input){
                case 'Y' : return false;
                case 'N' :
                    cout << "Returning to Menu." << endl;
                    return true;
            }
        }
    }
    for (auto it = Workers.begin(); it != Workers.end(); it++) {
        if ((*it)->getName('P') == pname && (*it)->getName('F') == fname) {
            cout << "A person already exists in the database under the same name. Continue? Y/N:" << endl;
            while(true) {
                cin >> input;
                if (input != 'Y' &&  input != 'N') {
                    std::cout << "Wrong Input. Choose Y/N: ";
                }
                else break;
            }
            switch(input){
                case 'Y' : return false;
                case 'N' :
                    cout << "Returning to Menu." << endl;
                    return true;
            }
        }
    }
    return false;
}