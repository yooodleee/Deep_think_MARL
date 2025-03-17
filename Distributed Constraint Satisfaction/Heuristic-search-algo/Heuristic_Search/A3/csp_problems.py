from csp import Constraint, Variable, CSP
from constraints import *
from backtracking import bt_search


#####################################################################
### NQUEENS
#####################################################################


def nQueens(n, tableCnstr):
    """Return an n-queens CSP, optionally use tableConstraints."""
    
    i = 0
    dom = []
    for i in range(n):
        dom.append(i + 1)
    
    vars = []
    for i in dom:
        vars.append(Variable('Q{}'.format(i), dom))
    
    cons = []
    for qi in range(len(dom)):
        for qj in range(qi + 1, len(dom)):
            if tableCnstr:
                con = QueensTableConstraint(
                    "C(Q{}, Q{})".format(qi + 1, qj + 1),
                    vars[qi], vars[qj], qi + 1, qj + 1
                )
            else:
                con = QueensConstraint(
                    "C(Q{}, Q{})".format(qi + 1, qj + 1),
                    vars[qi], vars[qj], qi + 1, qj + 1
                )
            cons.append(con)
    
    csp = CSP("{}-Queens".format(n), vars, cons)
    return csp


def solve_nQueens(
        n, algo, allsolns, tableCnstr=False, variableHeuristic='fixed', trace=False
):
    """Create and solve an nQueens CSP problem. 
    The first parameter is 'n' the number of queens in the problem, THe second specifies the 
    search algorithm to use (one of 'BT', 'FC', or 'GAC'), the third specifies if all solutions 
    are to be found or just one, variableHeuristic specifies how the next variable is to be 
    selected 'random' at random, 'fixed' in a fixed order, 'mrv' minimum remaining values. 
    Finally 'trace' if specified to be 'True' will generate some output as the search progress.
    """

    csp = nQueens(n, tableCnstr)
    solutions, num_nodes = bt_search(algo, csp, variableHeuristic, allsolns, trace)
    print("Explored {} nodes".format(num_nodes))

    if len(solutions) == 0:
        print("No solutions to {} found".format(csp.name()))
    else:
        print("Solutions to {}: ".format(csp.name()))
        i = 0
        for s in solutions:
            i += 1
            print("Solution #{}: ".format(i), )
            for (var, val) in s:
                print("{} = {}, ".format(var.name(), val), )
            print("")


#####################################################################
### SUDOKU
#####################################################################


def sudokuCSP(initial_sodoku_board, model='neq'):
    """The input board is specified as a list of 9 lists.
    Each of the 9 lists represents a row of the board. If 
    a 0 is in the list it represents an empty cell. Otherwise
    if a number between 1 ~ 9 is in the list then this represents
    a pre-set board position. E.g., the board:
    
    -------------------
    | | |2| |9| | |6| |
    | |4| | | |1| | |8|
    | |7| |4|2| | | |3|
    |5| | | | | |3| | |
    | | |1| |6| |5| | |
    | | |3| | | | | |6|
    |1| | | |5|7| |4| |
    |6| | |9| | | |2| |
    | |2| | |8| |1| | |
    -------------------
    would be represented by the list of lists
    
    [
        [0, 0, 2, 0, 9, 0, 0, 6, 0],
        [0, 4, 0, 0, 0, 1, 0, 0, 8],
        [0, 7, 0, 4, 2, 0, 0, 0, 3],
        [5, 0, 0, 0, 0, 0, 3, 0, 0],
        [0, 0, 1, 0, 6, 0, 5, 0, 0],
        [0, 0, 3, 0, 0, 0, 0, 0, 6],
        [1, 0, 0, 0, 5, 7, 0, 4, 0],
        [6, 0, 0, 9, 0, 0, 0, 2, 0],
        [0, 2, 0, 0, 8, 0, 1, 0, 0],
    ]
    
    Construct and return CSP for solving this sudoku board using
    binary not equals if model='neq' or using allDiff constraints
    if model='alldiff'.
    
    The CSP contains a variable for each cell of the board with 
    domain equal to {1-9} if the board has a 0 at that position,
    and domain equal {i} if the board has a fixed number i at that
    cell.
    
    The CSP has a neq constraint between every relevant pair of variables,
    or an alldiff constraint between every set of variables in a row, column,
    or sub-square.
    """

    # your implementation for Question 4 changes this function
    # implement handling of model == 'alldiff'

    if not model in ['neq', 'alldiff']:
        print(
            "Error wrong sudoku model specified {}. Must be one of {}"
            .format(model, ['neq', 'alldiff'])
        )
    
    # first define the variables
    i = 0
    var_array = []
    for row_list in initial_sodoku_board:
        var_array.append([])
        j = 0
        for col in row_list:
            cell = initial_sodoku_board[i][j]
            if cell == 0:
                dom = [1, 2, 3, 4, 5, 6, 7, 8, 9]
            else:
                dom = [cell]
            
            var = Variable("V{}, {}".format(i + 1, j + 1), dom)
            var_array[i].append(var)
            j += 1
        i += 1
    
    # Set up the constraints
    # row constraints
    constraint_list = []

    for row in var_array:
        if model == "neq":
            