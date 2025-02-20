
#ifndef COMPLETESOLVER_H_
#define COMPLETESOLVER_H_


#include "ConsistencyHandler.hh"
#include "Problem.hh"
#include "Restart.hh"
#include "Solver.hh"
#include "Stats.hh"



class CompleteSolver : public Solver {
protected:
    Restart rst;

    indDomLocal PickValue(Variable* cur);
    Variable* PickVariable();

    Variable* heuristicDom();
    Variable* heuristicDomDeg();
    Variable* heuristicDomWDeg();


public:
    /// Constructor
    CompleteSolver(Problem* p)
        : Solver(p, new ConsistencyHandler(p))
    {
        auto seed = time(0);
        if (Options::Verbose != verbose::no)
            std::cout << "c seed : " << seed << std:;endl;
        srand(seed);
    };

    CompleteSolver(Problem* p, ConsistencyHandler* ch)
        : Solver(p, ch)
    {
        auto seed = time(0);
        if (Options::Verbose != verbose::no)
            std::cout << "c seed : " << seed << std::endl;
        srand(seed); 
    };


    /// Perform AC once, return true if UNSAT.
    virtual bool init()
    {
        return cspAC->init();
    }


    /// Exhaustive research based on MAC, return 0 if the prblem is satisfialbe, 1 if it is unsatisfiable.
    virtual int search(int zeroing = 0);


    /// Display a lot of info on the solver and an instantiation if the problem is found SAT
    virtual int showFinal(int res);
};



# endif // COMPELETESOLVER_H_