#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

vector<int> solve_knapsack(const vector<int>& c, const vector<int>& w, int max_w) {
    int n = c.size();

    vector<int> dp(max_w + 1, -1);
    dp[0] = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = max_w; j >= w[i]; --j) {
            if (dp[j - w[i]] != -1) {
                dp[j] = max(dp[j], dp[j - w[i]] + c[i]);
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

    auto opt_left = solve_knapsack(c_left, w_left, w_opt);
    auto opt_right = solve_knapsack(c_right, w_right, w_opt);

    for (int i = 0; i <= w_opt; ++i) {
        if (opt_left[i] + opt_right[w_opt - i] == c_opt) {
            auto recover_left = recover_answer(c_left, w_left, opt_left[i], i);
            auto recover_right = recover_answer(c_right, w_right, opt_right[w_opt - i], w_opt - i);
            for (int& id : recover_right) {
                id += m;
            }
            recover_left.insert(recover_left.end(), recover_right.begin(), recover_right.end());
            return recover_left;
        }
    }

    throw 1;
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

    auto dp = solve_knapsack(c, w, max_w);
    int w_opt = max_element(dp.begin(), dp.end()) - dp.begin();
    auto things = recover_answer(c, w, dp[w_opt], w_opt);

    cout << dp[w_opt] << '\n';
    for (int id : things) {
        cout << id + 1 << ' ';
    }

    return 0;
}
