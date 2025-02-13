
#include "ConstraintPrimitive.hh"


#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;


bool ConstraintPrimitiveEqual::propagate(int level, Variable* cur, vector<Variable*>& touched) {
    int before = x->domainCurSize;
    if (x->keepOnlyValues(y, -k, level, this))
        return true;
    if (before != x->domainCurSize)
        touched.push_back(x);

    assert(x->domainCurSize <= y->domainCurSize);

    if (x->domainCurSize == y->domainCurSize)
        return false;

    before = y->domainCurSize;
    if (y->keepOnlyValues(x, k, level, this))
        return true;
    if (before != y->domainCurSize)
        touched.push_back(y);

    return false;
}


bool ConstraintPrimitiveLessEqual::propagate(int level, Variable* cur, vector<Variable*>& touched) {
    int before = x->domainCurSize;
    if (x->removeValuesAbove(y->getUpperBoundVal() - k, level, this))
        return true;
    if (before != x->domainCurSize)
        touched.push_back(x);
    
    before = y->domainCurSize;
    if (y->removeValuesUnder(x->getLowerBoundVal() + k, level, this))
        return true;
    if (before != y->domainCurSize)
        touched.push_back(y);

    return false;
}


bool ConstraintPrimitiveLessThan::propagate(int level, Variable* cur, vector<Variable*>& touched) {
    int before = x->domainCurSize;
    if (x->removeValuesAbove(y->getUpperBoundVal() - k - 1, level, this))
        return true;
    if (before != x->domainCurSize)
        touched.push_back(x);
    
    before = y->domainCurSize;
    if (y->removeValuesUnder(x->getLowerBoundVal() + k + 1, level, this))
        return true;
    if (before != y->domainCurSize)
        touched.push_back(y);

    return false;
}


bool ConstraintPrimitiveNotEqual::propagate(int level, Variable* cur, vector<Variable*>& touched) {
    int before = y->domainCurSize;
    if (x->isAssigned() && y->removeValue(x->getOnlyValue() + k, level, this))
        return true;
    if (before != y->domainCurSize)
        touched.push_back(y);

    before = x->domainCurSize;
    if (y->isAssigned() && x->removeValue(y->getOnlyValue() - k, level, this))
        return true;
    if (before != x->domainCurSize)
        touched.push_back(x);

    return false;
}


bool ConstraintDjisjuctive::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    int szX = x->domainCurSize;
    int szY = y->domainCurSize;

    for (;;) {
        int minx = x->getLowerBoundVal(), maxx = x->getUpperBoundVal();
        int miny = y->getLowerBoundVal(), maxy = y->getUpperBoundVal();

        if (!filterDomain(x, level, max(minx, min(maxx, maxy - k) + 1), min(maxx, max(minx, miny + ky) - 1)))
            return true;
        if (!filterDomain(y, level, max(miny, min(maxy, maxx - ky) + 1), min(maxy, max(miny, minx + k) - 1)))
            return true;

        if (minx == x->getLowerBoundVal() && maxx == x->getUpperBoundVal() && miny == y->getLowerBoundVal() && maxy == y->getUpperBoundVal())
            break;
    }

    if (szX != x->domainCurSize)
        touched.push_back(x);
    if (szY != y->domainCurSize)
        touched.push_back(y);

    return false;
}


bool ConstraintDisjunctive::filterDomain(Variable* cur, int level, int lbValue, int ubValue)
{
    // remove all values from cur between lbValue and ubValue (assume that domains have the same type)
    if (lbValue > ubValue)
        return true;    // nothing to filter
    
    if (cur->isAssign())
        return !(lbValue <= x->getLowerBoundVal() && x->getLowerBoundVal() <= ubValue);
    
    for (int i = cur->domainCurSize - 1; i >= 0; --i) {
        int val = cur->LocalDomIndToVal(i);
        if (lbValue <= val && val >= ubValue && deletevalue(cur, i, level))
            return false;
    }

    return true;
}