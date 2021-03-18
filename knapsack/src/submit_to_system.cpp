#include <algorithm>
#include <iostream>
#include <vector>


using namespace std;

#ifndef KNAPSACK_KNAPSACK_H
#define KNAPSACK_KNAPSACK_H

#include <vector>

using namespace std;

const long long INF = 1e9;

struct FullyKnapsackAnswer {
    long long c_opt;
    long long w_opt;
    vector<long long> things;
    FullyKnapsackAnswer(long long c_opt, long long w_opt, const vector<long long>& things): c_opt(c_opt), w_opt(w_opt), things(things) {}
    FullyKnapsackAnswer(): c_opt(0), w_opt(0) {}
};

class Knapsack {
public:
    Knapsack(const vector<long long>& c, const vector<long long>& w): c(c), w(w) {}

    FullyKnapsackAnswer solve_recover_approximation_half(long long max_w);
    FullyKnapsackAnswer solve_recover_branch_bound(long long max_w);

    long long calc_weight(const vector<long long>& things) const;
    int size() const;

    friend void rec(Knapsack& k, int i, int min_opt, int max_w, FullyKnapsackAnswer& cur, FullyKnapsackAnswer& ans);

private:
    vector<long long> c, w;
    vector<long long> pref_w, pref_c, sorted_id;
};

#endif //KNAPSACK_KNAPSACK_H

#include <numeric>

int Knapsack::size() const {
    return c.size();
}

long long Knapsack::calc_weight(const vector<long long>& things) const {
    long long weight = 0;
    for (long long id : things) {
        weight += w[id];
    }
    return weight;
}

FullyKnapsackAnswer Knapsack::solve_recover_approximation_half(long long max_w) {
    long long n = size();
    if (sorted_id.empty()) {
        sorted_id.resize(n);
        iota(sorted_id.begin(), sorted_id.end(), 0);
        sort(sorted_id.begin(), sorted_id.end(), [this](long long i, long long j) {
            return c[i] * w[j] > c[j] * w[i];
        });
    }

    if (pref_w.empty()) {
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
    return FullyKnapsackAnswer(c_approx, calc_weight(things), things);
}


void rec(Knapsack& k, int i, int min_opt, int max_w, FullyKnapsackAnswer& cur, FullyKnapsackAnswer& ans) {
    if (max_w < 0)
        exit(1);

    if (k.size() == 0) {
        if (ans.c_opt < cur.c_opt) {
            ans.c_opt = cur.c_opt;
            ans.w_opt = cur.w_opt;
            ans.things = cur.things;
            //ans = cur;
        }
        return;
    }

    Knapsack next_k(vector<long long>(k.c.begin() + 1, k.c.end()), vector<long long>(k.w.begin() + 1, k.w.end()));

    auto next_k_approx = next_k.solve_recover_approximation_half(max_w);
    auto c_skip_approx = next_k_approx.c_opt;
    long long max_c_skip = 2 * c_skip_approx;
    if (k.w[0] > max_w) {
        rec(next_k, i + 1, min_opt, max_w, cur, ans);
        return;
    }

    auto next_k_approx_take = next_k.solve_recover_approximation_half(max_w - k.w[0]);
    auto c_take_approx = next_k_approx.c_opt;
    long long max_c_take = 2 * c_take_approx + k.c[0];
    c_take_approx += k.c[0];

    min_opt = std::max<long long>({ min_opt, cur.c_opt + c_skip_approx, cur.c_opt + c_take_approx });

    if (c_take_approx < c_skip_approx) {
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

    if (c_take_approx >= c_skip_approx && cur.c_opt + max_c_skip >= min_opt) {
        rec(next_k, i + 1, min_opt, max_w, cur, ans);
    } else if (cur.c_opt + max_c_take >= min_opt) {
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

    vector<int> sorted(n), rev(n);
    iota(sorted.begin(), sorted.end(), 0);
    sort(sorted.begin(), sorted.end(), [this](int i, int j) {
        return c[i] > c[j];
    });

    vector<long long> nc(n), nw(n);
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


int main() {
    //freopen("input.txt", "r", stdin);
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    int n, max_w;
    cin >> n >> max_w;

    vector<long long> c(n), w(n);
    //for (int i = 0; i < n; ++i) {
    //    cin >> c[i] >> w[i];
    //}
    for (auto& x : w)
        cin >> x;
    for (auto& x : c)
        cin >> x;

    Knapsack k(c, w);
    auto res = k.solve_recover_branch_bound(max_w);

    for (int id : res.things) {
        cout << id + 1 << '\n';
    }

    return 0;
}
