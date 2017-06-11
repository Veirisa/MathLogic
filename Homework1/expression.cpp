#include "expression.h"

extern vector<expression*> hypotheses;
extern vector<expression*> derivations;

string expression::delete_spaces(const string &s) {
    string res = "";
    for (size_t i = 0; i < s.length(); ++i) {
        if ((s[i] != '\0') && (s[i] != ' ') && (s[i] != '\t')) {
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


expression::expression() {
    lexem = EMPTY;
    left = right = nullptr;
}

expression::expression(const string& s) {
    str = delete_spaces(s);
    int length = (int)str.length();
    if (check_operation(CONS, '-', 2, 0, 1)) {
        return;
    }
    if (check_operation(OR, '|', 1, length - 1, -1)) {
        return;
    }
    if (check_operation(AND, '&', 1, length - 1, -1)) {
        return;
    }
    if (s[0] == '!') {
        lexem = NOT;
        left = new expression(s.substr(1));
        right = nullptr;
        return;
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
    if (a.lexem != VAR) {
        if (*(a.left) == *(b.left) && (a.lexem == NOT || *(a.right) == *(b.right))) {
            return true;
        }
        return false;
    }
    return a.str == b.str;
}

bool expression::expr_is_this_axiom(expression* expr, map<string, expression*>& map) {
    if (lexem != expr->lexem && lexem != VAR) {
        return false;
    }
    if (lexem != VAR) {
        if (left->expr_is_this_axiom(expr->left, map) && (lexem == NOT || right->expr_is_this_axiom(expr->right, map))) {
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

bool expression::check_hypotheses() {
    for (size_t i = 1; i <= hypotheses.size(); ++i) {
        if (*(hypotheses[i - 1]) == *this) {
            cout << "(" << "Предположение " << i << ")" << "\n";
            return true;
        }
    }
    return false;
}

bool expression::check_axioms() {
    for (size_t i = 1; i <= SIZE_AXIOMS; ++i) {
        map<string, expression*> map;
        if (schemes_axioms[i - 1]->expr_is_this_axiom(this, map)) {
            cout << "(" << "Сх. акс. " << i << ")" << "\n";
            return true;
        }
    }
    return false;
}


bool expression::check_MP() {
    for (size_t i = 1; i < derivations.size(); ++i) {
        expression* expr = derivations[i - 1];
        if (expr->lexem == CONS && *(expr->right) == *this) {
            for (size_t j = 1; j < derivations.size(); ++j) {
                if (*(derivations[j - 1]) == *(expr->left))  { ;
                    cout << "(" << "M.P. " << j << ", " << i << ")" << "\n";
                    return true;
                }
            }
        }
    }
    return false;
}