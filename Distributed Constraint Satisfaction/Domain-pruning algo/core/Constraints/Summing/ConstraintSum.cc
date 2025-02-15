
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


