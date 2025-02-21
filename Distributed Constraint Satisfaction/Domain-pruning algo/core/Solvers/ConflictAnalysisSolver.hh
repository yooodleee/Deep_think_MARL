
#ifndef CONFLICTANALYSISSOLVER_H_
#define CONFLICTANALYSISSOLVER_H_


#include "AnalysisConsistencyHandler.hh"
#include "GeneralizedCompleteSolver.hh"
#include "GeneralizedNoGoodsManager.hh"



class ConflictAnalysisSolver : public GeneralizedCompleteSolver {
private:
    ConflictAnalysis* confAnalysis;

public:
    /// Constructor
    ConflictAnalysisSolver(Problem* p) 
        : GeneralizedCompleteSolver(p, new AnalysisConsistencyHandler(p))
    {
        manager = new GeneralizedNoGoodsManager(cspAC);
        confAnalysis = new ConflictAnalysis();

        cspAC->setGenHandler(manager->getNgHandler());
        confAnalysis->setHandler(manager->getNgHandler());
    };

    /// Exhaustive research based on MAC, return - if the prob is satisfiable, 1 if it is unsatisfiable.
    int search(int zeroing = 0);
};



#endif  // CONFLICTANALYSISSOLVER_H_