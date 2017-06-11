#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

using namespace std;
typedef unsigned long long ull;

enum type_lexem {
    EMPTY, VAR, NOT, CONS, OR, AND
};

class expression {

    type_lexem lexem;
    expression* left;
    expression* right;
    string str;

    string delete_spaces(const string &s);
    bool check_operation(type_lexem cur_lexem, char symbol, int dif, int start, int change);

public:

    expression();
    expression(const string& s);

    bool expr_is_this_axiom(expression* expr, map<string, expression*>& map);

    bool check_hypotheses();
    bool check_axioms();
    bool check_MP();

    string string_of_expr();

    friend bool operator==(const expression& a, const expression& b);

};

inline string expression::string_of_expr() {
    return str;
}

const size_t SIZE_AXIOMS = 10;
const vector<expression*> schemes_axioms = {
        new expression("a->b->a"), //0
        new expression("(a->b)->(a->b->c)->(a->c)"), // 1
        new expression("a->b->a&b"), //2
        new expression("a&b->a"), //3
        new expression("a&b->b"), //4
        new expression("a->a|b"), //5
        new expression("b->a|b"), //6
        new expression("(a->c)->(b->c)->(a|b->c)"), //7
        new expression("(a->b)->(a->!b)->!a"), //8
        new expression("!!a->a"), //9
};

#endif
