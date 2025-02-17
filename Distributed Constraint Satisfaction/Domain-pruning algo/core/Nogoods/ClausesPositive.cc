
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


