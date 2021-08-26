#pragma GCC optimize("O3", "unroll-loops")
#pragma GCC target("sse4.2")

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <bitset>
#include <sstream>
#include <deque>
#include <queue>
#include <complex>
#include <random>
#include <cassert>
#include <chrono>
#include <functional>

using namespace std;

#define FAST ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0)
#define FIXED cout << fixed << setprecision(12)
#define ll long long
#define ld long double
#define pii pair<int, int>
#define pll pair<ll, ll>
#define graph vector<vector<int>>
#define pb push_back
#define pf push_front
#define popb pop_back
#define popf pop_front
#define f first
#define s second
#define hashmap unordered_map
#define hashset unordered_set
#define eps 1e-9
#define mod 1000000007
#define inf 3000000000000000007ll
#define sz(a) signed((a).size())
#define all(a) (a).begin(), (a).end()
#define rall(a) (a).rbegin(), (a).rend()

#ifdef DEBUG
mt19937 gen(857204);
#else
mt19937 gen(chrono::high_resolution_clock::now().time_since_epoch().count());
#endif

void flush() { cout << flush; }
void flushln() { cout << endl; }
void println() { cout << '\n'; }
template<class T> void print(const T& x) { cout << x; }
template<class T> void read(T& x) { cin >> x; }
template<class T, class ...U> void read(T& x, U& ... u) { read(x); read(u...); }
template<class T, class ...U> void print(const T& x, const U& ... u) { print(x); print(u...); }
template<class T, class ...U> void println(const T& x, const U& ... u) { print(x); println(u...); }

#ifdef DEBUG
template<class T> string pdbg(const T& x) { stringstream ss; ss << x; return ss.str(); }
template<class T, class U> string pdbg(const pair<T, U>& p) { return "{" + pdbg(p.f) + "," + pdbg(p.s) + "}"; }
string pdbg(const string& s) { return "\"" + s + "\""; }
template<class It> string pdbg(It begin, It end, string d);
template<class T> string pdbg(const vector<T>& a) { return pdbg(all(a), ""); }
template<class T> string pdbg(const vector<vector<T>>& a) { return pdbg(all(a), "\n"); }
template<class T> string pdbg(const vector<vector<vector<T>>>& a) { return pdbg(all(a), "\n\n"); }
template<class T> string pdbg(const set<T>& a) { return pdbg(all(a), ""); }
template<class T> string pdbg(const hashset<T>& a) { return pdbg(all(a), ""); }
template<class T, class U> string pdbg(const map<T, U>& a) { return pdbg(all(a), ""); }
template<class T, class U> string pdbg(const hashmap<T, U>& a) { return pdbg(all(a), ""); }
template<class It> string pdbg(It begin, It end, string d) {
    string ans;
    ans += "{";
    if (begin != end) ans += pdbg(*begin++);
    while (begin != end)
        ans += "," + d + pdbg(*begin++);
    ans += "}";
    return ans;
}
template<class T> string dbgout(const T& x) { return pdbg(x); }
template<class T, class... U>
string dbgout(T const& t, const U &... u) {
    string ans = pdbg(t);
    ans += ", ";
    ans += dbgout(u...);
    return ans;
}
#define dbg(...) print("[", #__VA_ARGS__, "] = ", dbgout(__VA_ARGS__)), flushln()
#define msg(...) print("[", __VA_ARGS__, "]"), flushln()
#else
#define dbg(...) 0
#define msg(...) 0
#endif

template<class T, class U> inline bool chmin(T& x, const U& y) { return y < x ? x = y, 1 : 0; }
template<class T, class U> inline bool chmax(T& x, const U& y) { return y > x ? x = y, 1 : 0; }
template<class T> inline void sort(T& a) { sort(all(a)); }
template<class T> inline void rsort(T& a) { sort(rall(a)); }
template<class T> inline void reverse(T& a) { reverse(all(a)); }
template<class T, class U> inline istream& operator>>(istream& str, pair<T, U>& p) { return str >> p.f >> p.s; }
template<class T> inline istream& operator>>(istream& str, vector<T>& a) { for (auto& i : a) str >> i; return str; }
template<class T> inline T sorted(T a) { sort(a); return a; }

struct line {
    vector<int> val;
    int mn;
    line() {}
    line(int _mn, int _mx) : mn(_mn), val(_mx - _mn + 1) {}
    int& operator[](int ind) { return val[ind - mn]; }
    int operator[](int ind) const { return val[ind - mn]; }
};

struct Permutation {
    vector<line> st;
    Permutation(int n) {
        st.resize(8);
        st[0] = line(0, n);
        st[1] = line(0, n);
        st[2] = line(0, 2 * n);
        st[3] = line(-n, n);
        st[4] = line(0, 3 * n);
        st[5] = line(-2 * n, n);
        st[6] = line(0, 3 * n);
        st[7] = line(-n, 2 * n);
    }
    int val = 0;
    void upd(line& l, int x, int d) {
        if (d == 1) {
            val += l[x]++;
        }
        else {
            val -= --l[x];
        }
    }
    void upd(int i, int j, int d) {
        upd(st[0], i, d);
        upd(st[1], j, d);
        upd(st[2], i + j, d);
        upd(st[3], i - j, d);
        upd(st[4], i + 2 * j, d);
        upd(st[5], i - 2 * j, d);
        upd(st[6], 2 * i + j, d);
        upd(st[7], 2 * i - j, d);
    }
};

void makeswap(Permutation& p, vector<int>& val, int i, int j) {
    p.upd(i, val[i], -1);
    p.upd(j, val[j], -1);
    swap(val[i], val[j]);
    p.upd(i, val[i], 1);
    p.upd(j, val[j], 1);
}

signed main() {
    FAST; FIXED;
    int n, k;
    read(n, k);
    Permutation start(n);
    vector<bool> used(n), usedval(n);
    vector<int> startval(n, -1);
    for (int i = 0; i < k; ++i) {
        int x, y;
        read(x, y);
        --x, --y;
        start.upd(x, y, 1);
        used[x] = 1;
        usedval[y] = 1;
        startval[x] = y;
    }
    if (start.val != 0) {
        println("NO");
        return 0;
    }
    vector<int> unused, perm;
    for (int i = 0; i < n; ++i) {
        if (!used[i]) unused.pb(i);
        if (!usedval[i]) perm.pb(i);
    }

    auto rand01 = std::uniform_real_distribution<double>();

    while (clock() < 9.5 * CLOCKS_PER_SEC) {
        shuffle(all(perm), gen);
        auto val = startval;
        Permutation curr = start;
        for (int i = 0; i < sz(perm); ++i) {
            curr.upd(unused[i], perm[i], 1);
            val[unused[i]] = perm[i];
        }

        double T = 1;

        for (int global_iter = 0; global_iter < 5 * n * n; ++global_iter) {
            for (int local_iter = 0; local_iter < 20; ++local_iter) {
                int a = unused[gen() % sz(unused)];
                int b = unused[gen() % sz(unused)];
                int was = curr.val;
                makeswap(curr, val, a, b);

                if (curr.val > was && rand01(gen) > exp((was - curr.val) / T)) {
                    makeswap(curr, val, a, b);
                }
                else {
                    break;
                }
            }

            T *= 0.993;
        }

        if (curr.val == 0) {
            println("YES");
            for (auto i : val)
                println(i + 1);
            return 0;
        }
    }
    println("NO");
#ifdef DEBUG
    cerr << "Runtime is: " << clock() * 1.0 / CLOCKS_PER_SEC << endl;
#endif
    return 0;
}
