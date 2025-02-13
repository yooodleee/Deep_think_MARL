#include "ConstraintAllEqual.hh"

using namespace std;


bool ConstraintAllEqual::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    if (cur->isAssigned()) {
        // remove the assigned value from the domain of the variables.
        for (size_t i = 0; i < listSize; ++i) {
            if (scope[i]->isAssigned()) {
                if (cur->getOnlyValue() == scope[i]->getOnlyValue())
                    continue;
                else
                    return true;
            }

            if (fixToValue(scope[i], cur->getOnlyValue(), level))
                touched.push_back(scope[i]);
        }
    } else {
        set<indVpLocal> toDel;
        // collect the set of dropped (indexes of) values (since the last call) over all variables.
        for (size_t i = 0; i < listSize; ++i) {
            // cerr << scope[i]->domainCurSize << ":" << scope[i]->domainInitSize << "/" << frontier[i] << endl;
            for (int j = scope[i]->domainCurSize; j < frontier[i]; ++j) {
                assert(scope[i]->inDomLocalToIndVPLocal(j) < scope[0]->domainInitSize);
                toDel.insert(scope[i]->inDomLocalToIndVPLocal(j));
            }
        }

        // remove these dropped (indexes of) values from the domain of all variables.
        for (size_t i = 0; i < listSize; ++i) {
            Variable* var = scope[i];
            int before = var->domainCurSize;
            if (var->removeSetOfIndVpLocal(toDel, level, this))
                return true;
            if (before != var->domainCurSize)
                touched.push_back(var);

            // record the frontier of dropped (indexes of) values for the next call.
            frontier[i] = var->domainCurSize;
        }
    }

    return false;
}