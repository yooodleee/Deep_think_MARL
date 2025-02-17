
#ifndef CLAUSESPOSITIVE_H_
#define CLAUSESPOSITIVE_H_

#include <vector>

#include "Variable.hh"



class ClausesPositive {
private:
    std::vector<std::vector<ind>> clauses;
    std::vector<unsigned>* watched;

    size_t searchAnotherWatch(std::vector<ind>& cl);


public:
    /** Constructor */
    ClausesPositive()
    {
        watched = new std::vector<unsigned>[Variable::varProps.size()];
    }

    /** Propagator for nld-nogoods, do something only if @v is assigned */
    int propagate(int level, Variable* cur, std::vector<Variable*>& touched);

    /** Add the clause @cl to the base and init the watched literals */
    void addClause(std::vector<indVp>& cl);

    /** Return the number of nogoods recorded */
    size_t getNbClauses() {
        return clauses.size();
    }
};


#endif  // CLAUSESPOSITIVE_H_