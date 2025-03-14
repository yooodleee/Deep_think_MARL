from csp import Constraint, Variable
import util 


class TableConstraint(Constraint):
    """General type of constraint that can be use to implement any type of
    constraint. But might require a lot of space to do so.
    
    A table constraint explicitly stores the set of satisfying tuples of 
    assignments.
    """

    def __init__(self, name, scope, satisfyingAssignments):
        """Init by specifying a name and a set variables the constraint is over.
        Along with a list of satisfying assignments. Each satisfying assignment 
        is itself a list, of length equal to the number of variables in the
        constraints scope. If sa is a single satisfying assignment, e.g., 
        sa=satisfyingAssignments[0] then sa[i] is the value that will be assigned
        to the variable scope[i].
        
        Example, say you want to specify a constraint alldiff(A, B, C, D) for three
        variables A, B, C each with domain [1, 2, 3, 4]. Then you would create this
        constraint using the call 
        
        c = TableConstraint(
                'example', [A, B, C, D],
                [
                    [1, 2, 3, 4], [1, 2, 4, 3], [1, 3, 2, 4],
                    [1, 3, 4, 2], [1, 4, 2, 3], [1, 4, 3, 2],
                    [2, 1, 3, 4], [2, 1, 4, 3], [2, 3, 1, 4],
                    [2, 3, 4, 1], [2, 4, 1, 3], [2, 4, 3, 1],
                    [3, 1, 2, 4], [3, 1, 4, 2], [3, 2, 1, 4],
                    [3, 2, 4, 1], [3, 4, 1, 2], [3, 4, 2, 1],
                    [4, 1, 2, 3], [4, 1, 3, 2], [4, 2, 1, 3],
                    [4, 2, 3, 1], [4, 3, 1, 2], [4, 3, 2, 1],
                ]
        )
        
        as these are the only assignments to A, B, C respectively that satisfy 
        alldiff(A, B, C, D).
        """

        Constraint.__init__(self, name, scope)
        self._name = "TableCnstr_" + name
        self.satAssignments = satisfyingAssignments
    

    def check(self):
        """Check if current variable assignments are in the satisfying set."""
        assignments = []
        for v in self.scope():
            if v.isAssigned():
                assignments.append(v.getvalue())
            else:
                return True
        return assignments in self.satAssignments
    
    
    def hasSupport(self, var, val):
        """Check if var = val has an extension to an assignment of all variables in
        constraint's scope that satisfies the constraint. Important only to examine
        values in the variable's current domain as possible extensions.
        """

        if var not in self.scope():
            return True     # var = val has support on any constraint it does not participate in

        vindex = self.scope().index(var)
        found = False
        for assignment in self.satAssignments:
            if assignment[vindex] != val:
                continue    # this assignment can't work it doesn't make var = val
            found = True    # Otherwise it has potential. Assume found until shown otherwise
            for i, v in enumerate(self.scope()):
                if i != vindex and not v.inCurDomain(assignment[i]):
                    found = False   # Bummer...this assignment didn't work it assigns
                    break   # a value to v that is not in v's curDomain
                            # note you skip checking if val in var's curDomain
            
            if found:       # if found still true the assginment worked. you can stop
                break
        
        return found    # either way found has the right truth value


class QueensConstraint(Constraint):
    """Queens constraint between queen in row i and row j"""

    def __init__(self, name, qi, qj, i, j):
        scope = [qi, qj]
        Constraint.__init__(self, name, scope)
        self._name = "QueenCnstr_" + name
        self.i = i
        self.j = j
    

    def check(self):
        qi = self.scope()[0]
        qj = self.scope()[1]
        if not qi.isAssigned() or not qj.isAssigned():
            return True
        return self.queensCheck(qi.getValue(), qj.getValue())
    

    def queensCheck(self, vali, valj):
        diag = abs(vali - valj) == abs(self.i - self.j)
        return not diag and vali != valj
    

    def hasSupport(self, var, val):
        """Check if var = val has an extension to an assignment of the 
        other variable in the constraint that satisfies the constraint"""
        
        # hasSupport for this constraint is easier as you only have one
        # other variable in the constraint.
        if var not in self.scope():
            return True     # var = val has support on any constraint it does not participate in
        
        otherVar = self.scope()[0]
        if otherVar == var:
            otherVar = self.scope()[1]
        for otherVal in otherVar.curDomain():
            if self.queensCheck(val, otherVal):
                return True
        
        return False
    

