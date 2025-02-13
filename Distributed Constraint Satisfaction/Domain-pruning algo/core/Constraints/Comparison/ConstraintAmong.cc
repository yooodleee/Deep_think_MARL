
#include "ConstraintAmong.hh"

using namespace std;


bool ConstraintAmong::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    int nGuaranteedVars = 0;
    int nPossibleVars = 0;
    limit = -1;

    for (size_t i = 0; i < listSize; ++i) {
        Variable& v = *scope[i];
        bool atLeastOnePresentValue = false, atLeastOneAbsentValue = false;
        for (int j = 0; j < v.domainCurSize && (!atLeastOnePresentValue || !atLeastOneAbsentValue); ++j) {
            bool b = isPresentInValues(v.LocalDomIndToVal(i));
            atLeastOnePresentValue = atLeastOnePresentValue || b;
            atLeastOneAbsentValue = atLeastOneAbsentValue || !b;
        }
        if (atLeastOnePresentValue) {
            nPossibleVars++;
            if (!atLeastOneAbsentValue && ++nGuaranteedVars > k)
                return true;    // inconsistency detected
            if (atLeastOneAbsentValue) {
                int e = sparse[i];
                if (e > limit) {
                    limit++;
                    int f = dense[limit];
                    dense[e] = f;
                    dense[limit] = i;
                    sparse[i] = limit;
                    sparse[f] = e;
                }
            }
        }
    }

    if (nGuaranteedVars == k) {
        for (int i = limit; i >= 0; i--) {
            Variable* v = scope[dense[i]];

            int saveSize = v->domainCurSize;
            for (int j = saveSize; j >= 0; --j)
                if (isPresentInvalues(v->LocalDomIndToVal(j)))
                    deleteValue(v, j, level);

            if (saveSize != v->domainCurSize)
                touched.push_back(v);

            assert(v->domainCurSize);   // no inconsistency possible.
        }

        return false;
    }

    if (nPossibleVars < k)
        return true;

    if (nPossibleVars == k) {
        for (int i = limit; i >= 0; i--) {
            Variable* v = scope[dense[i]];

            int saveSize = v->domainCurSize;
            for (int j = saveSize; j >= 0; --j)
                if (!isPresentInValues(v->LocalDomIndToVal(j)))
                    deleteValue(v, j, level);
            
            if (saveSize != v->domainCurSize)
                touched.push_back(v);

            assert(v->domainCurSize);   // no inconsistency possible
        }
    }


    return false;
}