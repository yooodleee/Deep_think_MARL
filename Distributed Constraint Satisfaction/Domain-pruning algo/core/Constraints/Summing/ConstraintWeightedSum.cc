
#include "ConstraintWeightedSum.hh"

using namespace std;


bool ConstraintWeightedSumEQ::propagate(int level, Variable* cur, vector<Variable*>& touched)
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

            int c = coefficients[i];
            if (c >= 0) {
                min -= c * var->getLowerBoundVal();
                max -= c * var->getUpperBoundVal();
            } else {
                min -= c * var->getUpperBoundVal();
                max -= c * var->getLowerBoundVal();
            }

            for (int j = var->domainCurSize - 1; j >= 0; j--) {
                int v = var->LocalDomIndToVal(j);
                if (max + v * c < limit || min + v * c > limit)
                    if (deleteValue(var, j, level))
                        return true;
            }

            if (sizeBefore != var->domainCurSize) {
                touched.push_back(var);
                positionOfLastChange = i;
            }

            if (c >= 0) {
                min += c * var->getLowerBoundVal();
                max += c * var->getUpperBoundVal();
            } else {
                min += c * var->getUpperBoundVal();
                max += c * var->getLowerBoundVal();
            }
        }
    } while (positionOfLastChange != -1);

    return false;
}


