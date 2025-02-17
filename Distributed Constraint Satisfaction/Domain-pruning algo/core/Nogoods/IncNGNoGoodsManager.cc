
#include "IncNGNoGoodsManager.hh"
#include "Options.hh"
#include "Variable.hh"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

/**
 * Search from @offset for the next positive value in the branch and check
 * every negatives ones.
 * 
 * @param ng a nogood. 
 * @param offset the position where the search begin
 */
bool IncNGNoGoodsManager::findNewBeta(IncNG* ng, int offset, vector<Variable*>& touched)
{
    indVp ivpA = ng->pos[ng->alpha()];

    for (auto n : ng->neg[offset + 1]) {
        Variable* v = Variable::varProps[n].toVar;

        // If a neg value is assigned to its forbiden value => delete alpha value and stop using this ng
        if (v->isAssignedToIndVp(n)) {
            if (deleteValue(ivpA, touched)) {   // Domain wipeout of alpha
                Stats::nbConflict++;    // CONFLICT : negative val is assined to the forbidden val
                return true;
            }

            Stats::nbSupp++;

            ng->setM(0);
            return false;
        }

        watch(ng, n);
    }

    if (offset + 1 < ng->m()) {
        watch(ng, ng->pos[offset + 1]);
        ng->setBeta(offset + 1);
    } else
        ng->setBeta(ng->m());

    return false;
}

/**
 * If beta value is assigned, find a new one.
 * 
 * @param[in] ng a nogood.
 */
bool IncNGNoGoodsManager::updateBeta(IncNG* ng, vector<Variable*>& touched)
{
    indVp ivp = ng->pos[ng->beta()];
    VP& vp = Variable::varProps[ivp];

    if (!vp.toVar->isAssigned())
        return false;

    if (!vp.posInVar) { // If beta is assigned to the expected value : update it
        if (findNewBeta(ng, ng->beta(), touched))
            return true;
        if (ng->m() != 0 && ng->beta() != ng->m())
            return updateBeta(ng, touched);
    } else {    // If beta is assigned to an unexpected value : truncate nogood
        for (int i = ng->alpha(), beta = ng->beta(); i < beta; ++i)
            if (!ng->neg[i + 1].empty()) {
                ng->setM(beta + 1);
                return false;
            }
        ng->setM(0);
    }

    return false;
}

/**
 * Nogoods global constraint filter. 
 * 
 * @param[in] ng a nogood. 
 * 
 * @return true if conflict. 
 */
bool IncNGNoGoodsManager::filter(IncNG* ng, vector<Variable*>& touched)
{
    if (ng->deleted || ng->stamp == stamp)
        return false;
    
    ng->stamp = stamp;

    if (ng->level() < cspAC->getDecisionLevel())
        ng->incNgList.push_back({ ng->m(), ng->alpha(), ng->beta(), cspAC->getDecisionLevel() });
    
    if (updateAlpha(ng, touched))
        return true;

    if (ng->m() && ng->beta() != ng->m() && updateBeta(ng, touched))
        return true;

    if (!ng->m()) {
        ng->deleted = true;
        ng->dlevel = cspAC->getDecisionLevel();
    }

    return false;
}


void IncNGNoGoodsManager::reinitBase()
{
    for (int i = 0; i < sizeVarMap; ++i)
        varMap[i].clear();

    for (auto ng : nogoods) {
        ng->incNgList.clear();
        ng->incNgList.push_back({ (int)ng->pos.size(), 0, 1, 0 });
        ng->deleted = false;
        ng->dlevel = -1;
        ng->stamp = -1;

        for (int i = 0; i < sizeVarMap; ++i)
            ng->watchMap[i] = -1;

        watch(ng, ng->pos[0]);
        for (auto n : ng->neg[1])
            watch(ng, n);
        if (ng->pos.size() > 1)
            watch(ng, ng->pos[1]);
    }
}

