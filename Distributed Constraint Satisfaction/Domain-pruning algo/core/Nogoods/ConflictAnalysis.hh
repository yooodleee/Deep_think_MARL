
#ifndef CONFLICTANALYSIS_H_
#define CONFLICTANALYSIS_H_


class Constraint;


#include "Clauses.hh"
#include "Variable.hh"

#include <set>
#include <vector>
#include <utility>



class ConflictAnalysis {
protected:
    std::vector<unsigned> cl;
    std::vector<unsigned> conflictCl;
    std::set<int> uniqueVar;

    std::set<std::pair<unsigned, Expl*>, varOrderCmp> stack;

    Clauses* clHandler;

    inline std::pair<unsigned, Expl*> pop()
    {
        std::pair<unsigned, Expl*> lit = *stack.begin();
        stack.erase(stack.begin());
        return lit;
    }

    void buildStack(std::vector<unsigned>& conflictCl, int level);

    void printHeaderAnalyse(Constraint* ctr);


public:
    void setHandler(Clauses * clh) {
        clHandler = clh;
    }

    static unsigned long long gblCaStamp;

    virtual std::vector<unsigned> analyse(Constraint* ctr, int level, int& bjLevel, int& uniqVar);
};



#endif  // CONFLICTANALYSIS_H_