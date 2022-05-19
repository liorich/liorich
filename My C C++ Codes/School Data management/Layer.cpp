#include "Layer.h"

Layer::Layer(){
    LayerChar='x';
}
Layer::Layer(char layerChar) {
    LayerChar=layerChar;
}
Layer::~Layer(){
    //cout << "Layer Dtor Called" << endl;
}
void Layer::addClass(Class *newClass, int number) { // Add Class in Layer, by order
    bool flag=false;
    if(Classes.empty()) { flag=true; Classes.push_back(newClass); }
    else{
        for (auto it = Classes.begin(); it != Classes.end(); it++) {
            if ((*it)->getClassNumber() > number) {
                Classes.insert(it, newClass);
                flag=true;
                break;
            }
        }
    }
    if(!flag) Classes.push_back(newClass);
}

char Layer::getChar() const { return LayerChar; }

Class Layer::returnClass(int index) { // (Unused, since we're working with vectors)
    if(Classes.empty() || index > Classes.size()-1) {
        cout << "Empty vector or wrong input, returning empty class" << endl;
        Class a;
        return a;
    }
    return *Classes[index];
}

Class * Layer::returnClassptr(int number) {
    // return the pointer for the class with the number [number].
    for (auto it = Classes.begin(); it != Classes.end(); it++) {
        if((*it)->getClassNumber()==number) return (*it);
    }
    // else it doesn't exist yet
    return NULL;
}