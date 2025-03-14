from csp import Constraint, Variable, CSP
import random
import util


class UnassignedVars:
    """Class for holding the unassigned variables of a CSP. Extract from,
    re-initialize it, and return variables to it. Object is initialized by 
    passing a select_criteria (to determine the order variables are extracted)
    and the CSP object.
    
    Variables
    --------------
        select_criteria
            random, fixed, mrv with 
            'random' == select a random unassigned variable
            'fixed' == follow the ordering of the CSP variables (i.e., csp.variables()[0]
                before csp.variables()[1]
            'mrv' == select the variable with minimum values in its current domain
                break ties by the ordering in the CSP variables.
    """

    def __init__(self, select_criteria, csp):
        if select_criteria not in ['random', 'fixed', 'mrv']:
            print(
                "Error unassignedVars given an illegal selection criteria {}. "
                "Must be one of 'random', 'stack', 'queue', or 'mrv'".format(select_criteria)
            )
        self.unassigned = list(csp.variables())
        self.csp = csp
        self._select = select_criteria
        if select_criteria == 'fixed':
            # reverse unassigned list so that can add and extract from the back
            self.unassigned.reverse()
    
    
    def extract(self):
        if not self.unassigned:
            print("Warning, extracting from empty unassigned list")
            return None
        
        if self._select == 'random':
            i = random.randint(0, len(self.unassigned) - 1)
            nxtvar = self.unassigned[i]
            self.unassigned[i] = self.unassigned[-1]
            self.unassigned.pop()
            return nxtvar
        
        if self._select == 'fixed':
            return self.unassigned.pop()
        
        if self._select == 'fixed':
            nxtvar = min(self.unassigned, key=lambda v: v.curDomainCurSize())
            self.unassigned.remove(nxtvar)
            return nxtvar
    

    def empty(self):
        return len(self.unassigned) == 0
    
    def insert(self, var):
        if not var in self.csp.variables():
            print(
                "Error, trying to insert variable {} in unassigned that is not in the CSP problem".format(var.name())
            )
        else:
            self.unassigned.append(var)


def bt_search(
        algo, csp, variableHeuristic, allSolutions, trace
):
    """Main interface routine for calling different of backtracking search
    algorithm is one of ['BT', 'FC', 'GAC']
    csp is a CSP obj specifying the csp prob to solve
    variableHeuristic is one ot ['random', 'fixed', 'mrv']
    allSolutions True or False. True means finds all solutions.
    trace True or False. True means turn on tracing of the algorithm
    
    bt_search returns a list of solutions. Each solution is itself a list
    of pairs (var, value). Where var is a Variable obj, and value is a value
    from its domain.
    """

    varHeuristics = ['random', 'fixed', 'mrv']
    algorithms = ['BT', 'FC', 'GAC']

    # statistics
    bt_search.nodesExplored = 0

    if variableHeuristic not in varHeuristics:
        print(
            "Error. Unknown variable heuristics {}. Must be one of {}".format(variableHeuristic, varHeuristics)
        )
    if algo not in algorithms:
        print(
            "Error. Unknown algorithm heuristics {}. Must be one of {}".format(algo, algorithms)
        )
    
    uv = UnassignedVars(variableHeuristic, csp)
    Variable.clearUndoDict()
    for v in csp.variables():
        v.reset()
        
    if algo == 'BT':
        solutions = BT(uv, csp, allSolutions, trace)
    elif algo == 'FC':
        for cnstr in csp.constraints():
            if cnstr.arity() == 1:
                FCCheck(cnstr, None, None)  # FC with unary constraints at the root
        solutions = FC(uv, csp, allSolutions, trace)
    elif algo == 'GAC':
        GacEnforce(csp.constraints(), csp, None, None)  # GAC at the root
        solutions = GAC(uv, csp, allSolutions, trace)
    
    return solutions, bt_search.nodesExplored


def BT(unAssignedVars, csp, allSolutions, trace):
    """Backtracking Search. unAssignedVars is the current set of
    unassigned variables. csp is the csp prob, allSolutions is True
    if you want all solutioness trace if you want some tracing of
    variables assignments tried and constraints failed. Returns the 
    set of solutions found.
    
    To handle finding 'allSolutions', at every stage collect up the 
    solutions returned by the recursive calls, and then return a list
    of all of them.
    
    If you are looking for one solution you stop trying further values
    of the variable currently being tried as soon as one of the recursive 
    calls returns some solutions.
    """
    if unAssignedVars.empty():
        if trace:
            print("{} Solution Found".format(csp.name()))
        
        soln = []
        for v in csp.variables():
            soln.append((v, v.getValue()))
        return [soln]   # each call returns a list of solutions found
    
    bt_search.nodesExplored += 1
    solns = []  # so far you have no solutions recursive calls
    nxtvar = unAssignedVars.extract()
    if trace:
        print("==> Trying {}".format(nxtvar.name()))
    for val in nxtvar.domain():
        if trace:
            print("==> {} = {}".format(nxtvar.name(), val))
        nxtvar.setValue(val)
        constraintsOK = True
        for cnstr in csp.constraintOf(nxtvar):
            if cnstr.numUnassigned() == 0:
                if not cnstr.check():
                    constraintsOK = False
                    if trace:
                        print("<== falsified constraint\n")
                        break
        if constraintsOK:
            new_solns = BT(unAssignedVars, csp, allSolutions, trace)
            if new_solns:
                solns.append(new_solns)
            if len(solns) > 0 and not allSolutions:
                break   # don't bother with other values of nxtvar
                        # as you found a soln.
    
    nxtvar.unAssign()
    unAssignedVars.insert(nxtvar)
    return solns


