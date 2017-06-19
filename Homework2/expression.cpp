#include "expression.h"
#include <iostream>

extern vector<string> lemma;
extern vector<expression*> hypotheses;
extern vector<expression*> derivations;
extern expression* alpha;
extern string error;

string expression::delete_spaces(const string &s) {
    string res = "";
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] != '\0' && s[i] != ' ' && s[i] != '\t') {
            res += s[i];
        }
    }
    return res;
}

bool expression::check_operation(type_lexem cur_lexem, char symbol, int dif, int index, int change) {
    int balance = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        if (balance == 0 && str[index] == symbol) {
            lexem = cur_lexem;
            left = new expression(str.substr(0, (ull)index));
            right = new expression(str.substr((ull)(index + dif)));
            return true;
        }
        if (str[index] == '(') {
            ++balance;
        }
        if (str[index] == ')') {
            --balance;
        }
        index += change;
    }
    return false;
}

size_t expression::end_of_name(size_t index_var, char left, char right) {
    while (index_var < str.length() &&
           ((str[index_var] >= left && str[index_var] <= right) || (str[index_var] >= '0' && str[index_var] <= '9'))) {
        ++index_var;
    }
    return index_var;
}

void expression::quantifier_expression(type_lexem cur_lexem) {
    lexem = cur_lexem;
    size_t index = end_of_name(1, 'a', 'z');
    left = new expression(str.substr(1, index - 1));
    right = new expression(str.substr(index));
    return;
}

void expression::add_arguments(size_t index) {
    while (index < str.length() - 1) {
        size_t i = index;
        size_t balance = 0;
        while (i < str.length() - 1 && (str[i] != ',' || balance != 0)) {
            if (str[i] == '(')
                balance++;
            if (str[i] == ')')
                balance--;
            ++i;
        }
        args.push_back(new expression(str.substr(index, i - index)));
        index = i + 1;
    }
}

expression::expression() {
    lexem = EMPTY;
    derivation = EMPTY_DER;
    left = right = der_expr = nullptr;
}

expression::expression(const string& s) {
    str = delete_spaces(s);
    int length = (int)str.length();
    derivation = EMPTY_DER;
    der_expr = nullptr;
    if (check_operation(CONS, '-', 2, 0, 1)) {
        return;
    }
    if (check_operation(OR, '|', 1, length - 1, -1)) {
        return;
    }
    if (check_operation(AND, '&', 1, length - 1, -1)) {
        return;
    }
    switch (s[0]) {
        case '!':
            lexem = NOT;
            left = new expression(s.substr(1));
            right = nullptr;
            return;
        case '@':
            quantifier_expression(ANY);
            return;
        case '?':
            quantifier_expression(EXIST);
            return;
        default:
            if (str[0] >= 'A' && str[0] <= 'Z') {
                lexem = PRED;
                size_t index = end_of_name(1, 'A', 'Z');
                name = str.substr(0, index);
                ++index;
                add_arguments(index);
                left = right = nullptr;
                return;
            }
    }
    if (check_operation(EQUAL, '=', 1, 0, 1)) {
        return;
    }
    if (check_operation(ADD, '+', 1, 0, 1)) {
        return;
    }
    if (check_operation(MUL, '*', 1, 0, 1)) {
        return;
    }
    if (str[length - 1] == '\'') {
        lexem = INC;
        left = new expression(str.substr(0, str.length() - 1));
        right = nullptr;
        return;
    }
    if (str[0] == '0') {
        lexem = ZERO;
        str = "0";
        left = right = nullptr;
        return;
    }
    if (str[0] >= 'a' && str[0] <= 'z' && str.find("(") < str.length()) {
        lexem = FUNC;
        size_t index = end_of_name(1, 'a', 'z');
        if (index < str.length()) {
            name = str.substr(0, index);
            ++index;
            add_arguments(index);
            left = right = nullptr;
            return;
        }
    }
    if (str[0] == '(' && str[str.length() - 1] == ')') {
        *this = expression(str.substr(1, str.length() - 2));
        return;
    }
    lexem = VAR;
    left = right = nullptr;
}

