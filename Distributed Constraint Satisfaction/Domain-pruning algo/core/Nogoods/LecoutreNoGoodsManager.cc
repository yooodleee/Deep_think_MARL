
#include "LecoutreNoGoodsManager.hh"
#include "Options.hh"
#include "Variable.hh"


#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;


/**
 * Add the new nogood, reinit each nogoods and clear the valueMap.
 * 
 * @param[in] sigma : the branch where the conflict happend. 
 */
void LecoutreNoGoodsManager::restart(vector<vector<indVp>> negSet, vector<indVp>& posSet)
{
    vector<indVp> prefix;
    int saveNbNoGoods = Stats::nbNoGoods;

    assert(negSet[0].empty());
    assert(!cspAC->getDecisionLevel());

    for (size_t i = 0; i < posSet.size(); ++i) {
        prefix.push_back(posSet[i]);
        for (size_t j = 0; i < negSet[i + 1].size(); ++j) {
            vector<indVp> newNG(prefix);
            newNG.push_back(negSet[i + 1][j]);
            handler->addClause(newNG);
        }
    }

    fprintf(stderr, " %8llu | \n", Stats::nbNoGoods - saveNbNoGoods);
}