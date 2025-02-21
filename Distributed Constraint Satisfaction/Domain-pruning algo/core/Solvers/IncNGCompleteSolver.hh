
#ifndef INCNGCOMPLETESOLVER_H_
#define INCNGCOMPLETESOLVER_H_


#include "CompleteSolver.hh"
class IncNGNoGoodsManager;



class IncNGCompleteSolver : public CompleteSolver {
protected:
    IncNGNoGoodsManager* manager;


public:
    /// Constructor
    IncNGCompleteSolver(Problem* p)
        : CompleteSolver(p)
        , manager(nullptr) {};
    

    /// Launch the main search loop
    int search(int zeroing = 0);


    /// Print the final info
    int showFinal(int res);
};



#endif  // INCNGCOMPLETESOLVER_H_