#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <vector>
#include "glpk.h"
#include <chrono>
#include <cstring>
#include <tuple>

const int N = 501;
std::vector<std::tuple<int, int, int>> edges;
double x[N];
int used[N];
std::vector<std::pair<int, int>> gr[N];
int n, m;

const double EPS = 1e-7;

int start_cycle;
bool finish_cycle;

std::vector<int> cycle_ids;

bool dfs(int v, int lvl) {
    used[v] = lvl;
    for (auto e : gr[v]) {
        int u = e.first;
        int id = e.second;
        if (x[id] > EPS && x[id] < 1 - EPS) {
            if (used[u] == lvl) {
                start_cycle = u;
                cycle_ids.push_back(id);
                return true;
            } else if (used[u] == -1 && dfs(u, 1 - lvl)) {
                if (!finish_cycle) {
                    cycle_ids.push_back(id);
                    if (v == start_cycle)
                        finish_cycle = true;
                }
                return true;
            }
        }
    }
    return false;
}

void foo_bar(glp_tree *tree, void *info) {
    switch (glp_ios_reason(tree)) {
        case GLP_IROWGEN:
            glp_prob *cur = glp_ios_get_prob(tree);

            for (int i = 0; i < m; ++i) {
                x[i] = glp_get_col_prim(cur, i + 1);
            }

            memset(used, -1, sizeof used);
            finish_cycle = false;
            cycle_ids.clear();

            for (int i = 0; i < n; ++i) {
                if (used[i] == -1 && dfs(i, 0)) {
                    int cnt = cycle_ids.size();
                    int rows = glp_get_num_rows(cur);
                    glp_add_rows(cur, 1);
                    glp_set_row_bnds(cur, rows + 1, GLP_UP, 0.0, (cnt - 1) / 2);
                    auto* ind = new int[cnt + 1];
                    auto* val = new double[cnt + 1];
                    for (int j = 0; j < cnt; ++j) {
                        ind[j + 1] = cycle_ids[j] + 1;
                        val[j + 1] = 1;
                    }
                    glp_set_mat_row(cur, rows + 1, cnt, ind, val);
                    return;
                }
            }
    }
}

int main() {
    freopen("input.txt", "r", stdin);

    std::cin >> n >> m;
    edges.resize(m);
    for (int i = 0; i < m; ++i) {
        int v, u, w;
        std::cin >> v >> u >> w;
        gr[v].emplace_back(u, i);
        gr[u].emplace_back(v, i);
        edges[i] = std::tuple<int, int, int>(v, u, w);
    }

    glp_prob *lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);
    glp_add_rows(lp, n);
    for (int i = 0; i < n; ++i) {
        glp_set_row_bnds(lp, i + 1, GLP_FX, 1.0, 1.0);
    }

    glp_add_cols(lp, m);

    int nonzeros = m * 2;
    int ia[1 + nonzeros], ja[1 + nonzeros];
    double ar[1 + nonzeros];
    int top_mt = 1;

    for (int i = 0; i < m; ++i) {
        int v = std::get<0>(edges[i]);
        int u = std::get<1>(edges[i]);
        int w = std::get<2>(edges[i]);

        ia[top_mt] = v + 1;
        ja[top_mt] = i + 1;
        ar[top_mt] = 1;
        ++top_mt;

        ia[top_mt] = u + 1;
        ja[top_mt] = i + 1;
        ar[top_mt] = 1;
        ++top_mt;

        glp_set_obj_coef(lp, i + 1, w);
    }

    for (int i = 0; i < m; ++i) {
        glp_set_col_bnds(lp, i + 1, GLP_DB, 0.0, 1.0);
    }

    glp_load_matrix(lp, top_mt - 1, ia, ja, ar);

    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.cb_func = foo_bar;
    parm.presolve = GLP_ON;
    parm.msg_lev = GLP_MSG_OFF;
    //parm.gmi_cuts = GLP_ON; //x3
    //parm.mir_cuts = GLP_ON; // ultra fast
    //parm.cov_cuts = GLP_ON; // no updates
    //parm.clq_cuts = GLP_ON; // no updates

    cycle_ids.reserve(n);

    glp_intopt(lp, &parm);

    std::cout << glp_mip_obj_val(lp) << '\n';

    for (int i = 0; i < m; ++i) {
        if (glp_mip_col_val(lp, i + 1) > 0) {
            std::cout << i << ' ';
        }
    }

    //return 0;
    glp_delete_prob(lp);
}
