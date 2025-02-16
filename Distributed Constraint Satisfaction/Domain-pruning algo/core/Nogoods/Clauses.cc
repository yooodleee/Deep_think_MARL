
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

/*
    Progation process.

    @param[in] level,
    @param[in] cur,
    @param[in] touched,
*/
int Clauses::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    int ret;

    if (cur->isAssigned()) {
        // If the variable is assigned need to find new watch for the clauses where not(Lit) appears
        ret = checkWatch(cur->indDomLocalToIndVP(0) << 1 | 1, level, touched);
        if (ret != -1)
            return ret;
    }

    int saveDomainCurSize = cur->domainCurSize;
    for (int vi = cur->domainCurSize, stop = cur->getPuSizeLast(); vi < stop; vi++) {
        // For all of the vals removed since the last call need to find new watch for the clauses where Lit appears
        ret = checkWatch(cur->indDomLocalToIndVP(vi) << 1, level, touched);
        if (ret != -1)
            return ret;
    }

    cur->setPuSizeLast(saveDomainCurSize, level);
    return -1;
}   // propagate


/*
    Add the clause in the constraint and watch the two first litterals.

    @param[in] cl, a set of 'literals'
*/
Constraint* Clauses::addClause(vector<unsigned>& cl, int uniqVar)
{
    assert(cl.size() > 1);

    // If the DB is full, call reduce.
    if (Options::reduceDBOpt && nClausesInDB >= clauseDbSize)
        reduceDB();
    
    Stats::nbNoGoods++;
    nClausesInDB++;
    if (freeSpaces.empty()) {   // In case of no free spaces, add the new clause at the end and init struct
        watched[cl[0]].push_back(clauses.size());
        watched[cl[1]].push_back(clauses.size());
        refs.push_back(new RefClause(clauses.size()));
        clauses.push_back(cl);
        uniqVar.push_back(uniqVar);

        if (uniqVar == 2 && Options::keepBinary) {
            Stats::binary++;
            clState.push_back(3);
        } else 
            clState.push_back(2);

        refs.back()->claBumpActivity();
        return refs.back();
    } else {    // otherwise reinit the structs already existing
        assert(Options::reduceDBOpt);
        int id = freeSpaces.back();
        freopen.pop_back();

        watched[cl[0]].push_back(id);
        watched[cl[1]].push_back(id);
        clauses[id] = cl;
        uniqVars[id] = uniqVar;

        if (uniqVar == 2 && Options::keepBinary) {
            Stats::binary++;
            clState[id] = 3;
        } else
            clState[id] = 2;

        refs[id]->activity = .0;
        refs[id]->claBumpActivity();
        return refs[id];
    }
}   // addClause


bool Clauses::delClause(unsigned id)
{
    if (refs[id]->isReason) // Do not delete a clause that is reason !
        return false;
    
    nClausesInDB--;

    // Unsubscribe watched literals
    auto& w0 = watched[clauses[id][0]];
    for (auto it = w0.begin(); it != w0.end(); ++it)
        if (*it == id) {
            w0.areas(it);
            break;
        }

    auto& w1 = watched[clauses[id][1]];
    for (auto it = w1.begin(); it != w1.end(); ++it)
        if (*it == id) {
            w1.erase(it);
            break;
        }

    freeSpaces.push_back(id);
    return true;
}


void Clauses::reduceDB()
{
    assert(Options::reduceDBOpt);
    assert(freeSpaces.empty());

    // create the vec v of integer and put a seq of integer begining at 0 to represent clause IDs
    std::vector<int> v(clauses.size());
    std::iota(v.begin(), v.end(), 0);

    // Sort it by increasing num of unique variables, break ties by bigger activity 
    std::sort(v.begin(), v.end(), [this](int x, int y) {
        if (uniqVars[x] != uniqVars[y])
            return uniqVars[x] < uniqVars[y];
        else
            return refs[x]->activity  > refs[y]->activity;
    });

    double extra_lim = RefClause::cla_inc / nClausesInDB;
    // Remove the last half of clauses (if they aren't reason -> handled delClause)
    for (size_t i = v.size() >> 1, stop = v.size(); i < stop; ++i)
        if (clState[v[i]] != 3 && !refs[v[i]]->isReson && refs[v[i]]->activity < extra_lim)
            delClause(v[i]);

    clauseDbSize += 500;    // grow the clauses DB
}