
#include "ConstraintInt.hh"

using namespace std;


bool ConstraintInt::computeNewTuple(int var, int value, int nValidTuplesMax)
{
    for (size_t k = 0; k < listSize; ++k)
        cptRet[k] = 0;
    cptRet[var] = value;

    // Compute new tuple
    for (int i = 0; i < nValidTuplesMax; ++i) {
        tuple.clear();
        for (size_t k = 0; k < listSize; ++k)
            tuple[scope[k]->getName()] = scope[k]->getVarPropFromLocalDomInd(cptRet[k]).val;

        if (t->evaluate(tuple)) {
            for (size_t k = var; k < listSize; ++k) {
                indVp cur = scope[k]->getIndFromValue(tuple[scope[k]->getName()]);
                assert(cur != -1);
                vpRes[k][cur - scope[k]->domainStart] = tuple;
            }

            return true;
        }

        // update cpt
        for (size_t k = 0; k < listSize; ++k) {
            if ((int)k == var)
                continue;
            cptRet[k]++;
            if (cptRet[k] < scope[k]->domainCurSize)
                break;
            cptRet[k] = 0;
        }
    }

    return false;
}


