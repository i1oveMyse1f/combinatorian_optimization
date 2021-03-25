#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx")
#pragma GCC optimize 03
#pragma GCC optimize("unroll-loops")

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

#include <algorithm>
#include <numeric>
#include <tuple>
#include <vector>

FullyKnapsackAnswer Knapsack::solve_recover_approximation_half(long long max_w) {
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

    auto things = vector<int>(sorted_id.begin() + approx_l, sorted_id.begin() + approx_r);

    for (int i = 0; i < approx_l; ++i) {
        if (w_approx + w[sorted_id[i]] <= max_w) {
            w_approx += w[sorted_id[i]];
            c_approx += c[sorted_id[i]];
            things.push_back(sorted_id[i]);
        }
    }

    for (int i = approx_r; i < n; ++i) {
        if (w_approx + w[sorted_id[i]] <= max_w) {
            w_approx += w[sorted_id[i]];
            c_approx += c[sorted_id[i]];
            things.push_back(sorted_id[i]);
        }
    }

    return { c_approx, w_approx, things };
}

FullyKnapsackAnswer Knapsack::solve_recover_approximation_slow(long long max_w, double eps) {
    int n = c.size();

    auto [c_half_approx, w_half_approx, things_half_approx] = solve_recover_approximation_half(max_w);
    if (eps >= 0.5) {
        return { c_half_approx, w_half_approx, things_half_approx };
    }
    eps /= 2;

    auto max_opt_c = static_cast<long long>(static_cast<double>(n) / eps);
    long long c_max = 2 * c_half_approx;
    auto c_scaled = c;
    for (auto& x : c_scaled) {
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
    exit(1);
}

vector<int> recover_things_dpc(const Knapsack& k, long long c_opt, long long w_opt) {
    int n = k.size();
    if (n == 0 || w_opt == 0) {
        return {};
    }
    if (n == 1) {
        return { 0 };
    }

    long long m = n / 2;

    Knapsack left(vector<int>(k.c.begin(), k.c.begin() + m), vector<int>(k.w.begin(), k.w.begin() + m));
    Knapsack right(vector<int>(k.c.begin() + m, k.c.end()), vector<int>(k.w.begin() + m, k.w.end()));

    auto opt_left = left.solve_dpc(c_opt);
    auto opt_right = right.solve_dpc(c_opt);

    for (long long i = 0; i <= c_opt; ++i) {
        if (opt_left[i] != INF && opt_right[c_opt - i] != INF && opt_left[i] + opt_right[c_opt - i] == w_opt) {
            auto recover_left = recover_things_dpc(left, i, opt_left[i]);
            auto recover_right = recover_things_dpc(right, c_opt - i, opt_right[c_opt - i]);
            for (auto& id : recover_right) {
                id += m;
            }
            recover_left.insert(recover_left.end(), recover_right.begin(), recover_right.end());
            return recover_left;
        }
    }
    exit(1);
}

vector<int> recover_things_dpw(const Knapsack& k, long long c_opt, long long w_opt) {
    int n = k.size();
    if (n == 0 || w_opt == 0) {
        return {};
    }
    if (n == 1) {
        return { 0 };
    }

    long long m = n / 2;

    Knapsack left(vector<int>(k.c.begin(), k.c.begin() + m), vector<int>(k.w.begin(), k.w.begin() + m));
    Knapsack right(vector<int>(k.c.begin() + m, k.c.end()), vector<int>(k.w.begin() + m, k.w.end()));

    auto opt_left = left.solve_dpw(w_opt);
    auto opt_right = right.solve_dpw(w_opt);

    for (long long i = 0; i <= w_opt; ++i) {
        if (opt_left[i] + opt_right[w_opt - i] == c_opt) {
            auto recover_left = recover_things_dpw(left, opt_left[i], i);
            auto recover_right = recover_things_dpw(right, opt_right[w_opt - i], w_opt - i);
            for (auto& id : recover_right) {
                id += m;
            }
            recover_left.insert(recover_left.end(), recover_right.begin(), recover_right.end());
            return recover_left;
        }
    }

    exit(1);
}

FullyKnapsackAnswer Knapsack::solve_recover_dpw(long long max_w) const {
    auto dp = solve_dpw(max_w);
    long long w_opt = max_element(dp.begin(), dp.end()) - dp.begin();
    auto things = recover_things_dpw(*this, dp[w_opt], w_opt);
    return { dp[w_opt], w_opt, things };
}

vector<long long> Knapsack::solve_dpw(long long max_w) const {
    int n = size();

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
    int n = size();

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
    int n = size();

    auto [w_half_approx, c_half_approx, things_half_approx] = solve_recover_approximation_half(max_w);
    if (eps >= 0.5) {
        return { w_half_approx, c_half_approx, things_half_approx };
    }

    eps /= 3;

    long long max_opt_c = 1.0 / eps / eps;
    long long max_small_c = 1.0 / eps;
    long long c_max = 2 * c_half_approx;
    auto c_scaled = c;
    for (auto& x : c_scaled) {
        x = (double)x / c_max * max_opt_c;
    }

    vector<int> c_small, c_large, w_small, w_large, id_small, id_large;
    for (int i = 0; i < n; ++i) {
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
    vector<int> ans_small_things;
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
    vector<int> things;
    things.reserve(id_small.size() + id_large.size());
    for (auto id : ans_small_things) {
        things.push_back(id_small[id]);
    }
    for (auto id : ans_large_things) {
        things.push_back(id_large[id]);
    }

    long long c_opt = calc_cost(things);
    long long w_opt = calc_weight(things);
    return { c_opt, w_opt, things };
}

long long Knapsack::calc_weight(const vector<int>& things) const {
    long long weight = 0;
    for (long long id : things) {
        weight += w[id];
    }
    return weight;
}

long long Knapsack::calc_cost(const vector<int>& things) const {
    long long cost = 0;
    for (long long id : things) {
        cost += c[id];
    }
    return cost;
}

int Knapsack::size() const {
    return c.size();
}

void rec(Knapsack& k, int i, int min_opt, int max_w, FullyKnapsackAnswer& cur, FullyKnapsackAnswer& ans) {
    if (k.size() == 0) {
        if (ans.c_opt < cur.c_opt) {
            ans = cur;
        }
        return;
    }

    Knapsack next_k(vector<int>(k.c.begin() + 1, k.c.end()), vector<int>(k.w.begin() + 1, k.w.end()));

    auto [c_skip_approx, w_skip, things_skip] = next_k.solve_recover_approximation_half(max_w);
    if (k.w[0] > max_w) {
        rec(next_k, i + 1, min_opt, max_w, cur, ans);
        return;
    }

    auto [c_take_approx, w_take, things_take] = next_k.solve_recover_approximation_half(max_w - k.w[0]);

    long long max_c_take = 2 * c_take_approx, max_c_skip = 2 * c_skip_approx;

    if (min_opt > cur.c_opt + max(2 * c_skip_approx, 2 * c_take_approx + k.c[0]))
        return;

    min_opt = std::max<long long>({ min_opt, cur.c_opt + c_skip_approx, cur.c_opt + c_take_approx + k.c[0] });

    if (c_take_approx + k.c[0] < c_skip_approx) {
        rec(next_k, i + 1, min_opt, max_w, cur, ans);
    } else {
        cur.c_opt += k.c[0];
        cur.w_opt += k.w[0];
        cur.things.push_back(i);
        rec(next_k, i + 1, min_opt, max_w - k.w[0], cur, ans);
        cur.c_opt -= k.c[0];
        cur.w_opt -= k.w[0];
        cur.things.pop_back();
    }

    min_opt = max<long long>(min_opt, ans.c_opt);

    if (c_take_approx + k.c[0] >= c_skip_approx && cur.c_opt + 2 * c_skip_approx > min_opt) {
        rec(next_k, i + 1, min_opt, max_w, cur, ans);
    } else if (cur.c_opt + 2 * c_take_approx + k.c[0] > min_opt) {
        cur.c_opt += k.c[0];
        cur.w_opt += k.w[0];
        cur.things.push_back(i);
        rec(next_k, i + 1, min_opt, max_w - k.w[0], cur, ans);
        cur.c_opt -= k.c[0];
        cur.w_opt -= k.w[0];
        cur.things.pop_back();
    }
}

FullyKnapsackAnswer Knapsack::solve_recover_branch_bound(long long max_w) {
    int n = size();

    vector<int> sorted(n);
    iota(sorted.begin(), sorted.end(), 0);
    sort(sorted.begin(), sorted.end(), [this](int i, int j) {
        return c[i] > c[j];
    });

    vector<int> nc(n), nw(n);
    for (int i = 0; i < n; ++i) {
        nc[i] = c[sorted[i]];
        nw[i] = w[sorted[i]];
    }

    Knapsack k(nc, nw);
    FullyKnapsackAnswer cur, ans;
    rec(k, 0, 0, max_w, cur, ans);

    for (auto& id : ans.things) {
        id = sorted[id];
    }

    return ans;
}


FullyKnapsackAnswer Knapsack::solve_recover_meet_in_the_middle(long long max_w) {
    int n = size();
    int m = n / 2;

    int P = 1 << m;
    vector<tuple<int, int, int>> all_left(P);

    int w_best = 0, c_best = 0, mask_best_left = 0;

    for (int mask = 0; mask < P; ++mask) {
        int w_total = 0, c_total = 0;
        for (int i = 0; i < m; ++i) {
            if ((mask >> i) & 1) {
                w_total += w[i];
                c_total += c[i];
            }
        }
        all_left[mask] = { w_total, c_total, mask };
        if (w_total <= max_w) {
            w_best = w_total;
            c_best = c_total;
            mask_best_left = mask;
        }
    }

    sort(all_left.rbegin(), all_left.rend());

    int j = 0;
    for (int i = 1; i < all_left.size(); ++i) {
        if (get<1>(all_left[i - 1]) < get<1>(all_left[i])) {
            all_left[j] = all_left[i];
        } else {
            ++j;
        }
    }
    ++j;

    all_left.erase(all_left.begin() + j, all_left.end());
    reverse(all_left.begin(), all_left.end());

    int P2 = 1 << (n - m);

    int mask_best_right = 0;

    for (int mask = 0; mask < P2; ++mask) {
        int w_right = 0, c_right = 0;
        for (int i = 0; i < (n - m); ++i) {
            if ((mask >> i) & 1) {
                w_right += w[i + m];
                c_right += c[i + m];
            }
        }

        int pos = lower_bound(all_left.begin(), all_left.end(), make_tuple(max_w - w_right + 1, 0, 0)) - all_left.begin();
        if (pos == 0)
            continue;
        --pos;

        auto [w_left, c_left, mask_left] = all_left[pos];
        if (c_left + c_right > c_best) {
            c_best = c_left + c_right;
            w_best = w_left + w_right;
            mask_best_left = mask_left;
            mask_best_right = mask;
        }
    }

    vector<int> things;
    for (int i = 0; i < m; ++i) {
        if ((mask_best_left >> i) & 1) {
            things.push_back(i);
        }
    }

    for (int i = 0; i < n - m; ++i) {
        if ((mask_best_right >> i) & 1) {
            things.push_back(i + m);
        }
    }
    return { c_best, w_best, things};
}

void Knapsack::init_sorted_id() {
    if (sorted_id.empty()) {
        int n = size();
        sorted_id.resize(n);
        iota(sorted_id.begin(), sorted_id.end(), 0);
        sort(sorted_id.begin(), sorted_id.end(), [this](long long i, long long j) {
            return c[i] * 1LL * w[j] > c[j] * 1LL * w[i];
        });
    };
}

void Knapsack::init_pref() {
    if (pref_w.empty()) {
        int n = size();
        pref_w.resize(n);
        pref_c.resize(n);
        for (int i = 0; i < n; ++i) {
            pref_w[i] = (i == 0 ? 0 : pref_w[i - 1]) + w[sorted_id[i]];
            pref_c[i] = (i == 0 ? 0 : pref_c[i - 1]) + c[sorted_id[i]];
        }
    }
}

#include <iostream>
#include <vector>

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

    auto best = k.solve_recover_approximation_ibarra(max_w, 0.05);

    auto center_meet_in_the_middle = k.solve_recover_on_center_optimization(max_w, 15, 30,
                                                                            &Knapsack::solve_recover_meet_in_the_middle);
    update_best(best, center_meet_in_the_middle);

    auto center_slow = k.solve_recover_on_center_optimization(max_w, 100, 500,
                                                              &Knapsack::solve_recover_approximation_slow, 0.0005);
    update_best(best, center_slow);

    center_slow = k.solve_recover_on_center_optimization(max_w, 50, 250,
                                                         &Knapsack::solve_recover_approximation_slow, 0.0005);
    update_best(best, center_slow);

    cout << best.c_opt << '\n';
    for (int id : best.things) {
        cout << id + 1 << ' ';
    }

    return 0;
}
