#ifndef KNAPSACK_KNAPSACK_H
#define KNAPSACK_KNAPSACK_H

#include <algorithm>
#include <numeric>
#include <vector>

using namespace std;

const long long INF = 1e9;

struct FullyKnapsackAnswer {
    long long c_opt = 0;
    long long w_opt = 0;
    vector<int> things;
};

class Knapsack {
public:
    Knapsack(const vector<int>& c, const vector<int>& w): c(c), w(w) {}

    [[nodiscard]] FullyKnapsackAnswer solve_recover_dpw(long long max_w) const;

    [[nodiscard]] vector<long long> solve_dpw(long long max_w) const;
    [[nodiscard]] vector<long long> solve_dpc(long long max_c) const;

    friend vector<int> recover_things_dpw(const Knapsack& k, long long c_opt, long long w_opt);
    friend vector<int> recover_things_dpc(const Knapsack& k, long long c_opt, long long w_opt);

    FullyKnapsackAnswer solve_recover_approximation_half(long long max_w);
    FullyKnapsackAnswer solve_recover_approximation_slow(long long max_w, double eps);
    FullyKnapsackAnswer solve_recover_approximation_ibarra(long long max_w, double eps);

    [[nodiscard]] long long calc_weight(const vector<int>& things) const;
    [[nodiscard]] long long calc_cost(const vector<int>& things) const;
    [[nodiscard]] int size() const;

    FullyKnapsackAnswer solve_recover_branch_bound(long long max_w);
    FullyKnapsackAnswer solve_recover_meet_in_the_middle(long long max_w);

    friend void rec(Knapsack& k, int i, int min_opt, int max_w, FullyKnapsackAnswer& cur, FullyKnapsackAnswer& ans);

    template<typename Func, typename... Args>
    FullyKnapsackAnswer solve_recover_on_center_optimization(int max_w, int kL, int kR, Func f, Args&&... args);

private:
    vector<int> c, w;
    vector<long long> pref_w, pref_c;
    vector<int> sorted_id;

    void init_sorted_id();
    void init_pref();
};

//TODO: to knapsack.cpp
template<typename Func, typename... Args>
FullyKnapsackAnswer Knapsack::solve_recover_on_center_optimization(int max_w, int kL, int kR, Func f, Args&&... args) {
    int n = size();
    init_sorted_id();
    init_pref();

    int approx_l = 0;
    int approx_r = upper_bound(pref_w.begin(), pref_w.end(), max_w) - pref_w.begin();
    long long c_approx = approx_r == 0 ? 0 : pref_c[approx_r - 1];
    long long w_approx = approx_r == 0 ? 0 : pref_w[approx_r - 1];

    if (approx_r < n && c_approx < c[sorted_id[approx_r]] && w[sorted_id[approx_r]] <= max_w) {
        c_approx = c[sorted_id[approx_r]];
        w_approx = w[sorted_id[approx_r]];
        approx_l = approx_r;
        approx_r = approx_r + 1;
    }

    int center_l = max(0, approx_r - kL);
    int center_r = min(n, approx_r + kR);

    vector<int> center_w(center_r - center_l), center_c(center_r - center_l);
    for (int i = center_l; i < center_r; ++i) {
        center_c[i - center_l] = c[sorted_id[i]];
        center_w[i - center_l] = w[sorted_id[i]];
    }

    Knapsack center_k(center_c, center_w);
    auto [center_opt_c, center_opt_w, center_things] = (center_k.*f)(max_w - (center_l == 0 ? 0 : pref_w[center_l - 1]), std::forward<Args>(args)...);
    for (auto& id : center_things) {
        id = sorted_id[center_l + id];
    }

    long long total_c = (center_l == 0 ? 0 : pref_c[center_l - 1]) + center_opt_c;
    long long total_w = (center_l == 0 ? 0 : pref_w[center_l - 1]) + center_opt_w;

    auto things = vector<int>(sorted_id.begin() + min(approx_l, center_l), sorted_id.begin() + center_l);
    things.insert(things.end(), center_things.begin(), center_things.end());

    for (int i = 0; i < min(center_l, approx_l); ++i) {
        if (total_w + w[sorted_id[i]] <= max_w) {
            total_w += w[sorted_id[i]];
            total_c += c[sorted_id[i]];
            things.push_back(sorted_id[i]);
        }
    }

    for (int i = max(center_r, approx_r); i < n; ++i) {
        if (total_w + w[sorted_id[i]] <= max_w) {
            total_w += w[sorted_id[i]];
            total_c += c[sorted_id[i]];
            things.push_back(sorted_id[i]);
        }
    }

    return { total_c, total_w, things };
}

#endif //KNAPSACK_KNAPSACK_H
