#include <iostream>
#include <vector>
#include "knapsack.h"

using namespace std;

void update_best(FullyKnapsackAnswer& best, const FullyKnapsackAnswer& current) {
    if (best.c_opt < current.c_opt) {
        best = current;
    }
}

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

    auto best = k.solve_recover_approximation_ibarra(max_w, 0.1);

    auto center_meet_in_the_middle = k.solve_recover_on_center_optimization(max_w, 15, 25,
                                                                            &Knapsack::solve_recover_meet_in_the_middle);
    update_best(best, center_meet_in_the_middle);

    auto center_slow = k.solve_recover_on_center_optimization(max_w, 100, 500,
                                                              &Knapsack::solve_recover_approximation_slow, 0.01);
    update_best(best, center_slow);

    center_slow = k.solve_recover_on_center_optimization(max_w, 50, 250,
                                                         &Knapsack::solve_recover_approximation_slow, 0.005);
    update_best(best, center_slow);

    cout << best.c_opt << '\n';
    for (int id : best.things) {
        cout << id + 1 << ' ';
    }

    return 0;
}
