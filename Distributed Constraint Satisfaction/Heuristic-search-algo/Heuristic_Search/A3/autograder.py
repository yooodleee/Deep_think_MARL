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