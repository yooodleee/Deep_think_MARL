
#include "ConstraintSum.hh"

using namespace std;


bool ConstraintSumEQ::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();

    if (limit < min || max < limit)
        return true;

    int positionOfLastChange = -1;
    do {
        for (int i = 0; (size_t)i < listSize; i++) {
            if (i == positionOfLastChange) {
                positionOfLastChange = -1;
                break;
            }

            Variable* var = scope[i];

            int sizeBefore = var->domainCurSize;
            if (sizeBefore == 1)
                continue;

            min -= var->getLowerBoundVal();
            max -= var->getUpperBoundVal();

            for (int j = var->domainCurSize - 1; j >= 0; j--) {
                int v = var->LocalDomIndToVal(j);
                if (v < limit - max || v > limit - min)
                    if (deleteValue(var, j, level))
                        return true;
            }

            if (sizeBefore != var->domainCurSize) {
                touched.push_back(var);
                positionOfLastChange = i;
            }

            min += var->getLowerBoundVal();
            max += var->getUpperBoundVal();
        }
    } while (positionOfLastChange != -1);

    return false;
}


bool ConstraintSumGE::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();

    if (min >= limit)
        return false;
    if (max < limit)
        return true;

    for (size_t i = 0; i < listSize; i++) {
        Variable* var = scope[i];

        int sizeBefore = var->domainCurSize;
        if (sizeBefore == 1)
            continue;
        
        min -= var->getLowerBoundVal();

        int curLim = limit - (max - var->getUpperBoundVal());
        for (int j = var->domainCurSize - 1; j >= 0; j--)
            if (var->LocalDomIndToVal(j) < curLim)
                deleteValue(var, j, level);

        if (sizeBefore != var->domainCurSize)
            touched.push_back(var);

        assert(var->domainCurSize);
        
        min += var->getLowerBoundVal();
        if (min >= limit)
            return false;
    }

    return false;
}


bool ConstraintSumLE::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();

    if (max <= limit)
        return false;
    if (min > limit)
        return true;

    for (size_t i = 0; i < listSize; i++) {
        Variable* var = scope[i];

        int sizeBefore = var->domainCurSize;
        if (sizeBefore == 1)
            continue;
        
        max -= var->getUpperBoundVal();

        int curLim = limit - (min - var->getLowerBoundVal());
        for (int j = var->domainCurSize - 1; j >= 0; j--)
            if (var->LocalDomIndToVal(j) > curLim)
                deleteValue(var, j, level);

        if (sizeBefore != var->domainCurSize)
            touched.push_back(var);

        assert(var->domainCurSize);

        max += var->getUpperBoundVal();
        if (max <= limit)
            return false;
    }

    return false;
}


