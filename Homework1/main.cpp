#include "expression.h"

vector<expression*> hypotheses;
vector<expression*> derivations;

void add_hypotheses(const string& s) {
    size_t i = 0;
    string new_hypo = "";
    while (s[i] != '|' || s[i + 1] != '-') {
        if (s[i] == '\0' || s[i] == ' ' || s[i] == '\t') {
            ++i;
            continue;
        }
        new_hypo += s[i];
        ++i;
        if (s[i] == ',') {
            ++i;
            hypotheses.push_back(new expression(new_hypo));
            new_hypo = "";
        }
    }
    if (new_hypo.size() > 0) {
        hypotheses.push_back(new expression(new_hypo));
    }
}

void full_check() {
    string s;
    while (getline(cin, s)) {
        expression* expr = new expression(s);
        derivations.push_back(expr);
        cout << "(" << derivations.size() << ")" << " " << expr->string_of_expr() << " ";
        if (!expr->check_hypotheses() && !expr->check_axioms() && !expr->check_MP()) {
            cout << "(" << "Не доказано" << ")" << "\n";
        }
    }
}

int main() {
    cin.tie();
    ios_base::sync_with_stdio(0);
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    string s;
    getline(cin, s);
    add_hypotheses(s);
    cout << s << "\n";
    full_check();
    return 0;
}
