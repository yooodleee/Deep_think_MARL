from constraints import *
from backtracking import bt_search, GacEnforce
from csp import Variable, CSP
from csp_problem import nQueens, sudokuCSP, solve_planes
from sudoku import b1, b5, b6
from plane_scheduling import p1, p2, p3, p4, p5, p6, p7, check_plane_solution

import argparse

legalQs = ["q1", "q2", "q3", "q4", "q5", "q6"]
tested = [False] * 5

gradeMessage = ""
grades = [0, 0, 0, 0, 0, 0]
outof = [4, 5, 7, 2, 4, 10]
tests = ["Q1", "Q2", "Q3", "Q4", "Q5", "Q6"]
titles = [
    "Q1. Table Constraint for nQueens (4 points)",
    "Q2. Forward Checking implementation (5 points)",
    "Q3. GacEnforce and GAC implementation(5 points)",
    "Q4. AllDiff for Sudoku (2 points)",
    "Q5. NValues Constraint implementation (4 points)",
    "Q6. Plane Scheduling (10 points)",
]


def print_title(i):
    l = max([len(t) for t in titles])
    print("-" * l)
    print(titles[i])
    print("-" * l)


def print_sep(c='-'):
    l = max([len(t) for t in titles])
    print(c * l)


def print_soln(s):
    for (var, val) in s:
        print("{} = {}".format(var.name(), val), end='')


def question_1():
    print_title(0)
    tested[0] = True
    ntests = 3
    fails = [False] * ntests

    # test1 constraint.check()
    q2 = Variable("Q2", [1, 2, 3, 4, 5])
    q5 = Variable("Q5", [1, 2, 3, 4, 5])
    c = QueenTableConstraint("Q2/Q5", q2, q5, 2, 5)
    q2.setValue(2)
    for val in q5.domain():
        q5.setValue(val)
        if c.check():
            if val in [2, 5]:
                print("Quuens table constraint check routine failed")
                print(
                    "Q2={}, Q5={} not detected as falsifying constraint".format(q2.getValue(), q5.getValue())
                )
            else:
                if val in [1, 3, 4]:
                    print("Queens table constraint check routine failed")
                    print(
                        "Q2={}, Q5={} not detected as satisfying constraint".format(q2.getValue(), q5.getValue())
                    )
    
    if fails[0]:
        print("Fail Q1 test 1")
    else:
        print("Pass Q1 test 1")
    print_sep()

    # test2 constraint.hasSupport()
    q2.reset()
    q3 = Variable("Q3", [1, 2, 3, 4, 5])
    q2.pruneValue(1, None, None)
    q2.pruneValue(4, None, None)
    q2.pruneValue(5, None, None)
    c = QueensTableConstraint("Q2/Q5", q2, q3, 2, 3)
    for val in q3.domain():
        if c.hasSupport(q3, val):
            if val not in [1, 4, 5]:
                print("Queens table constraint hasSupport routine failed")
                print(
                    "Q2 current domain = {}, Q3 = {} detected to have support (doesn't)".format(q2.curDomain(), val)
                )
        else:
            if val not in [2, 3]:
                print("Queens table constraint hasSupport routine failed")
                print(
                    "Q2 current domain = {}, Q3 = {} detected to not have support (does)".format(q2.curDomain(), val)
                )
    
    if fails[1]:
        print("Fail Q1 test 2")
    else:
        print("Pass Q1 test 2")
    print_sep()

    # test3 within backtracking search
    csp = nQueens(8, True)
    solutions, num_nodes = bt_search('BT', csp, 'fixed', True, False)
    if num_nodes != 1965:
        print("Queens table constraint not working correctly. BT should explore 1965 nodes.")
        print("With your implementation it explores {}".format(num_nodes))
        fails[2] = True
    
    if len(solutions) != 92:
        print("Queens table constraint not working correctly. BT should return 92 solutions")
        print("With your implementation it returns {}".format(solutions))
        fails[2] = True
    
    if fails[2]:
        print("Fail Q1 test 3")
    else:
        print("Pass Q1 test 3")
    
    if any(fails):
        grades[0] = 0
    else:
        grades[0] = outof[0]


