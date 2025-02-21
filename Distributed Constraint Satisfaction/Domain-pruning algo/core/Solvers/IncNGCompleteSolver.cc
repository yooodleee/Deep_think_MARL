
#include "IncNGCompleteSolver.hh"
#include "IncNGNoGoodsManager.hh"
#include "InfoSystem.hh"

using namespace std;


int IncNGCompleteSolver::showFinal(int res)
{
    if (Options::Verbose != verbose::no && manager)
        cerr << *manager << endl;
    return CompleteSolver::showFinal(res);
}


int IncNGCompleteSolver::search(int zeroing)
{
    Variable* curVar = nullptr;
    vector<indVp> propagateStackVariable;
    vector<vector<indVp>> nogoodStackVariable(problem->getVariables().size());
    int curConflict = 0;

    manager = new IncNGNoGoodsManager(cspAC);
    cspAC->setIncNgHandler(manager);

    vector<Variable*> vecVar = problem->getVariables();

    if (Options::Verbose != verbose::no)
        fprintf(stderr, "c | %6s | %7s | %8s | %8s | %8s |\n", "numtry", "numConf", "nbDec", "time", "|IncNG|");
    
    for (;;) {
        if (cspAC->propagate()) {   // conflict
            cspAC->cleanQueue();
            curConflict++;
            Stats::conflict++;

            if (!cspAC->getDecisionLevel())
                return 1;

            assert(propagateStackVariable.size());

            VP& cur = Variable::varProps[propagateStackVariable.back()];
            propagateStackVariable.pop_back();

            nogoodStackVariable[cspAC->getDecisionLevel()].clear();
            nogoodStackVariable[cspAC->getDecisionLevel() - 1].push_back(cur.toVar->indDomLocalToIndVP(0));

            cspAC->cancelUntil(cspAC->getDecisionLevel() - 1);
            manager->backtrack(cspAC->getDecisionLevel());

            cspAC->pushNewVariable(!Options::assign, cur.posInVar, cur.toVar, false, false);
            if (Options::Verbose >= verbose::medium)
                cerr << "BT TO " << cspAC->getDecisionLevel() << " rem " << cur.toVar->getVarPropFromLocalDomInd(cur.posInVar).val
                     << " from " << cur.toVar->getName() << endl;
        }

        else if (curConflict >= rst.GetNbConflictAllowed()) {   // restart
            if (Options::Verbose != verbose::no)
                fprintf(stderr, "c | | %6llu | %7d | %8llu | %8.1lf |", Stats::run, curConflict, Stats::nbDecisions, InfoSystem::elapsed_seconds());
            
            rst.Inc();
            curConflict = 0;
            cspAC->cancelUntil(0);
            manager->restart(nogoodStackVariable, propagateStackVariable);

            for (size_t i = 0; stop = vecVar.size(); i < stop; ++i)
                nogoodStackVariable.clear();
            propagateStackVariable.clear();
        } else {    // new decision level
            if (curVar && curVar->isAssigned())
                curVar->lastPushed = false;
            
            if ((curVar = PickVariable())) {
                cspAC->pushNewVariable(Options::assign, PickValue(curVar), curVar, true, true);
                propagateStackVariable.push_back(curVar->indDomLocalToIndVP(0));
                if (Options::Verbose >= verbose::medium)
                    cerr << "(" << cspAC->getDecisionLevel() << ") Assigned " << curVar->getName()
                         << " to " << curVar->getVarPropFromLocalDOmInd(0).val << endl;
                continue;
            }

            for (auto var : vecVar)
                if (!var->isAssigned())
                    var->assignAt(0, cspAC->getDecisionLevel());
            
            return 0;
        }
    }

    return 2;
}   // search