#include <algorithm>
#include <iostream>
#include <vector>
#include "knapsack.h"

using namespace std;

int main() {
    //freopen("data/9.public", "r", stdin);
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    int n, max_w;
    cin >> n >> max_w;

    vector<long long> c(n), w(n);
    for (int i = 0; i < n; ++i) {
        cin >> c[i] >> w[i];
    }
    //for (auto& x : w)
    //    cin >> x;
    //for (auto& x : c)
    //    cin >> x;

    Knapsack k(c, w);
    auto [c_opt, w_opt, things] = k.solve_recover_meet_in_the_middle_bullshit(max_w, 15, 25);

    cout << c_opt << '\n';
    for (int id : things) {
        cout << id + 1 << ' ';
    }

    return 0;
}
