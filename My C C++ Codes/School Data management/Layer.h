#include "Class.h"

class Layer{
    char LayerChar;
    vector<Class*> Classes;
public:
    Layer();
    Layer(char layerChar);
    void addClass(Class * newClass, int number);
    char getChar() const;
    ~Layer();
    Class returnClass(int index);
    Class * returnClassptr(int number);
};
