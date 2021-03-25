#include <algorithm>
#include <iostream>
#include <vector>
#include "knapsack.h"

using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    int n, max_w;
    cin >> n >> max_w;

    vector<int> c(n), w(n);
    for (int i = 0; i < n; ++i) {
        cin >> c[i] >> w[i];
    }

    Knapsack k(c, w);
    auto [c_opt, w_opt, things] = k.solve_recover_dpw(max_w);

    cout << c_opt << '\n';
    for (int id : things) {
        cout << id + 1 << ' ';
    }

    return 0;
}
