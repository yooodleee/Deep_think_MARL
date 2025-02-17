
#ifndef INCNGNOGOODSMANAGER_H_
#define INCNGNOGOODSMANAGER_H_

#include "ConsistencyHandler.hh"
#include "IncNG.hh"
#include "IncNGCompleteSolver.hh"
#include "Stats.hh"


typedef struct {
    IncNG nogoodn;
    int counter;
} varMapNode;

/**
 * Manage nogoods at revision, backtrack and restarts
 */

class IncNGNoGoodsManager {
private:
    // Map of the connection between InfoValueVar and nogoods (where they are watched)
    std::vector<varMapNode>* varMap;
    int sizeVarMap;
    unsigned long long stamp;

    ConsistencyHandler* cspAC;

    void watch(IncNG* ng, indVp vp)
    {
        int var = Variable::varProps[vp].toVar->getId();
        if (ng->watchMap[var] != -1)
            varMap[var][ng->watchMap[var]].counter++;
        else {
            ng->watchMap[var] = varMap[var].size();
            varMap[var].push_back({ ng, 1 });
        }
    }

    // Decrements the counter of the nogoodnode in the watched var.
    void unwatch(IncNG* ng, indVp vp)
    {
        int var = Variable::varProps[vp].toVar->getId();
        if (ng->watchMap[var] != -1)
            varMap[var][ng->watchMap[var]].counter--;
    }

    bool findNewBeta(IncNG* ngn, int offset, std::vector<Variable*>& touched);
    bool updateAlpha(IncNG* ngn, std::vector<Variable*>& touched);
    bool updateBeta(IncNG* ngn, std::vector<Variable*>& touched);
    bool filter(IncNG* ng, std::vector<Variable*>& touched);
    void reinitBase();

    inline bool fixValue(indVp val) {
        return fixValue(val, cspAC->getDecisionLevel());
    }
    inline bool fixValue(indVp val, int level)
    {
        cspAC->addToQueue(Variable::varProps[val].toVar);
        return Variable::varProps[val].toVar->assignAt(Variable::varProps[val].posInVar, level);
    }

    inline bool deleteValue(intVp val) {
        return deleteValue(val, cspAC->getDecisionLevel());
    }
    inline bool deleteValue(indVp val, int val)
    {
        cspAC->addToQueue(Variable::varProps[val].toVar);
        return Variable::varProps[val].toVar->removeAt(Variable::varProps[val].posInVar, level);
    }

    inline bool deleteValue(indVp val, std::vector<Variable*>& touched) {
        return deleteValue(val, cspAC->getDecisionLevel(), touched);
    }
    inline bool deleteValue(indVp val, int level, std::vector<Variable*>& touched)
    {
        VP& vp = Variable::varProps[val];
        touched.push_back(vp.toVar);
        return vp.toVar->removeAt(vp.posInVar, level);
    }


public:
    // Constructor with @m the type of minimization
    IncNGNoGoodsManager(ConsistencyHandler* ac)
        : sizeVarMap(Stats::nbVars)
        , stamp(0)
        , cspAC(ac)
    {
        varMap = new std::vector<varMapNode>[sizeVarMap];
    }

    // Nogoods storage
    std::vector<IncNG*> nogoods;

    // Restore all nogoods to the state they were at @level
    void backtrack(int level);
    // Add a new nogood to the database and reinit the others
    void restart(std::vector<std::vector<indVp>> negValues, std::vector<indVp> posValues)

    bool revise(Variable* cur, std::vector<Variable*>& touched);

    friend std::ostream& operator<<(std::ostream& out, const IncNGNoGoodsManager& inm)
    {
        out << "c" << std::endl
            << "c # nogoods = " << Stats::nbNoGoods << std::endl 
            << "c $ Ng supp = " << Stats::nbSupp << std::endl
            << "c # Ng conflicts = " << Stats::nbConflict;
        
        return out;
    }
};


#endif  // INCNGNOGOODSMANAGER_H_