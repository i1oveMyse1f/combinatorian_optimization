#include <algorithm>
#include <numeric>
#include <vector>
#include "knapsack.h"

FullyKnapsackAnswer Knapsack::solve_recover_approximation_half(long long max_w) {
    long long n = size();
    if (sorted_id.empty()) {
        sorted_id.resize(n);
        iota(sorted_id.begin(), sorted_id.end(), 0);
        sort(sorted_id.begin(), sorted_id.end(), [this](long long i, long long j) {
            return c[i] * w[j] > c[j] * w[i];
        });
        pref_w.resize(n);
        pref_c.resize(n);
        for (int i = 0; i < n; ++i) {
            pref_w[i] = (i == 0 ? 0 : pref_w[i - 1]) + w[sorted_id[i]];
            pref_c[i] = (i == 0 ? 0 : pref_c[i - 1]) + c[sorted_id[i]];
        }
    }

    long long approx_l = 0;
    long long approx_r = upper_bound(pref_w.begin(), pref_w.end(), max_w) - pref_w.begin();
    long long c_approx = approx_r == 0 ? 0 : pref_c[approx_r - 1];

    if (approx_r < n && c_approx < c[sorted_id[approx_r]] && w[sorted_id[approx_r]] <= max_w) {
        c_approx = c[sorted_id[approx_r]];
        approx_l = approx_r;
        approx_r = approx_r + 1;
    }
    vector<long long> things = vector<long long>(sorted_id.begin() + approx_l, sorted_id.begin() + approx_r);
    return { c_approx, calc_weight(things), things };
}

FullyKnapsackAnswer Knapsack::solve_recover_approximation_slow(long long max_w, double eps) {
    long long n = c.size();

    auto [c_half_approx, w_half_approx, things_half_approx] = solve_recover_approximation_half(max_w);
    if (eps >= 0.5) {
        return { c_half_approx, w_half_approx, things_half_approx };
    }
    eps /= 2;

    auto max_opt_c = static_cast<long long>(static_cast<double>(n) / eps);
    long long c_max = 2 * c_half_approx;
    vector<long long> c_scaled = c;
    for (long long& x : c_scaled) {
        x = static_cast<long long>(static_cast<double>(x) / static_cast<double>(c_max) * static_cast<double>(max_opt_c));
    }

    Knapsack scaled(c_scaled, w);
    auto dp = scaled.solve_dpc(max_opt_c);
    for (long long i = max_opt_c; i >= 0; --i) {
        if (dp[i] != INF && dp[i] <= max_w) {
            auto things = recover_things_dpc(scaled, i, dp[i]);
            long long c_opt = calc_cost(things);
            long long w_opt = calc_weight(things);
            return { c_opt, w_opt, things };
        }
    }
}

vector<long long> recover_things_dpc(const Knapsack& k, long long c_opt, long long w_opt) {
    long long n = k.size();
    if (n == 0 || w_opt == 0) {
        return {};
    }
    if (n == 1) {
        return { 0 };
    }

    long long m = n / 2;

    Knapsack left(vector<long long>(k.c.begin(), k.c.begin() + m), vector<long long>(k.w.begin(), k.w.begin() + m));
    Knapsack right(vector<long long>(k.c.begin() + m, k.c.end()), vector<long long>(k.w.begin() + m, k.w.end()));

    auto opt_left = left.solve_dpc(c_opt);
    auto opt_right = right.solve_dpc(c_opt);

    for (long long i = 0; i <= c_opt; ++i) {
        if (opt_left[i] != INF && opt_right[c_opt - i] != INF && opt_left[i] + opt_right[c_opt - i] == w_opt) {
            auto recover_left = recover_things_dpc(left, i, opt_left[i]);
            auto recover_right = recover_things_dpc(right, c_opt - i, opt_right[c_opt - i]);
            for (long long& id : recover_right) {
                id += m;
            }
            recover_left.insert(recover_left.end(), recover_right.begin(), recover_right.end());
            return recover_left;
        }
    }
}

