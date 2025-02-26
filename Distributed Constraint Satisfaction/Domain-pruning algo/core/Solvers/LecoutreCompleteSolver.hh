
#ifndef LECOUTRECOMPLETESOLVER_H_
#define LECOUTRECOMPLETESOLVER_H_


#include "CompleteSolver.hh"
class LecoutreNoGoodsManager;


class LecoutreCompleteSolver : public CompleteSolver {
protected:
    LecoutreNoGoodsManager* manager;


public:
    /// Constructor
    LecoutreCompleteSolver(Problem* p)
        : CompleteSolver(p)
        , manager(nullptr) {};
    

    /// Launch the main search loop
    int search(int zeroing = 0);


    /// Print the final info
    ins showFinal(int res);
};



#endif  // LECOUTRECOMPLETESOLVER_H_