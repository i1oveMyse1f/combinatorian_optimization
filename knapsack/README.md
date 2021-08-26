# Knapsack problem

The statements of the problem available at [knapsack.pdf](knapsack.pdf).

All solutions wrote on C++

* [knapsack.h](src/knapcask.h) is knapcask-solver class
* [solve_recover_dpw](https://github.com/i1oveMyse1f/combinatorian_optimization/blob/4d0d6707b31ac620060fb4369f968341853bddbd/knapsack/src/knapsack.h#L22) is basic dp-solution with divide-and-conqueror technique to recover the answer; O(nW) time, O(W) memory.
* [solve_recover_approximation_half](https://github.com/i1oveMyse1f/combinatorian_optimization/blob/4d0d6707b31ac620060fb4369f968341853bddbd/knapsack/src/knapsack.h#L30) is half-approximation of knapsack. O(n log n) first time, O(n) in next times.
* [solve_recover_approximation_slow](https://github.com/i1oveMyse1f/combinatorian_optimization/blob/4d0d6707b31ac620060fb4369f968341853bddbd/knapsack/src/knapsack.h#L31) is first Ibarra algorithm that working in O(n log n + n^2/eps) time
* [solve_recover_approximation_ibarra](https://github.com/i1oveMyse1f/combinatorian_optimization/blob/4d0d6707b31ac620060fb4369f968341853bddbd/knapsack/src/knapsack.h#L32) is Ibarra algorithm that working in O(n log n + n/eps^2) time
* [solve_recover_meet_in_the_middle](https://github.com/i1oveMyse1f/combinatorian_optimization/blob/4d0d6707b31ac620060fb4369f968341853bddbd/knapsack/src/knapsack.h#L39) is NP-completeness solver of knapsack, O(n2^(n/2)) using meet-in-the-middle technique
* [solve_recover_branch_bound](https://github.com/i1oveMyse1f/combinatorian_optimization/blob/4d0d6707b31ac620060fb4369f968341853bddbd/knapsack/src/knapsack.h#L38) is NP-completeness solver of knapsack, O(2^n) in the worst case, using Branch&Bound technique
* [solve_recover_on_center_optimization](https://github.com/i1oveMyse1f/combinatorian_optimization/blob/4d0d6707b31ac620060fb4369f968341853bddbd/knapsack/src/knapsack.h#L44) is works well if answer will be like [1, 1,..., 1], [0, 1, 0, 0, ..., 0], [0, 0, ..., 0], where things are sorted by c/w. Middle part is sending to to other solver; O(n log n) + O(middle_solver(middle_part)
* [src/solver_*](src/) is examples of using class methods.
* [src/submit_to_system](src/submit_to_system.cpp) is final version of code
* [answers/results.csv](answers/results.csv) is csv with results of all experements.

You can run and send to system using [runner.py](runner.py):

    python3 runner.py solver experement_name

where `solver` is executable file.
