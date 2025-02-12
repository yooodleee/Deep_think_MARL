
#ifndef ANALYSISCONSISTENCYHANDLER_H_
#define ANALYSISCONSISTENCYHANDLER_H_


#include "AnalysisConsistencyHandler.hh"
#include "Clauses.hh"
#include "ConflictAnalysis.hh"
#include "ConsistencyHandler.hh"
#include "Options.hh"
#include "Problem.hh"
#include "Variable.hh"


#include <stack>


class AnalysisConsistencyHandler : public ConsistencyHandler {
private:
    std::vector<Variable*> clQueue;

    Variable* pickVariable() override
    {
        assert(!propagateQueue.empty());

        Variable* vTmp = nullptr;
        int domSize = std::numeric_limits<int>::max();

        auto toRem = propagateQueue.end();
        for (auto it = propagateQueue.begin(); it != propagateQueue.end(); ++it) {
            inf curDom = (*it)->domainCurSize;
            if (curDom < domSize) {
                domSize = curDom;
                vTmp = *it;
                toRem = it;
            }
        }

        assert(vTmp);
        assert(toRem != propagateQueue.end());

        if (vTmp->clStamp != clStamp) {
            vTmp->clStamp = clStamp;
            clQueue.push_back(vTmp);
        }

        propagateQueue.erase(toRem);
        vTmp->propagateStamp = 0;
        return vTmp;
    }

    unsigned long long clStamp = 1;

public:
    /// Constructor
    AnalysisConsistencyHandler(Problem* p)
        : ConsistencyHandler(p){};

    ~AnalysisConsistencyHandler(){};

    inline void cleanQueue() override
    {
        consistencyHandler::cleanQueue();
        clQueue.clear();
        clStamp++;
    }
    
    /*
    * Perform AC on the propagation queue by variable minDom and with stamps.
    *
    * @param[in] inConflict if set to false it doesn't increase the score of constraints and variables.
    * 
    * @return true if an inconsistency is detected.
    */
   Constraint* propagate(bool incConflict = true);
};



#endif  // ANALYSISCONSISTENCYHANDLER_H_