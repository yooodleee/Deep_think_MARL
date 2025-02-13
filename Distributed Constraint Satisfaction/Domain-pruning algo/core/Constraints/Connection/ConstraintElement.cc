
#include "ConstraintElement.hh"

using namespace std;


bool ConstraintElementConstant::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    if (!index->isAssigned()) {
        int saveSize = index->domainCurSize;
        for (int i = saveSize - 1; i >= 0; --i)
        if (!getVariableFor(index->LocalDomIndToVal(i))->isValidValue(result))
            if (deleteValue(index, i, level))
                return true;

        if (saveSize != index->domainCurSize)
            touched.push_back(index);
    }

    // domain can have changed
    if (index->isAssigned()) {
        Variable* var = getVariableFor(index->getOnlyValue());  // This variable must be equal to result
        if (!var->isAssignedTo(result)) {
            if (fixToValue(var, result, level)) {
                touched.push_back(var);
                return false;
            }
            return true;
        }
    }

    return false;
}


bool ConstraintElementVariable::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    // If index is not singleton, try to prune values :
    // - in result's domain, prune the values which aren't in any of list variables'domains
    // - in index's domain, prune the values v for which there is no value j such that list[v] and value both have j in their
    // domains.
    if (!index->isAssigned()) {
        // Update resultSentinels and dom(result)
        if (!reduceResultDomain(touched, level))
            return true;

        for (;;) {
            // Update listSentinels and dom(index)
            int sizeBefore = index->domainCurSize;
            if (!reduceIndexDomain(touched, level))
                return true;
            if (sizeBefore == index->domainCurSize)
                break;
            
            // Update resultSentinels and dom(result)
            sizeBefore = result->domainCurSize;
            if (!reduceResultDomain(touched, level))
                return true;
            if (sizeBefore == result->domainCurSize)
                break;
        }
    }

    // If index is singleton, update dom(list[index]) and dom(result)
    // so that they are both equal to the intersection of the two domains
    if (index->isAssigned()) {
        Variable* var = getVariableFor(index->getOnlyValue());
        int before1 = var->domainCurSize;
        int before2 = result->domainCurSize;
        if (var->keepOnlyValues(result, 0, level, this) || result->keepOnlyValues(var, 0, level, this))
            return true;

        if (before1 != var->domainCurSize)
            touched.push_back(var);
        if (before2 != result->domainCurSize)
            touched.push_back(result);
    }

    return false;
}