class QueensTableConstraint(TableConstraint):
    """Queens constraint between queen in row i and row j, but
    using a table constraint instead. That is, you have to create
    and add the satisfying tuples.
    
    Since you inherit from TableConstraint, you can call 
    TableConstraint.__init__(self, ...) to set up the constraint.
    
    Then you get hasSupport and check automatically from Tableconstraint.
    """

    # your implementation for Question 1 goes
    # inside of this class body. You must not change
    # the existing function signatures.
    def __init__(self, name, qi, qj, i, j):
        self._name = "Queen_" + name
        self.assigns = [
            [ci, cj] for ci in qi.domain()
            for cj in qj.domain()
            if ci != cj and abs(i - j) != abs(ci - cj)
        ]
        TableConstraint.__init__(self, name, [qi, qj], self.assigns)


class NeqConstraint(Constraint):
    """Neq constraint between two variables"""

    def __init__(self, name, scope):
        if len(scope) != 2:
            print("Error Neq Constraints are only between two variables")
        Constraint.__init__(self, name, scope)
        self._name = "Neqcnstr_" + name
    

    def check(self):
        v0 = self.scope()[0]
        v1 = self.scope()[1]
        if not v0.isAssigned() or not v1.isAssigned():
            return True
        
        return v0.getValue() != v1.getValue()
    

    def hasSupport(self, var, val):
        """Check if var = val has an extension to an assignment of the
        other variable in the constraint that satisfies the constraint"""

        # hasSupport for this constraint is easier as you only have one
        # other variable in the constraint.
        if var not in self.scope():
            return True     # var = val has support on any constraint it does not participate in

        otherVar = self.scope()[0]
        if otherVar == var:
            otherVar = self.scope()[1]
        for otherVal in otherVar.curDomain():
            if val != otherVal:
                return True
        
        return False


class AllDiffConstraint(Constraint):
    """All diff constraint between a set of variables.
    If you are curious as to how to more efficiently perform GAC on
    an AllDiff see
    http://www.constraint-programming.com/people/regin/papers/alldiff.pdf
    """

    def __init__(self, name, scope):
        Constraint.__init__(self, name, scope)
        self._name = "AllDiff_" + name
    

    def check(self):
        assignments = []
        for v in self.scope():
            if v.isAssigned():
                assignments.append(v.getValue())
            else:
                return True
        
        return len(set(assignments)) == len(assignments)
    

    def hasSupport(self, var, val):
        """Check if var = val has an extension to an assignment of the
        other variable in the constraint that satisfies the constraint."""

        if var not in self.scope():
            return True     # var = val has support on any constraint it does not participate in
        
        """since the constraint has many variables use the helper function
        'findvals' for that you need two test functions
        
        1. for testin complete assignments to the constraint's scope
            return True if and only if the complete assignment satisfies the constraint
        
        2. for testing partial assignments to see if they could possibly work.
            return False if the partial assignment cannot be extended to a satisfying complete assignment
            
        Function 2. is only needed for efficiency (sometimes don't have one)
        if it isn't supplied findvals will use a function that never returns False
        
        For alldiff, you do have both funtions! And they are the same!
        You just check if the assignments are all to different values. If not return False.
        """

        def valsNotEqual(l):
            """tests a list of assignments which are pairs (var, val)
            to see if they can satisfy the call diff"""

            vals = [val for (var, val) in l]
            return len(set(vals)) == len(vals)
        
        varsToAssign = self.scope()
        varsToAssign.remove(var)
        x = findvals(varsToAssign, [(var, val)], valsNotEqual, valsNotEqual)

        return x
    

