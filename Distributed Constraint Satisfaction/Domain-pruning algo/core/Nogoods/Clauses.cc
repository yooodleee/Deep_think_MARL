
#include "Clauses.hh"
#include "Constraint.hh"
#include "GeneralizedNoGoodsManager.hh"
#include "Options.hh"
#include "Variable.hh"

#include <numeric>

using namespace std;


int Clauses::checkWatch(unsigned lit, int level, vector<Variable*>& touched)
{
    assert(lit < (Variable::varProps.size() << 1));

    unsigned* ptr_j = &idxClauses.data()[0];
    unsigned* ptr_i = &idxClauses.data()[0];
    unsigned* ptr_e = &idxClauses.data()[idxClauses.size()];

    for (; ptr_i != ptr_e; ++ptr_i) {
        vector<unsigned>& cl = clauses[*ptr_i];

        // Putting lit at pos 1
        if (lit == cl[0])
            swap(cl[0], cl[1]);

        // check the satisfiability of the clause (by looking at cl[0])
        if ((Variable::varProps[cl[0] >> 1].state & sign(cl[0])) == POS) {
            *ptr_j++ = *ptr_i;
            continue;
        }

        for (size_t another = 2, stop = cl.size(); another < stop; ++another) {
            if ((Variable::varProps[cl[another] >> 1].state ^ sign(cl[another])) != NEG) {  // have another watch
                swap(cl[1], cl[another]);
                watched[cl[1]].push_back(*ptr_i);
                goto nextTurn;
            }
        }

        // There is no new watch => its a unit clause
        // Checking also watch at pos 0 to know if the last watch is assigned too
        if (sign(cl[0])) {
            Variable& zV = *Variable::varProps[cl[0] >> 1].toVar;
            assert(zV.isValidVpInd(cl[0] >> 1));    // the val must be alive in the domain otherwise the clause was already satisfied
            if (zV.isAssigned()) {
                Stats::nbConflict++;
                int idx = *ptr_i;
                for (; ptr_i != ptr_e;)
                    *ptr_j++ = *ptr_i++;
                idxClauses.resize(ptr_j - (unsigned*)&idxClauses.data()[0]);
                return idx;
            }

            Stats::nbSupp++;
            zV.removeAt(Variable::varProps[cl[0] >> 1].posInVar, level, refs[*ptr_i]);
            touched.push_back(&zV);
        } else {    // positive sign, must assign the val
            Variable& zV = *Variable::varProps[cl[0] >> 1].toVar;
            if ((Variable::varProps[cl[0] >> 1].state ^ sign(cl[0])) == NEG) {  // if the val was removed => conflict
                Stats::nbConflict++;
                int idx = *ptr_i;
                for (; ptr_i != ptr_e;)
                    *ptr_j++ = *ptr_i++;
                idxClauses.resize(ptr_j - (unsigned*)&idxClauses.data()[0]);
                return idx;
            }

            assert(!zV.isAssigned());   // the val is not alone in the domain otherwise the clause was already satisfied.
            Stats::nbSupp += zV.domainCurSize - 1;

            zV.assignAt(Variable::varProps[cl[0] >> 1].posInVar, level, refs[*ptr_i]);
            touched.push_back(&zV);
        }

        *ptr_j++ = *ptr_i;  // lit continue to be a watched val for backtracking.
    nextTurn:;
    }

    idxClauses.resize(ptr_j - (unsigned*)&idxClauses.data()[0]);
    return -1;
}

