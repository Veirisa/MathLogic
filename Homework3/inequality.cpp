#include "inequality.h"
#include <fstream>

inequality::inequality(int a, int b) : a(a), b(b) {};

void inequality::give_lemma(const string &name, string *const lemma) {
    fstream fin(name);
    string s;
    *lemma = "";
    while (getline(fin, s)) {
        *lemma += s + "\n";
    }
    fin.close();
}

void inequality::write_not_more_derivation() {
    string a_s = "";
    for (size_t i = 0; i < a; ++i) {
        a_s += "'";
    }
    string b_s = a_s;
    string dif_s = "";
    for (size_t i = 0; i < b - a; ++i) {
        b_s += "'";
        dif_s += "'";
    }
    cout << "|-?p(0" + a_s + "+p)=0" + b_s + "\n";
    string* s_ptr = new string("");
    give_lemma("lemma_add_zero.txt", s_ptr);
    cout << *s_ptr;
    cout << "@a(a+0=a)->0" + a_s + "+0=0" + a_s + "\n";
    cout << "0" + a_s + "+0=0" + a_s + "\n";
    give_lemma("lemma_many_inc.txt", s_ptr);
    string * changed_s_ptr = new string("");
    for (size_t i = 0; i < s_ptr->size(); ++i) {
        if (s_ptr->at(i) == 'A') {
            *changed_s_ptr += a_s;
        } else {
            *changed_s_ptr += s_ptr->at(i);
        }
    }
    swap(s_ptr, changed_s_ptr);
    delete changed_s_ptr;
    string inc = "";
    for (size_t i = 0; i < b - a; ++i) {
        for (size_t j = 0; j < s_ptr->size(); ++j) {
            if (s_ptr->at(j) == '$') {
                cout << inc;
            } else {
                cout << s_ptr->at(j);
            }
        }
        inc += "'";
    }
    cout << "(0" + a_s + "+0" + dif_s + ")=0" + b_s + "->?p(0" + a_s + "+p)=0" + b_s + "\n";
    cout << "?p(0" + a_s + "+p)=0" + b_s;
    delete s_ptr;
}

void inequality::write_less_derivation() {
    string b_s = "";
    for (size_t i = 0; i < b; ++i) {
        b_s += "'";
    }
    string a_s = b_s;
    string dif_s = "";
    for (size_t i = 0; i < a - b - 1; ++i) {
        dif_s += "'";
        a_s += "'";
    }
    cout << "|-!(?p(0'" + a_s + "+p=0" + b_s + "))" << "\n";
    string * s_ptr = new string("");
    give_lemma("lemma_def_add.txt", s_ptr);
    cout << *s_ptr;
    give_lemma("lemma_deMorgan.txt", s_ptr);
    for (size_t i = 0; i < s_ptr->size(); ++i) {
        if (s_ptr->at(i) == 'B') {
            cout << b_s;
        } else if (s_ptr->at(i) == 'C') {
            cout << dif_s;
        } else {
            cout << s_ptr->at(i);
        }
    }
    delete s_ptr;
}

void inequality::write_derivation() {
    if (a <= b) {
        write_not_more_derivation();
    } else {
        write_less_derivation();
    }
}

