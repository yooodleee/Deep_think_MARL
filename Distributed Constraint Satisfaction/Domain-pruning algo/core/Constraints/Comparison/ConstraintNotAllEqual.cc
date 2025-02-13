
#include "ConstraintNotAllEqual.hh"

#include <limits>

using namespace std;


bool ConstraintAllEqual::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    int uniqueUnfixedVar = 1;
    int uniqueFixedVal = numeric_limits<int>::max();

    for (size_t i = 0; i < listSize; ++i) {
        if (!scope[i]->isAssigned()) {
            if (uniqueUnfixedVar != -1)
                return false;   // AC because at least two unfixed variables.
            uniqueUnfixedVar = i;
        } else {
            if (uniqueFixedVal == numeric_limits<int>::max())
                uniqueFixedVal = scope[i]->getLowerBoundVal();
            else if (uniqueFixedVal != scope[i]->getLowerBoundVal())
                return false;   // AC because two fixed variables with different values.
        }
    }

    if (uniqueUnfixedVar == -1)
        return true;    // because all variables are assigned to the same value
    
    assert(uniqueFixedVal != numeric_limits<int>::max());

    int before = scope[uniqueUnfixedVar]->domainCurSize;
    if (scope[uniqueUnfixedVar]->removeValue(uniqueFixedVal, level, this))
        return true;

    if (before != scope[uniqueUnfixedVar]->domainCurSize)
        touched.push_back(scope[uniqueUnfixedVar]);
    return false;
}