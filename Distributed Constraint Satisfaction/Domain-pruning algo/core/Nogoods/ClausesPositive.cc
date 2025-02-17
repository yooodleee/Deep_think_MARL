
#include "ClausesPositive.cc"
#include "LecoutreNoGoodsManager.hh"
#include "Variable.hh"

using namespace std;


size_t ClausePositive::searchAnotherWatch(vector<ind>& cl)
{
    for (size_t another = 2; another < cl.size(); ++another)
        if (!Variable::varProps[cl[another]].toVar->isAssignedToIndVp(cl[another]))
            return another;
    return cl.size();
}


int ClausePositive::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    assert(cur->isAssigned());

    indVp lit = cur->getLowerBoundInd();
    vector<unsigned>& idxClauses = watched[lit];
    vector<unsigned> newWatch;

    // check all clauses where lit is a watch
    for (size_t i = 0, stop = idxClauses.size(); i < stop; ++i) {
        vector<ind>& cl = clauses[idxClauses[i]];

        // Putting lit at pos 1
        if (lit == cl[0])
            swap(cl[0], cl[1]);

        // Searching for another watch
        size_t another = searchAnotherWatch(cl);

        // If a new watch is found
        if (another < cl.size()) {
            swap(cl[1], cl[another]);
            watched[cl[1]].push_back(idxClauses[i]);
            continue;
        }

        // There is no new watch => its a unit clause
        // Checking also watch at pos 0 to know if the last watch is assigned too
        Variable* zV = Variable::varProps[cl[0]].toVar;

        if (zV->isAssignedToIndVp(cl[0])) { // UNSAT because assigned to the watched value
            assert(zV->getLowerBoundVal() == Variable::varProps[cl[0]].val);
            Stats::nbConflict++;
            newWatch.insert(newWatch.end(), watched[lit].begin() + i + 1, watched[lit].end());
            watched[lit] = newWatch;
            return idxClauses[i];
        }

        // if the value is still alive and not alone in the domain => remove it
        int before = zV->domainCurSize;
        zV->removeAt(Variable::varProps[cl[0]].posInVar, level);
        Stats::nbSupp++;
        if (before != zV->domainCurSize)
            touched.push_back(zV);

        newWatch.push_back(idxClauses[i]);  // lit continue to be a watched value for backtracking
    }

    watched[lit] = newWatch;
    return -1;
}   // propagate


