#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>
#include <map>

using namespace std;
typedef unsigned long long ull;

enum type_lexem {
    EMPTY, ZERO, VAR, NOT, INC, CONS, OR, AND, EQUAL, ADD, MUL, PRED, FUNC, ANY, EXIST
    
};

enum type_derivation {
    EMPTY_DER, HYPO_DER, AXIOM_DER, MP_DER, ANY_DER, EXIST_DER
};

class expression {
    
    type_lexem lexem;
    type_derivation derivation;
    expression* left;
    expression* right;
    expression* der_expr;
    string str, name;
    vector<expression*> args;
    
    bool unary_lexem() const;
    bool binary_lexem() const ;
    bool name_lexem() const ;
    bool quantifier_lexem() const ;
    
    string delete_spaces(const string &s);
    size_t end_of_name(size_t index_var, char left, char right);
    void add_arguments(size_t index);
    void quantifier_expression(type_lexem cur_lexem);
    bool check_operation(type_lexem cur_lexem, char symbol, int dif, int start, int change);
    
    bool var_is_included_free(expression* var);
    bool var_is_included(expression* var);
    bool expr_is_this_axiom(expression* expr, map<string, expression*>& m);
    bool was_free_substitution_of_term(expression* expr, expression* var, expression& term);
    bool is_free_to_substitution_in_quantifier_scheme(bool any_scheme);
    bool check_free_includings(expression* var, expression* expr);
    
    void add_changed_lemma(string* res, size_t num, const string &a, const string &b, const string& c);
    
public:
    
    expression();
    expression(const string& s);
    
    bool check_hypotheses();
    bool check_axioms();
    bool check_MP();
    bool check_any();
    bool check_exist();
    
    string string_of_expr();
    string* change_derivation();
    
    friend bool operator==(const expression& a, const expression& b);
    friend bool operator!=(const expression& a, const expression& b);
    
};

inline bool expression::unary_lexem() const {
    return lexem == NOT || lexem == INC;
}

inline bool expression::binary_lexem() const {
    return lexem == CONS || lexem == OR || lexem == AND || lexem == EQUAL || lexem == ADD || lexem == MUL;
}

inline bool expression::name_lexem() const {
    return lexem == PRED || lexem == FUNC;
}

inline bool expression::quantifier_lexem() const {
    return lexem == ANY || lexem == EXIST;
}

inline string expression::string_of_expr() {
    return str;
}

const size_t SIZE_AXIOMS = 21;
const size_t BORDER_AXIOMS = 10;
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
    //------------------------------
    new expression("@xY->Y"), //10
    new expression("Y->?xY"), //11
    new expression("a=b->a'=b'"), //12
    new expression("a=b->a=c->b=c"), //13
    new expression("a'=b'->a=b"), //14
    new expression("!a'=0"), //15
    new expression("a+b'=(a+b)'"), //16
    new expression("a+0=a"), //17
    new expression("a*0=0"), //18
    new expression("a*b'=a*b+a"), //19
    //------------------------------
    new expression("Y&@x(Y->Y)->Y") //20
};

#endif