def FCCheck(cnstr, reasonVar, reasonVal):
    if cnstr.numUnassigned() != 1:
        print(
            "Error FCCheck called on constraint {} with {} neq 1 unassigned vars"
            .format(cnstr.name(), cnstr.numUnassignedVars)
        )
    var = cnstr.unAssignedvars()[0]
    for val in var.curDomain():
        var.setValue(val)
        if not cnstr.check():
            var.pruneValue(val, reasonVar, reasonVal)
        var.unAssign()  # NOTE YOU MUST UNDO TRIAL ASSIGNMENT
    if var.curDomainSize() == 0:
        return "DWO"
    return "OK"


def FC(unAssignedVars, csp, allSolutions, trace):
    """Forward checking search.
    unAssignedvars is the current set of unassigned variables.
    csp is the csp prob, allSolutions is True if you want all 
    solutional trace if you want some tracing of variable assignments
    tried and constraints failed.
    
    RETURNS LIST OF ALL SOLUTIONS FOUND.
    
    Finding allSolutions is handled just as it was in BT. Execpt that 
    when you are not looking for all solutions and you stop early because
    one of the recursive calls found a solution you must make sure that 
    you restore all pruned values before returning.
    """

    # your implementation for Question 2 goes in this function body.
    # you must not change the function parameters.
    # Implementing handling of the trace parameter is optional
    # but it can be useful for debugging

    if unAssignedVars.empty():
        return [[(v, v.getvalue()) for v in csp.variables()]]

    sol_a = []
    v = unAssignedVars.extract()
    bt_search.nodesExplored += 1
    for val in v.curDomain():
        v.setValue(val)
        DWO = False
        for c in csp.constraintsOf(v):
            if c.numUnassigned() == 1:
                if FCCheck(c, v, val) == 'DWO':
                    DWO = True
                    break
        
        if not DWO:
            sol_a += FC(unAssignedVars, csp, allSolutions, trace)
            if not allSolutions and len(sol_a):
                Variable.restoreValues(v, val)
                break
        
        Variable.restorevalues(v, val)
    v.unAssign()
    unAssignedVars.insert(v)
    return sol_a


def GacEnforce(constraints, csp, reasonVar, reasonVal):
    """Establish GAC on constraints by pruning values
    from the current domains of the variables. Return
    "OK" if completed "DWO" if found a domain wipe out.
    """

    # your implementation for Question 3 goes in this function body
    # you must not change the function parameters
    # ensure that you return one of "OK" or "DWO"
    while constraints:
        c = constraints.pop()
        for v in c.scope():
            for val in v.curDomain():
                if not c.hasSupport(v, val):
                    v.pruneValue(val, reasonVar, reasonVal)
                    if v.curDomainSize() == 0:
                        return "DWO"
                    for recheck in csp.constraintsOf(v):
                        if recheck is not c and recheck not in constraints:
                            constraints.append(recheck)
    
    return "NO DWO"


def GAC(unAssignedVars, csp, allSolutions, trace):
    """GAC search.
    unAssignedVars is the current set of unassigned variables.
    csp is the csp prob, allSolutions is True if you want all 
    solutionsl trace if you want some tracing of variable assignments
    tried and constraints failed.
    
    RETURNS LIST OF ALL SOLUTIONS FOUND.
    
    Finding allSolutions is handled just as it was in BT. Except that
    when you are not looking for all solutions and you stop early because
    one of the recursive calls found a solution you must make sure that
    you restore all pruned values before returning.
    """

    # your implementation for Question 3 goes in this function body
    # you must not change the function params.
    # implementing support for "trace" is optional, but it might
    # help you in debugging

    if unAssignedVars.empty():
        if trace:
            print("{} Solution Found".format(csp.name()))
        return [[(v, v.getValue()) for v in csp.variables()]]

    bt_search.nodesExplored += 1
    sol_a = []
    nv = unAssignedVars.extract()
    nxtvar = unAssignedVars.extract()
    if trace:
        print("variable {}".format(nxtvar.name()))
    for val in nv.curDomain():
        if trace:
            print("-> {} = {}".format(nxtvar.name(), val))
        nv.setValue(val)
        DWO = False
        if GacEnforce(csp.constraintsOf(nv), csp, nv, val) == "DWO":
            DWO = True
        if not DWO:
            ns = GAC(unAssignedVars, csp, allSolutions, trace)
            if ns:
                sol_a.extend(ns)
            if len(sol_a) > 0 and not allSolutions:
                nv.restoreValues(nv, val)
                break
        nv.restoreValues(nv, val)
    nv.unAssign()
    unAssignedVars.insert(nv)
    return sol_a