vector<long long> recover_things_dpw(const Knapsack& k, long long c_opt, long long w_opt) {
    long long n = k.size();
    if (n == 0 || w_opt == 0) {
        return {};
    }
    if (n == 1) {
        return { 0 };
    }

    long long m = n / 2;

    Knapsack left(vector<long long>(k.c.begin(), k.c.begin() + m), vector<long long>(k.w.begin(), k.w.begin() + m));
    Knapsack right(vector<long long>(k.c.begin() + m, k.c.end()), vector<long long>(k.w.begin() + m, k.w.end()));

    auto opt_left = left.solve_dpw(w_opt);
    auto opt_right = right.solve_dpw(w_opt);

    for (long long i = 0; i <= w_opt; ++i) {
        if (opt_left[i] + opt_right[w_opt - i] == c_opt) {
            auto recover_left = recover_things_dpw(left, opt_left[i], i);
            auto recover_right = recover_things_dpw(right, opt_right[w_opt - i], w_opt - i);
            for (long long& id : recover_right) {
                id += m;
            }
            recover_left.insert(recover_left.end(), recover_right.begin(), recover_right.end());
            return recover_left;
        }
    }
}

FullyKnapsackAnswer Knapsack::solve_recover_dpw(long long max_w) const {
    auto dp = solve_dpw(max_w);
    long long w_opt = max_element(dp.begin(), dp.end()) - dp.begin();
    auto things = recover_things_dpw(*this, dp[w_opt], w_opt);
    return { dp[w_opt], w_opt, things };
}

vector<long long> Knapsack::solve_dpw(long long max_w) const {
    long long n = size();

    vector<long long> dp(max_w + 1, -INF);
    dp[0] = 0;
    for (long long i = 0; i < n; ++i) {
        for (long long j = max_w; j >= w[i]; --j) {
            if (dp[j - w[i]] != -INF) {
                dp[j] = max(dp[j], dp[j - w[i]] + c[i]);
            }
        }
    }

    return dp;
}

vector<long long> Knapsack::solve_dpc(long long max_c) const {
    long long n = size();

    vector<long long> dp(max_c + 1, INF);
    dp[0] = 0;
    for (long long i = 0; i < n; ++i) {
        for (long long j = max_c; j >= c[i]; --j) {
            if (dp[j - c[i]] != INF) {
                dp[j] = min(dp[j], dp[j - c[i]] + w[i]);
            }
        }
    }

    return dp;
}

FullyKnapsackAnswer Knapsack::solve_recover_approximation_ibarra(long long max_w, double eps) {
    long long n = size();

    auto [w_half_approx, c_half_approx, things_half_approx] = solve_recover_approximation_half(max_w);
    if (eps >= 0.5) {
        return { w_half_approx, c_half_approx, things_half_approx };
    }

    eps /= 3;

    long long max_opt_c = 1.0 / eps / eps;
    long long max_small_c = 1.0 / eps;
    long long c_max = 2 * c_half_approx;
    vector<long long> c_scaled = c;
    for (long long& x : c_scaled) {
        x = (double)x / c_max * max_opt_c;
    }

    vector<long long> c_small, c_large, w_small, w_large, id_small, id_large;
    for (long long i = 0; i < n; ++i) {
        if (c_scaled[i] < max_small_c) {
            c_small.push_back(c_scaled[i]);
            w_small.push_back(w[i]);
            id_small.push_back(i);
        } else {
            c_large.push_back(c_scaled[i]);
            w_large.push_back(w[i]);
            id_large.push_back(i);
        }
    }

    Knapsack small(c_small, w_small), large(c_large, w_large);

    auto dp = large.solve_dpc(max_opt_c);
    long long max_left_scaled = 0, max_total_scaled = -1;
    vector<long long> ans_small_things;
    for (long long i = 0; i <= max_opt_c; ++i) {
        if (dp[i] != INF && dp[i] <= max_w) {
            auto [c_half_approx_small, w_half_approx_small, cur_small_things] =
                    small.solve_recover_approximation_half(max_w - dp[i]);
            if (max_total_scaled < c_half_approx_small + i) {
                max_left_scaled = i;
                max_total_scaled = c_half_approx_small + i;
                ans_small_things = cur_small_things;
            }
        }
    }

    auto ans_large_things = recover_things_dpc(large, max_left_scaled, dp[max_left_scaled]);
    vector<long long> things;
    things.reserve(id_small.size() + id_large.size());
    for (long long id : ans_small_things) {
        things.push_back(id_small[id]);
    }
    for (long long id : ans_large_things) {
        things.push_back(id_large[id]);
    }

    long long c_opt = calc_cost(things);
    long long w_opt = calc_weight(things);
    return { c_opt, w_opt, things };
}

long long Knapsack::calc_weight(const vector<long long>& things) const {
    long long weight = 0;
    for (long long id : things) {
        weight += w[id];
    }
    return weight;
}

long long Knapsack::calc_cost(const vector<long long>& things) const {
    long long cost = 0;
    for (long long id : things) {
        cost += c[id];
    }
    return cost;
}

long long Knapsack::size() const {
    return c.size();
}
