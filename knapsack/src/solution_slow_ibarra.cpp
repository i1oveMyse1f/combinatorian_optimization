#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

#define int long long

const int INF = 1e18;

vector<int> solve_knapsack(const vector<int>& c, const vector<int>& w, int max_c) {
    int n = c.size();

    vector<int> dp(max_c + 1, INF);
    dp[0] = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = max_c; j >= c[i]; --j) {
            if (dp[j - c[i]] != INF) {
                dp[j] = min(dp[j], dp[j - c[i]] + w[i]);
            }
        }
    }

    return dp;
}

vector<int> recover_answer(const vector<int>& c, const vector<int>& w, int c_opt, int w_opt) {
    int n = c.size();
    if (n == 0 || w_opt == 0) {
        return {};
    }
    if (n == 1) {
        return { 0 };
    }

    int m = n / 2;
    auto c_left = vector<int>(c.begin(), c.begin() + m), c_right = vector<int>(c.begin() + m, c.end());
    auto w_left = vector<int>(w.begin(), w.begin() + m), w_right = vector<int>(w.begin() + m, w.end());

    auto opt_left = solve_knapsack(c_left, w_left, c_opt);
    auto opt_right = solve_knapsack(c_right, w_right, c_opt);

    for (int i = 0; i <= c_opt; ++i) {
        if (opt_left[i] != INF && opt_right[c_opt - i] != INF && opt_left[i] + opt_right[c_opt - i] == w_opt) {
            auto recover_left = recover_answer(c_left, w_left, i, opt_left[i]);
            auto recover_right = recover_answer(c_right, w_right, c_opt - i, opt_right[c_opt - i]);
            for (int& id : recover_right) {
                id += m;
            }
            recover_left.insert(recover_left.end(), recover_right.begin(), recover_right.end());
            return recover_left;
        }
    }

    exit(1);
}

pair<int, vector<int>> get_half_approximation(const vector<int>& c, const vector<int>& w, int max_w) {
    int n = c.size();
    vector<int> id(n);
    iota(id.begin(), id.end(), 0);
    sort(id.begin(), id.end(), [&c, &w](int i, int j){
       return c[i] * w[j] > c[j] * w[i];
    });
    int c_approx = 0, w_approx = 0;
    int i = 0;
    for (; i < n && w_approx + w[id[i]] <= max_w; ++i) {
        w_approx += w[id[i]];
        c_approx += c[id[i]];
    }
    if (i < n) {
        c_approx = max(c_approx, c[id[i]]);
    }
    return { c_approx, vector<int>(id.begin(), id.begin() + i) };
}

int calc_cost(const vector<int>& c, const vector<int>& things) {
    int cost = 0;
    for (int id : things) {
        cost += c[id];
    }
    return cost;
}

pair<int, vector<int>> get_eps_approximation(const vector<int>& c, const vector<int>& w, int max_w, double eps) {
    int n = c.size();

    auto [_, things_half_approx] = get_half_approximation(c, w, max_w);
    int c_half_approx = calc_cost(c, things_half_approx);
    if (eps >= 0.5) {
        return { c_half_approx, things_half_approx };
    }
    eps /= 2;

    int max_opt_c = n / eps;
    int c_max = 2 * c_half_approx;
    vector<int> c_scaled = c;
    for (int& x : c_scaled) {
        x = (double)x / c_max * max_opt_c;
    }

    auto dp = solve_knapsack(c_scaled, w, max_opt_c);
    for (int i = max_opt_c; i >= 0; --i) {
        if (dp[i] != INF && dp[i] <= max_w) {
            auto things = recover_answer(c_scaled, w, i, dp[i]);
            int c_opt = calc_cost(c, things);
            return { c_opt, things };
        }
    }

    exit(2);
}

signed main() {
    //freopen("data/1.public", "r", stdin);
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    int n, max_w;
    cin >> n >> max_w;

    vector<int> c(n), w(n);
    for (int i = 0; i < n; ++i) {
        cin >> c[i] >> w[i];
    }

    auto [c_opt, things] = get_eps_approximation(c, w, max_w, 0.1);

    cout << c_opt << '\n';
    for (int id : things) {
        cout << id + 1 << ' ';
    }

    return 0;
}
