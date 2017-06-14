#include <iostream>
#include <fstream>
#include "expression.h"

const string lemm_in[3] = {"lemma_for_any_first.txt",
    "lemma_for_any_second.txt",
    "lemma_for_exist.txt"};
vector<string> lemma;
vector<expression*> hypotheses;
vector<expression*> derivations;
expression* alpha;
expression* answer;
string error;

bool add_hypotheses(const string& s) {
    size_t i = 0;
    int balance = 0;
    string buffer = "";
    while (s[i] != '|' || s[i + 1] != '-') {
        if (s[i] == '\0' || s[i] == ' ' || s[i] == '\t') {
            ++i;
            continue;
        }
        if (s[i] == '(') {
            ++balance;
        }
        if (s[i] == ')') {
            --balance;
        }
        buffer += s[i];
        ++i;
        if (s[i] == ',' && balance == 0) {
            ++i;
            hypotheses.push_back(new expression(buffer));
            buffer = "";
        }
    }
    if (buffer.size() > 0) {
        hypotheses.push_back(new expression(buffer));
    }
    i += 2;
    buffer = "";
    for (; i < s.length(); ++i) {
        buffer += s[i];
    }
    answer = new expression(buffer);
    if (hypotheses.empty()) {
        alpha = new expression();
        return false;
    }
    alpha = hypotheses.back();
    return true;
}

string create_head() {
    string res = "|-(" + alpha->string_of_expr() + ")" + "->" + "(" + answer->string_of_expr() + ")";
    if (hypotheses.size() == 1) {
        return res;
    }
    size_t i = hypotheses.size() - 2;
    while (i >= 1) {
        res = "," + hypotheses[i]->string_of_expr() + res;
        --i;
    }
    return hypotheses[0]->string_of_expr() + res;
}

void add_lemma(size_t num) {
    ifstream fin(lemm_in[num]);
    lemma.push_back("");
    string s;
    while (getline(fin, s)) {
        lemma[num] += s + "\n";
    }
    fin.close();
}

bool full_check() {
    string s;
    while (getline(cin, s)) {
        expression* expr = new expression(s);
        derivations.push_back(expr);
        if (!expr->check_hypotheses() && !expr->check_axioms() && !expr->check_MP() && !expr->check_any() && !expr->check_exist()) {
            cout << "Вывод некорректен начиная с формулы номер " << derivations.size() << error << "\n" << s;
            return false;
        }
    }
    return true;
}

int main() {
    cin.tie();
    ios_base::sync_with_stdio(0);
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    string s;
    getline(cin, s);
    bool need_change = add_hypotheses(s);
    if (full_check()) {
        if (need_change) {
            cout << create_head() << "\n";
            for (size_t i = 0; i < 3; ++i) {
                add_lemma(i);
            }
            for (size_t i = 0; i < derivations.size(); ++i) {
                string* result_derivation = derivations[i]->change_derivation();
                cout << *result_derivation;
                delete result_derivation;
            }
        } else {
            cout << "|-" << answer->string_of_expr() << "\n";
            for (size_t i = 0; i < derivations.size(); ++i) {
                cout << derivations[i]->string_of_expr() << "\n";
            }
        }
    }
    return 0;
}
