
#ifndef CLAUSES_H_
#define CLAUSES_H_

#include "Stats.hh"
#include "Variable.hh"

#include <vector>


class RefClause;


class Clauses {
protected:
    std::vector<std::vector<unsigned>> watched;
    std::vector<unsigned> freeSpaces;

    std::vector<unsigned> clState;
    std::vector<unsigned> uniqVars;

    std::vector<bool> variablesUtil;
    std::vector<Variable*> varPtrUtil;

    int clauseDbSize = 4000;
    int nClausesInDB = 0;

    virtual int checkWatch(unsigned lit, int level, std::vector<Variable*>& touched);

    bool delClause(unsigned id);
    void reduceDB();

    inline bool sign(unsigned x) {
        return x & 1;
    }


public:
    std::vector<std::vector<unsigned>> clauses;
    std::vector<RefClause*> refs;

    /** Constructor */
    Clauses()
    {
        watched.resize(Variable::varProps.size() << 1);
        variablesUtil.resize(Variable::varProps.back().toVar->getId());
    }

    /** Propagator for nogoods */
    int propagate(int level, Variable* cur, std::vector<Variable*>& touched);

    /** Add the clause @cl to the base and init the watched literals */
    Constraint* addClause(std::vector<unsigned>& cl, int uniqVar = 0);
};



#endif  // CLAUSES_H_