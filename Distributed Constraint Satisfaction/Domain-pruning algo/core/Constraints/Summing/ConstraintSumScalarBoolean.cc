
#include "ConstraintSumScalarBoolean.hh"

using namespace std;


bool SumScalarBooleanLE::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();

    if (max <= limit)
        return false;
    if (min > limit)
        return true;

    // this is the only case where can filter
    if (min == limit)
        filter(level, touched, 1);

    return false;
}


bool SumScalarBooleanEQ::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();

    if (min > limit || max < limit)
        return true;
    if (min == max || (min < limit && limit < max))
        return false;   // because entailed in that case

    if (min == limit)
        filter(level, touched, 1);
    else if (max == limit) {
        tilter(level, touched, 0);
        for (int i = limitSetEq; i >= 0; i--) {
            int j = set01vs01[i];
            assert(scope[j]->domainCurSize == 2 && scope[half + j]->domainCurSize == 2);

            scope[j]->removeValue(0, level, this);
            touched.push_back(scope[j]);

            scope[half + j]->removeValue(0, level, this);
            touched.push_back(scope[half + j]);
        }
    }

    return false;
}


bool SumScalarBooleanLEVar::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();

    int before = limit->domainCurSize;
    if (limit->removeValueUnder(min, level, this))
        return true;
    if (before != limit->domainCurSize)
        touched.push_back(limit);

    if (max <= limit->getUpperBoundVal())
        return false;

    // this is the only case where can filter
    if (min == limit->getUpperBoundVal())
        filter(level, touched, 1);

    return false;
}