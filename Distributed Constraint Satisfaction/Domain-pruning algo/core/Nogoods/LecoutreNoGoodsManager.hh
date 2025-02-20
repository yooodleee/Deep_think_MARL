
#ifndef LECOUTRENOGOODSMANAGER_H_
#define LECOUTRENOGOODSMANAGER_H_

#include "ClausesPositive.hh"
#include "ConsistencyHandler.hh"
#include "LecoutreCompleteSolver.hh"
#include "Stats.hh"



/**
 * Manage nogoods at revision, backtrack and restarts
 */
class LecoutreNoGoodsManager {
private:
    ClausesPositive* handler;
    ConsistencyHandler* cspAC;


public:
    LecoutreNoGoodsManager(ConsistencyHandler* ac)
        : cspAC(ac)
    {
        handler = new ClausesPositive();
    }

    ClausesPositive* getNgHandler()
    {
        return handler;
    }

    void blockSolution(std::vector<indVp>& solution) { handler->addClause(solution); }

    void restart(std::vector<std::vector<indVp>> negSet, std::vector<indVp>& posSet);

    friend std::ostream& operator<<(std::ostream& out, const LecoutreNoGoodsManager& lnm)
    {
        out << "c" << std::endl
            << "c # nogoods = " << Stats::nbNoGoods << std::endl
            << "c # Ng supp = " << Stats::nbSupp << std::endl
            << "c # Ng conflicts = " << Stats::nbConflict;
        
        return out;
    }
};



#endif  // LECOUTRENOGOODSMANAGER_H_