import util 
import random
import sys


class Variable:
    """Class for defining CSP variables.
    
    On initialization the variable obj can be given a name and a list
    containing variable's domain of values. You can reset the variable's 
    if you want to solve a similar prob where the domains have changed.
    
    To support CSP propagation, the class also maintains a current domain
    for the variable. Values pruned from the variable domain are removed
    from the current but not from the original domain. values can be also
    restored.
    """

    undoDict = dict()   # stores pruned values indexed by a (variable, value) reason pair

    def __init__(self, name, domain):
        """Create a variable obj, specifying its name (a 
        string) and domain of values."""

        self._name = name               # test name for variable
        self._dom = list(domain)        # Make a copy of passed domain
        self._curdom = list(domain)     # using list
        self._value = None
    
    def __str__(self):
        return "Variable {}".format(self._name)
    
    def domain(self):
        """return copy of variable domain"""
        return (list(self._dom))
    
    def domainSize(self):
        """Return the size of the domain"""
        return (len(self.domain()))
    
    def resetDomain(self, newdomain):
        """reset the domain of this variable"""
        self._dom = newdomain

    def getValue(self):
        return self._value
    
    def setValue(self, value):
        if value != None and not value in self._dom:
            print(
                "Error tried to assign value {} to variable {} that is not in {}'s domain"
                .format(value, self._name, self._name)
            )
        else:
            self._value = value
    
    def unAssign(self):
        self.setValue(None)
    
    def isAssigned(self):
        return self.getValue() != None
    
    def name(self):
        return self._name
    
    def curDomain(self):
        """return copy of variable current domain. But if variable is assigned
        return just its assigned value (this makes implementing hasSupport easier"""

        if self.isAssigned():
            return ([self.getValue()])
        return (list(self._curdom))
    
    def curDomainSize(self):
        """Return the size of the current domain"""
        if self.isAssigned():
            return (1)
        return (len(self._curdom))
    
    def inCurDomain(self, value):
        """Check if value is in current domain"""
        if self.isAssigned():
            return (value == self.getValue())
        return (value in self._curdom)
    
    def pruneValue(self, value, reasonVar, reasonVal):
        """Remove value from current domain"""
        try:
            self._curdom.remove(value)
        except:
            print(
                "Error: tried to prune value {} from variable {}'s domain, but value not present!"
                .format(value, self._name)
            )
        
        dkey = (reasonVar, reasonVal)
        if not dkey in Variable.undoDict:
            Variable.undoDict[dkey] = []
        Variable.undoDict[dkey].append((self, value))
    
    def restoreVal(self, value):
        self._curdom.append(value)
    
    def restoreCurDomain(self):
        self._curdom = self.domain()
    
    def reset(self):
        self.restoreCurDomain()
        self.unAssign()
    
    def dumpVar(self):
        print(
            "Variable\"{}={}\": Dom = {}, CurDom = {}"
            .format(self._name, self._value, self._dom, self._curdom)
        )
    
    @staticmethod
    def clearUndoDict():
        undoDict = dict()
    
    @staticmethod
    def restoreValues(reasonVar, reasonVal):
        dkey = (reasonVar, reasonVal)
        if dkey in Variable.undoDict:
            for (var, val) in Variable.undoDict[dkey]:
                var.restoreVal(val)
            del Variable.undoDict[dkey]


# implement various types of constraints
class Constraint:
    