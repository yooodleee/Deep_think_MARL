
#ifndef GENERALIZEDNOGOODSMANAGER_H_
#define GENERALIZEDNOGOODSMANAGER_H_

#include "Clauses.hh"
#include "ConsistencyHandler.hh"
#include "Stats.hh"



/**
 * Manage nogoods at revision, backtrack and restarts
 */
class GeneralizedNoGoodsManager {
private:
    Clauses* handler;
    ConsistencyHandler* cspAC;

    inline void assign(unsigned lit)
    {
        Variable* v = Variable::varProps[lit >> 1].toVar;
        int pos = Variable::varProps[lit >> 1].posInVar;

        if (lit & 1)
            v->removeAt(pos, cspAC->getDecisionLevel());
        else
            v->assignAt(pos, cspAC->getDecisionLevel());

        cspAC->addToQueue(v);
    }   // assign


public:
    GeneralizedNoGoodsManager(ConsistencyHandler* ac)
        : cspAC(ac)
    {
        handler = new Clauses();
    }

    GeneralizedNoGoodsManager(ConsistencyHandler* ac, Clauses* hand)
        : handler(hand)
        , cspAC(ac) {};

    Clauses* getNgHandler()
    {
        return handler;
    }

    void restart(std::vector<std::vector<unsigned>>& denied, std::vector<unsigned>& decided);

    friend std::ostream& operator<<(std::ostream& out, const GeneralizedNoGoodsManager& lnm)
    {
        out << "c" << sd::endl
            << "c # clauses = " << Stats::nbNoGoods << std::endl
            << "c # Ng supp = " << Stats::nbSupp << std::endl
            << "c # Ng conflicts = " << Stats::nbConflict << std::endl
            << "c # Unit clauses = " << Stats::unit << std::endl
            << "c # Unit(var) = " << Stats::unitVar << std::endl
            << "c # Bin(var) = " << Stats::binary;

        return out;
    }
};



#endif  // GENERALIZEDNOGOODSMANAGER_H_