def question_2():
    print_title(1)
    tested[1] = True

    fails = [False, False]

    # test1. Find one solution
    csp = nQueens(8, False)
    solutions, num_nodes = bt_search('FC', csp, 'fixed', False, False)
    errors = csp.check(solutions)

    if len(errors) > 0:
        fails[0] = True
        print("Fail Q2 test 1: invalid solution(s) returned by FC")
        for err in errors:
            print_soln(err[0])
            print("\n", err[1])
    
    if len(solutions) != 1:
        fails[0] = True
        print("Fail Q2 test 1: FC failed to return only one solution")
        print(" returned: ")
        for s in solutions:
            print_soln(s)
            print("")
    
    ok = True
    for v in csp.variables():
        if set(v.curDomain()) != set(v.domain()):
            fails[0] = True
            print("Fail Q2 test 1: FC failed to restore domains of variables")
    
    if not fails[0]:
        print("Pass Q2 test 1")
    print_sep()

    csp = nQueens(8, False)
    solutions, num_nodes = bt_search('FC', csp, 'fixed', True, False)
    errors = csp.check(solutions)

    if len(errors) > 0:
        fails[1] = True
        print("Fail Q2 test 2: invalid solution(s) returned by FC")
        for err in errors:
            print_soln(err[0])
            print("\n", err[1])
    
    if len(solutions) != 92:
        fails[1] = True
        print("Fail Q2 test 2: FC failed to return 92 solutions")
        print(" returned {} solutions".format(len(solutions)))
    
    ok = True
    for v in csp.variables():
        if set(v.curDomain()) != set(v.domain()):
            fails[1] = True
            print("Fail Q2 test 2: FC failed to restore domains of variables")
    
    if not fails[1]:
        print("Pass Q2 test 2")
    print_sep()

    if sum(fails) == 2:
        grades[1] = 0
    elif sum(fails) == 1:
        grades[1] = 3
    elif sum(fails) == 0:
        grades[1] = outof[1]


