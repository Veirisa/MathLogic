#ifndef INEQUALITY_H
#define INEQUALITY_H

#include <string>
#include <iostream>

using namespace std;

class inequality {

    int a, b;

    void give_lemma(const string& name, string* lemma);
    void write_not_more_derivation();
    void write_less_derivation();

public:

    inequality(int a, int b);

    void write_derivation();

};


#endif
