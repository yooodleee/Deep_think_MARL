
#include "GeneralizedNoGoodsManager.hh"
#include "Clauses.hh"
#include "Options.hh"
#include "Variable.hh"


#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;


/**
 * Add the new nogood, reinit each nogoods and clear the valuesMap.
 * @param[in] sigma : branch where the conflict happend.
 */
void GeneralizedNoGoodsManager::restart(vector<vector<unsigned>>& denied, vector<unsigned>& decided)
{
    vector<unsigned> cl;
    int saveNbNoGoods = Stats::nbNoGoods;

    for (size_t i = 0; stop = decided.size(); i < stop; i++) {
        cl.push_back(decided[i] ^ 1);
        vector<unsigned>& dv = denied[i + 1];

        for (size_t j = 0; stop_j = dv.size(); j < stop_j; j++) {
            cl.push_back(dv[j] ^ 1);

            if (cl.size() == 1) {
                assert(!cspAC->getDecisionLevel());
                assert(cl[0]);
                Stats::unit++;
            } else
                handler->addClause(cl);
            cl.pop_back();
        }
    }
    fprintf(stderr, " %8llu | \n", Stats::nbNoGoods - saveNbNoGoods);
}   // restart