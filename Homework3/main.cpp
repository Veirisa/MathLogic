#include "inequality.h"

int main() {
    cin.tie();
    ios_base::sync_with_stdio(0);
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    int a, b;
    cin >> a >> b;
    inequality(a, b).write_derivation();
    return 0;
}
