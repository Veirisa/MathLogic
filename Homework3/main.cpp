#include "inequality.h"

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    int a, b;
    cin >> a >> b;
    inequality(a, b).write_derivation();
    return 0;
}