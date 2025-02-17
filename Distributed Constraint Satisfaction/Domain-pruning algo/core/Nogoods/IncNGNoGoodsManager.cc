
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


/*
####################
#      Public      #
####################
*/

/**
 * Add the new nogood, reinit each nogoods and clear the valuesMap. 
 * 
 * @param[in] sigma : the branch where the conflict happend. 
 */
void IncNGNoGoodsManager::restart(vector<vector<indVp>> negValues, vector<indVp> posValues)
{
    if (!posValues.empty()) {
        IncNG* ng = new IncNG(posValues, negValues, sizeVarMap);

        size_t curNbNg = 0;
        for (size_t i = 0; i < posValues.size(); ++i)
            curNbNg += negValues[i + 1].size();
        fprintf(stderr, " %8zu | \n", curNbNg + posValues.size());
        Stats::nbNoGoods += curNbNg;

        if (ng->m())
            nogoods.push_back(ng);
    } else {
        fprintf(stderr, " %8d | \n", 0);
    }

    reinitBase();
}


/**
 * Save the new level and backtrack (or undelete) a nogood to the previous
 * registered level. 
 * 
 * @param[in] level The new level. 
 */
void IncNGNoGoodsManager::backtrack(int level)
{
    if (cspAC->getDecisionLevel() == level)
        return;
    
    for (auto ng : nogoods) {
        // restore the nogoods deleted during the last search
        if (ng->deleted) {
            if (ng->dlevel <= cspAC->getDecisionLevel())
                continue;
            ng->deleted = false;
            ng->dlevel = -1;
        }

        if (ng->incNgList.size() <= 1 || ng->level() <= cspAC->getDecisionLevel())
            continue;

        int preB = ng->beta();
        ng->incNgList.pop_back();
        if (preB == ng->beta())
            continue;
        

        // Put the nogood to its previous state by unwatching the range (beta, previousBeta]
        if (preB < (int)ng->pos.size())
            unwatch(ng, ng->pos[preB]);
        for (auto n : ng->get[ng->beta()])
            unwatch(ng, n);

        for (int i = ng->beta() + 1; i < preB; i++) {
            unwatch(ng, ng->pos[i]);
            for (auto n : ng->neg[i + 1])
                unwatch(ng, n);
        }
    }
}


