#ifndef KNAPSACK_KNAPSACK_H
#define KNAPSACK_KNAPSACK_H

#include <vector>

using namespace std;

const long long INF = 1e9;

struct FullyKnapsackAnswer {
    long long c_opt;
    long long w_opt;
    vector<long long> things;
};

class Knapsack {
public:
    Knapsack(const vector<long long>& c, const vector<long long>& w): c(c), w(w) {}

    [[nodiscard]] FullyKnapsackAnswer solve_recover_dpw(long long max_w) const;

    [[nodiscard]] vector<long long> solve_dpw(long long max_w) const;
    [[nodiscard]] vector<long long> solve_dpc(long long max_c) const;

    friend vector<long long> recover_things_dpw(const Knapsack& k, long long c_opt, long long w_opt);
    friend vector<long long> recover_things_dpc(const Knapsack& k, long long c_opt, long long w_opt);

    FullyKnapsackAnswer solve_recover_approximation_half(long long max_w);
    FullyKnapsackAnswer solve_recover_approximation_slow(long long max_w, double eps);
    FullyKnapsackAnswer solve_recover_approximation_ibarra(long long max_w, double eps);

    [[nodiscard]] long long calc_weight(const vector<long long>& things) const;
    [[nodiscard]] long long calc_cost(const vector<long long>& things) const;
    [[nodiscard]] long long size() const;

private:
    vector<long long> c, w;
    vector<long long> pref_w, pref_c, sorted_id;
};

#endif //KNAPSACK_KNAPSACK_H
