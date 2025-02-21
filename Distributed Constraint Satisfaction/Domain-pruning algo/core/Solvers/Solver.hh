
#ifndef SOLVER_H_
#define SOLVER_H_


#include "ConsistencyHandler.hh"
#include "Problem.hh"



class Solver {
protected:
    Problem* problem;
    ConsistencyHandler* cspAC;


public:
    /// Constructor
    Solver(Problem* p, ConsistencyHandler* ch)
        : problem(p)
        , cspAC(ch) {};


    inline void setConsistencyHandler(ConsistencyHandler* ch) { cspAC = ch; }
    inline ConsistencyHandler* getConsistencyHandler() { return cspAC; }


    /// Function used for init and preprocessing
    virtual bool init() = 0;


    /// Function used for the main loop of the solver
    virtual int search(int zeroing = 0) = 0;

    
    /// Call at the end of the exec or when interrupted, must print infos about solver
    virtual int showFinal(int res) = 0;


    /// Function used to launch the solver
    virtual int solve()
    {
        if (init())
            return 1;
        return search();
    }
};



#endif  // SOLVER_H_