bool operator==(const expression& a, const expression& b) {
    if (a.lexem != b.lexem) {
        return false;
    }
    if (a.lexem == ZERO) {
        return true;
    }
    if (a.name_lexem()) {
        if (a.name == b.name && a.args.size() == b.args.size()) {
            for (size_t i = 0; i < a.args.size(); ++i) {
                if (*(a.args[i]) != *(b.args[i])) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    if (a.lexem != VAR) {
        if (*(a.left) == *(b.left) && (a.unary_lexem() || *(a.right) == *(b.right))) {
            return true;
        }
        return false;
    }
    return a.str == b.str;
}

bool operator!=(const expression& a, const expression& b) {
    return !(a == b);
}

bool expression::expr_is_this_axiom(expression* expr, map<string, expression*>& map) {
    if (lexem != expr->lexem && lexem != VAR && lexem != PRED) {
        return false;
    }
    if (lexem == ZERO) {
        return true;
    }
    if (name_lexem()) {
        if (name == expr->name && args.size() == expr->args.size()) {
            for (size_t i = 0; i < args.size(); ++i) {
                if (!args[i]->expr_is_this_axiom(expr->args[i], map)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    if (lexem != VAR) {
        if (left->expr_is_this_axiom(expr->left, map) && (unary_lexem() || right->expr_is_this_axiom(expr->right, map))) {
            return true;
        }
        return false;
    }
    if (map.find(str) == map.cend()) {
        map[str] = expr;
        return true;
    }
    return *(map[str]) == *expr;
}

bool expression::var_is_included_free(expression* var) {
    if (lexem == ZERO) {
        return false;
    }
    if (unary_lexem()) {
        return left->var_is_included_free(var);
    }
    if (binary_lexem()) {
        return left->var_is_included_free(var) || right->var_is_included_free(var);
    }
    if (quantifier_lexem()) {
        if (*left == *var) {
            return false;
        }
        return right->var_is_included_free(var);
    }
    if (name_lexem()) {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i]->var_is_included_free(var)) {
                return true;
            }
        }
    }
    return *this == *var;
}

bool expression::var_is_included(expression* var) {
    if (lexem == ZERO) {
        return false;
    }
    if (unary_lexem()) {
        return left->var_is_included(var);
    }
    if (binary_lexem() || quantifier_lexem()) {
        return left->var_is_included(var) || right->var_is_included(var);
    }
    if (name_lexem()) {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i]->var_is_included_free(var)) {
                return true;
            }
        }
    }
    return *this == *var;
}

bool expression::was_free_substitution_of_term(expression* expr, expression* var, expression &term) {
    if (*var == *this) {
        if (term.lexem == EMPTY) {
            term = *expr;
            return true;
        }
        return term == *expr;
    }
    if (lexem != expr->lexem) {
        return false;
    }
    if (lexem == ZERO) {
        return true;
    }
    if (unary_lexem()) {
        return left->was_free_substitution_of_term(expr->left, var, term);
    }
    if (binary_lexem()) {
        return left->was_free_substitution_of_term(expr->left, var, term) && right->was_free_substitution_of_term(expr->right, var, term);
    }
    if (quantifier_lexem()) {
        if (*left == *var) {
            return *var == *(expr->left) && *right == *(expr->right);
        } else {
            if (*left == *(expr->left) && right->was_free_substitution_of_term(expr->right, var, term)) {
                if (term.lexem != EMPTY) {
                    if (term.var_is_included(left)) {
                        error = ":";
                        return false;
                    }
                }
                return true;
            }
            return false;
        }
    }
    if (name_lexem()) {
        if (name == expr->name && args.size() == expr->args.size()) {
            for (size_t i = 0; i < args.size(); ++i) {
                if (!args[i]->was_free_substitution_of_term(expr->args[i], var, term)) {
                    return false;
                }
            }
            return true;
        }
    }
    return str == expr->str;
}

bool expression::is_free_to_substitution_in_quantifier_scheme(bool any_scheme) {
    expression term;
    expression* var = new expression();
    expression* expr_with_var = new expression();
    expression* expr_with_term = new expression();
    bool try_check = false;
    if (lexem == CONS && left->lexem == ANY && any_scheme) {
        var = left->left;
        expr_with_var = left->right;
        expr_with_term  = right;
        try_check = true;
    }
    if (lexem == CONS && right->lexem == EXIST && !any_scheme) {
        var = right->left;
        expr_with_var = right->right;
        expr_with_term = left;
        try_check = true;
    }
    if (try_check) {
        if (!expr_with_var->was_free_substitution_of_term(expr_with_term, var, term)) {
            if (error == ":") {
                error += " терм " + term.str + " не свободен для подстановки в формулу " + expr_with_var->str + " вместо переменной " + var->str + ".";
            }
            return false;
        }
        return true;
    }
    return false;
}

bool expression::check_free_includings(expression* var, expression* expr) {
    if (expr->var_is_included_free(var)) {
        error = ": переменная " + var->str + " входит свободно в формулу " + expr->str + ".";
        return false;
    }
    if (alpha->lexem != EMPTY && alpha->var_is_included_free(var)) {
        error += ": используется правило с квантором по переменной " + var->str + ", входящей свободно в допущение " + alpha->str + ".";
        return false;
    }
    return true;
}

bool expression::check_hypotheses() {
    for (size_t i = 0; i < hypotheses.size(); ++i) {
        if (*(hypotheses[i]) == *this) {
            derivation = HYPO_DER;
            return true;
        }
    }
    return false;
}

bool expression::check_axioms() {
    for (size_t i = 0; i < SIZE_AXIOMS; ++i) {
        map<string, expression*> map;
        if ((i < BORDER_AXIOMS && schemes_axioms[i]->expr_is_this_axiom(this, map)) // 0 - 9
            || (i >= BORDER_AXIOMS && *(schemes_axioms[i]) == *this) // 10 - 19 (complete equals)
            || ((i == BORDER_AXIOMS || i == BORDER_AXIOMS + 1) && is_free_to_substitution_in_quantifier_scheme(i == BORDER_AXIOMS))) { // 10-11
            derivation = AXIOM_DER;
            return true;
        }
    }
    if (lexem == CONS && left->lexem == AND && left->right->lexem == ANY && left->right->right->lexem == CONS) { // 20
        expression* expr = right;
        expression* quant = left->right;
        expression term;
        if (expr->was_free_substitution_of_term(left->left, quant->left, term) && term.lexem == ZERO && *(quant->right->left) == *expr) {
            term = expression();
            if (expr->was_free_substitution_of_term(quant->right->right, quant->left, term) && term.lexem == INC && *(term.left) == *(quant->left)) {
                derivation = AXIOM_DER;
                return true;
            }
        }
    }
    return false;
}


bool expression::check_MP() {
    for (size_t i = 0; i < derivations.size() - 1; ++i) {
        expression* expr = derivations[i];
        if (expr->lexem == CONS && *(expr->right) == *this) {
            for (size_t j = 0; j < derivations.size() - 1; ++j) {
                if (*(derivations[j]) == *(expr->left))  {
                    derivation = MP_DER;
                    der_expr = derivations[j];
                    return true;
                }
            }
        }
    }
    return false;
}


bool expression::check_any() {
    if (lexem == CONS && right->lexem == ANY) {
        expression* var = right->left;
        expression* expr = left;
        for (size_t i = 0; i < derivations.size() - 1; ++i) {
            if (derivations[i]->lexem == CONS && *(derivations[i]->left) == *left && *(derivations[i]->right) == *(right->right)) {
                if (!check_free_includings(var, expr)) {
                    return false;
                }
                derivation = ANY_DER;
                der_expr = derivations[i];
                return true;
            }
        }
    }
    return false;
}

bool expression::check_exist() {
    if (lexem == CONS && left->lexem == EXIST) {
        expression* var = left->left;
        expression* expr = right;
        for (size_t i = 0; i < derivations.size() - 1; ++i) {
            if (derivations[i]->lexem == CONS && *(derivations[i]->right) == *right && *(derivations[i]->left) == *(left->right)) {
                if (!check_free_includings(var, expr)) {
                    return false;
                }
                derivation = EXIST_DER;
                return true;
            }
        }
    }
    return false;
}

void expression::add_changed_lemma(string* res, size_t num, const string &a, const string &b, const string& c) {
    for (size_t i = 0; i < lemma[num].size(); ++i) {
        switch (lemma[num][i]) {
            case 'a':
                *res += a;
                break;
            case 'b':
                *res += b;
                break;
            case 'c':
                *res += c;
                break;
            default:
                *res += lemma[num][i];
        }
    }
}

string* expression::change_derivation() {
    string* res = new string("");
    if (*this == *alpha) {
        string a = "(" + alpha->str + ")";
        string a_cons = a + "->" + a;
        *res += a + "->" + a_cons + "\n";
        *res += "(" + a + "->" + a_cons + ")" + "->" + "(" + a + "->" + "(" + a_cons + ")" + "->" + a + ")" + "->" + a_cons + "\n";
        *res += "(" + a + "->" + "(" + a_cons + ")" + "->" + a + ")" + "->" + a_cons + "\n";
        *res += a + "->" + "(" + a_cons + ")" + "->" + a + "\n";
        *res += a_cons + "\n";
        return res;
    }
    string a, b, c, j, i, i_j, j_i;
    switch (derivation) {
        case MP_DER:
            a = "(" + alpha->str + ")";
            j = a + "->" + der_expr->str;
            i = a + "->" + str;
            j_i = "(" + der_expr->str + ")->" + str;
            *res += "(" + j + ")" + "->" + "(" + a + "->" + j_i + ")" + "->" + i + "\n";
            *res += "(" + a + "->" + j_i + ")" + "->" + i + "\n";
            *res += i + "\n";
            return res;
        case ANY_DER:
            a = "(" + alpha->str + ")";
            b = "(" + left->str + ")";
            c = "(" + right->right->str + ")";
            add_changed_lemma(res, 0, a, b, c);
            i_j = "(" + der_expr->str + ")";
            i = "(" + a + "&" + "(" + left->str + ")" + ")";
            *res += i + "->" + c + "\n";
            c = right->str;
            *res += i + "->" + "(" + c + ")" + "\n";
            add_changed_lemma(res, 1, a, b, c);
            *res += a + "->" + str + "\n";
            return res;
        case EXIST_DER:
            a = "(" + alpha->str + ")";
            b = "(" + left->right->str + ")";
            c = "(" + right->str + ")";
            add_changed_lemma(res, 2, a, b, c);
            *res += b + "->" + a + "->" + c + "\n";
            b = a;
            a = left->str;
            *res += "(" + a + ")" + "->" + b + "->" + c + "\n";
            add_changed_lemma(res, 2, a, b, c);
            *res += b + "->" + str + "\n";
            return res;
        default:  //AXIOM_DER, HYPO_DER
            *res += str + "\n";
            *res += "(" + str + ")" + "->" + "(" + alpha->str + ")" + "->" + "(" + str + ")" + "\n";
            *res += "(" + alpha->str + ")" + "->" + "(" + str + ")" + "\n";
            return res;
    }
}
