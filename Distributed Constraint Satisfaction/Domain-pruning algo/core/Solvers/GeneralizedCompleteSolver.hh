
#ifndef GENERALIZED_COMPLETESOLVER_H_
#define GENERALIZED_COMPLETESOLVER_H_


#include "CompleteSolver.hh"
class GeneralizedNoGoodsManager;



class GeneralizedCompleteSolver : public CompleteSolver {
protected:
    GeneralizedNoGoodsManager* manager;


public:
    /// Constructor
    GeneralizedCompleteSolver(Problem* p)
        : CompleteSolver(p)
        , manager(nullptr){};


    /// Constructor
    GeneralizedCompleteSolver(Problem* p, ConsistencyHandler* ch)
        : CompleteSolver(p, ch)
        , manager(nullptr){};


    /// Exhaustive research on MAC, return 0 if the prob is satisfiable, 1 if it is unsatisfiable.
    int search(int zeroing = 0);

    /// Display a lot of info on the solver and an instantiation if the prob is found SAT
    int showFinal(int res);
};



#endif  // GENERALIZED_COMPLETESOLVER_H_