
#ifndef CONSISTENCYHANDLER_H_
#define CONSISTENCYHANDLER_H_


#include "Clauses.hh"
#include "ClausesPositive.hh"
#include "ConflictAnalysis.hh"
#include "Problem.hh"
#include "Stats.hh"
#include "Variable.hh"


#include <stack>



class IncNGNoGoodsManager;

class ConsistencyHandler {
protected:
    Problem* problem;

    std::vector<Variable*> propagateQueue;
    std::vector<Variable*> touched;

    int decisionLevel;
    unsigned long long gblStamp;

    Clauses* genHandler;
    ClausesPositive* ngHandler;
    IncNGNoGoodsManager* incNgHandler;

    virtual Variable* pickVariable();

    inline void increase(Constraint* c) {
        for (auto v : c->getScope())
            v->score++;
    }

public:
    /// Constructor
    ConsistencyHandler(problem* p)
        : problem(p)
        , decisionLevel(0)
        , gblStamp(0)
        , genHandler(nullptr)
        , ngHandler(nullptr),
        , incNgHandler(nullptr){};

    virtual ~ConsistencyHandler(){};

    unsigned long long propagateStamp = 1;

    /// Initialized the problem (variables and constraints) and perform AC on it.
    /// Return true if an inconsistency is detected.
    bool init();

    /// Backtrack the problem to the level @lev
    void cancelUntil(int lev);

    inline void setGenHandler(Clauses* cl) {
        genHandler = cl;
        RefClauses::dbClauses = cl;
    }
    inline void setNgHandler(ClausesPositive* cl) { ngHandler = cl; }
    inline void setIncNgHandler(IncNGNoGoodsManager* cl) { incNgHandler = cl; }

    /*
    * If @sign is tre assign the variable @var th value of his sparse set
    * Else (@sign is false) remove the value from @var domain.
    * 
    * @param[in] sign if set to true assign, else remove
    * @param[in] v the local index of the domain of @var
    * @param[in] var the variable where the change is performed
    * @param[in] count if true increase @nbDecision if @newDec is set to true
    * @param[in] newDec if true increase @decisionLevel
    */
    void pushNewVariable(bool sign, incDomLocal v, Variable* var, bool count, bool newDec);

    /*
    * perform AC on the propagation queue by variable minDom and with stamps.

    * @param[in] incConflict if set to false it doesn't increase the score of constraints and variables. 
    *
    * @return true if an inconsistency is detected.
    */
    bool propagate(bool incConflict = true);

    /// Empty the propagation queue
    virtual inline void cleanQueue() {
        propagateQueue.clear();
        propagateStamp++;
    }

    /// Getter for the current decision level
    inline int getDecisionLevel() { return decisionLevel; }

    /// Setter for the decision level
    inline void setDecisionLevel(int dl) {
        assert(dl >= decisionLevel);
        decisionLevel = dl;
    }

    inline void addToQueue(Variable* v) {
        v->ctrStamp = ++gblStamp;
        if (propagateStamp != v->propagateStamp) {
            v->propagateStamp = propagateStamp;
            propagateQueue.push_back(v);
        }
    }
};


#endif // CONSISTENCYHANDLER_H_