#include "Teacher.h"
#include "AdministrationPersonal.h"

class Manager: public Teacher, public AdministrationPersonal{
public:
    static Manager * manager;
    Manager();
    Manager(string pname, string fname, int TeacherYears, int ManagerYears);
    ~Manager();
    static Manager * getManager();
    static bool hasManager();
    double returnSalary();
    bool isExcellent();
    void printData();
};