def question_3():
    print_title(2)
    tested[2] = True

    fails = [False, False, False, False, False, False, False, False]
    v1 = Variable('V1', [1, 2])
    v2 = Variable('V2', [1, 2])
    v3 = Variable('V3', [1, 2, 3, 4, 5])
    v4 = Variable('V4', [1, 2, 3, 4, 5])
    v5 = Variable('V5', [1, 2, 3, 4, 5])
    v6 = Variable('V6', [1, 2, 3, 4, 5, 6, 7, 8, 9])
    v7 = Variable('V7', [1, 2, 3, 4, 5, 6, 7, 8, 9])
    v8 = Variable('V8', [1, 2, 3, 4, 5, 6, 7, 8, 9])
    v9 = Variable('V9', [1, 2, 3, 4, 5, 6, 7, 8, 9])
    vars = [v1, v2, v3, v4, v5, v6, v7, v8, v9]
    ac = AllDiffConstraint('test9', vars)
    testcsp = CSP('test', vars, [ac])
    GacEnforce([ac], testcsp, None, None)

    # v1.pruneValue(1, None, None)

    test1 = "       v1 = Variable('V1', [1, 2])\n\
            v2 = Variable('V2', [1, 2])\n\
            v3 = Variable('V3', [1, 2, 3, 4, 5])\n\
            v4 = Variable('V4', [1, 2, 3, 4, 5])\n\
            v5 = Variable('V5', [1, 2, 3, 4, 5])\n\
            v6 = Variable('V6', [1, 2, 3, 4, 5, 6, 7, 8, 9])\n\
            v7 = Variable('V7', [1, 2, 3, 4, 5, 6, 7, 8, 9])\n\
            v8 = Variable('V8', [1, 2, 3, 4, 5, 6, 7, 8, 9])\n\
            v9 = Variable('V9', [1, 2, 3, 4, 5, 6, 7, 8, 9])\n\
            vars = [v1, v2, v3, v4, v5, v6, v7, v8, v9]\n\
            ac = AllDiffConstraint('test9', vars)\n\
            testcsp = CSP('test', vars, [ac])\n\
            GacEnForce([ac], testcsp, None, None)"
    
    soln_doms = [   
        set([1, 2]), set([1, 2]), set([3, 4, 5]), set([3, 4, 5]), set([3, 4, 5]),
        set([6, 7, 8, 9]), set([6, 7, 8, 9]), set([6, 7, 8, 9]), set([6, 7, 8, 9])  
    ]

    for i, v in enumerate(vars):
        if set(v.curDomain()) != soln_doms[i]:
            fails[0] = True
            print("Error: {}.curDomain() == {}".format(v.name(), v.curDomain()))
            print("Correct curDomain should be == {}".format(list(soln_doms[i])))
        
    if fails[0]:
        print("\nFail Q3 test 1\nErrors were generated on the following code: ")
        print(test1)
    else:
        print("Pass Q3 test 1")
    print_sep()

    v1 = Variable('V1', [1, 2])
    v2 = Variable('V2', [1, 2])
    v3 = Variable('V3', [1, 2, 3, 4, 5])
    v4 = Variable('V4', [1, 2, 3, 4, 5])
    v5 = Variable('V5', [1, 2, 3, 4, 5])
    v6 = Variable('V6', [1, 3, 4, 5])
    v7 = Variable('V7', [1, 3, 4, 5])

    ac1 = AllDiffConstraint('1', [v1, v2, v3])
    ac2 = AllDiffConstraint('1', [v1, v2, v4])
    ac3 = AllDiffConstraint('1', [v1, v2, v5])
    ac4 = AllDiffConstraint('1', [v3, v4, v5, v6])
    ac5 = AllDiffConstraint('1', [v3, v4, v5, v7])

    vars = [v1, v2, v3, v4, v5, v6, v7]
    cnstrs = [ac1, ac2, ac3, ac4, ac5]
    testcsp = CSP('test2', vars, cnstrs)
    GacEnforce(cnstrs, testcsp, None, None)

    test2 = "      v1 = Variable('V1', [1, 2])\n\
            v2 = Variable('V2', [1, 2])\n\
            v3 = Variable('V3', [1, 2, 3, 4, 5])\n\
            v4 = Variable('V4', [1, 2, 3, 4, 5])\n\
            v5 = Variable('V5', [1, 2, 3, 4, 5])\n\
            v6 = Variable('V6', [1, 3, 4, 5])\n\
            v7 = Variable('V7', [1, 3, 4, 5])\n\
            ac1 = AllDiffConstraint('1', [v1, v2, v3])\n\
            ac2 = AllDiffConstraint('1', [v1, v2, v4])\n\
            ac3 = AllDiffConstraint('1', [v1, v2, v5])\n\
            ac4 = AllDiffConstraint('1', [v3, v4, v5, v6])\n\
            ac5 = AllDiffConstraint('1', [v3, v4, v5, v7])\n\
            vars = [v1, v2, v3, v4, v5, v6, v7]\n\
            cnstrs = [ac1, ac2, ac3, ac4, ac5]\n\
            testcsp = CSP('test2', vars, cnstrs)\n\
            GacEnForce(cnstrs, testcsp, None, None)"
    
    soln_doms = [
            set([1, 2]), set([1, 2]), 
            set([3, 4, 5]), set([3, 4, 5]), set([3, 4, 5]),
            set([1]), set([1])  
    ]

    # v1.pruneValue(1, None, None)

    for i, v in enumerate(vars):
        if set(v.curDomain()) != soln_doms[i]:
            fails[1] = True
            print("Error: {}.curDomain() == {}".format(v.name(), v.curDomain()))
            print("Correct curDomain should be == {}".format(list(soln_doms[i])))
    
    if fails[1]:
        print("\nFail Q3 test 2\nErrors were generated on the following code: ")
        print(test2)
    else:
        print("Pass Q3 test 2")
    print_sep()

    csp = sudokuCSP(b1, 'neq')
    GacEnforce(csp.constraints(), csp, None, None)
    vars = csp.variables()
    vars.sort(key=lambda var: var.name())
    
    soln_doms = [
            set([3]), set([1]), set([2]), set([5]), set([9]),
            set([8]), set([7]), set([6]), set([4]), set([9]), set([4]),
            set([6]), set([7]), set([3]), set([1]), set([2]), set([5]),
            set([8]), set([8]), set([7]), set([5]), set([4]), set([2]),
            set([6]), set([9]), set([1]), set([3]), set([5]), set([6]),
            set([7]), set([8]), set([4]), set([2]), set([3]), set([9]),
            set([1]), set([4]), set([8]), set([1]), set([3]), set([6]),
            set([9]), set([5]), set([7]), set([2]), set([2]), set([9]),
            set([3]), set([1]), set([7]), set([5]), set([4]), set([8]),
            set([6]), set([1]), set([3]), set([8]), set([2]), set([5]),
            set([7]), set([6]), set([4]), set([9]), set([6]), set([5]),
            set([4]), set([9]), set([1]), set([3]), set([8]), set([2]),
            set([7]), set([7]), set([2]), set([9]), set([6]), set([8]),
            set([4]), set([1]), set([3]), set([5])
    ]