def findvals(
        remainingVars, 
        assignment,
        finalTestfn,
        partialTestfn=lambda x: True,
):
    """Helper function for finding an assignment to the variables of a constraint
    that together with var = val satisfy the constraint. That is, this function
    looks for a supporting tuple.
    
    findvals uses recursion to build up a complete assignment, oen value
    from every variable's current domain, along with var = val.
    
    It tries all ways of constructing wuch an assignment (using a recursive depth-
    first search).
    
    If partialTestfn is supplied, it will use this function to test all partial 
    assignments--if the function returns False it will terminate trying to grow that
    assignment.
    
    It will test all full assignments to "allVars" using finalTestfn returning once
    it finds assignment that passes this test.
    
    returns True if it finds a suitable full assignment, False if none exist.
    (yes you are using an algorithm that is exactly like backtracking!)
    """

    # print("==>findvars([",)
    # for v in remainingVars: print(v.name(), " ", end='')
    # print("], [")
    # for x, y in assignment: print("({}={}) ".format(x.name(), y), end='')
    # print("")

    # sort the variables call the internal version with the variables sorted
    remainingVars.sort(reverse=True, key=lambda v: v.curDomainSize())
    return findvals_(
        remainingVars, assignment, finalTestfn, partialTestfn
    )


def findvals_(
        remianingVars, assignment, finalTestfn, partialTestfn
):
    """findvals_ internal function with remainingVars sorted by the size
    of their current domain"""

    if len(remianingVars) == 0:
        return finalTestfn(assignment)
    
    var = remianingVars.pop()
    for val in var.curDomain():
        assignment.append((var, val))
        if partialTestfn(assignment):
            if findvals_(
                remianingVars, assignment, finalTestfn, partialTestfn
            ):
                return True
        assignment.pop()    # (var, val) didn't work since you didn't do the return
    remianingVars.append(var)
    return False


class NValuesConstraint(Constraint):
    """NValues constraint over a set of variables. Among the variables in
    the constraint's scope the num that have been assigned values in the 
    set 'required_values' is in the range [lower_bound, upper_bound]
    (lower_bound <= # of variables assigned 'required_value' <= upper_bound)
    
    For example, if you have 4 variables V1, V2, V3, V4, each with domain
    [1, 2, 3, 4], then the call
    NValuesConstraint('test_nvalues', [V1, V2, V3, V4], [3, 2], 2, 3) will
    only be satisfied by assignments such that at least 2 the V1, V2, V3, V4
    are assigned the value 3 or 2, and at most 3 of them have been assigned 
    the value 3 or 2.
    """

    def __init__(
            self,
            name,
            scope,
            required_values,
            lower_bound,
            upper_bound,
    ):
        Constraint.__init__(self, name, scope)
        self._name = "NValues_" + name
        self._required = required_values
        self._lb = lower_bound
        self._ub = upper_bound
    

    def check(self):
        sum = 0
        for v in self.scope():
            if v.isAssigned():
                if v.getValue() in self._required:
                    sum += 1
            else:
                return True
        
        return self._lb <= sum <= self._ub
    

    def hasSupport(self, var, val):
        """Check if var = val has an extension to an assignment of the
        other variable in the constraint that satisfies the constraint
        HINT: check the implementation of AllDiffConstraint.hasSupport
            a similar approach is applicable here (but of course there
            are other ways as well)
        """

        if var not in self.scope():
            return True
        
        def double_bd(l):
            return self._lb <= sum(
                [
                    1 for (variable, value) in l
                    if value in self._required
                ]
            ) <= self._ub
        
        def up_bd(l):
            return sum(
                [
                    1 for (variable, value) in l
                    if value in self._required
                ]
            ) <= self._ub
        
        assign = self.scope()
        assign.remove(var)
        return findvals(assign, [(var, val)], double_bd, up_bd)


class allOnce(Constraint):

    def __init__(self, name, scope, values):
        Constraint.__init__(self, name, scope)
        self._name = "all_Once " + name
        self._scope = scope
        self._values = values
    
    
    def check(self):
        flights = []
        for var in self.scope():
            if not var.isAssigned():
                return True
            elif var.getValue() != 0:
                flights.append(var.getValue())
        
        if len(flights) == len(set(flights)) == len(set(self._values)):
            return True
        
        return False
    

    def hasSupport(self, var, val):
        """Check if var = val has an extension to an assignment of the
        other variable in the constraint that satisfies the constraint"""

        if var not in self.scope():
            return True     # var = val has support on any constraint it does not participate in
        
        def all_flights(l):
            diff_flight = [val for (var, val) in l if val != 0]
            set_fl = len(set(diff_flight))

            if not set_fl == len(set(self._values)) == len(diff_flight):
                return False
            return True
        
        def all_possible(l):
            return len(set(self._scope)) >= len(set(self._values))
        
        vars = self.scope()
        vars.remove(var)

        return findvals(vars, [(var, val)], all_flights, all_possible)