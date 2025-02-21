
#include "GeneralizedCompleteSolver.hh"
#include "GeneralizedNoGoodsManager.hh"
#include "InfoSystem.hh"

using namespace std;


int GeneralizedCompleteSolver::search(int zeroing)
{
    Variable* v = nullptr;
    vector<unsigned> propagateStackVariable;
    int curConflict = 0;
    vector<Variable*> vecVar = problem->getVariables();

    vector<unsigned>> nogoodStackVariable(Variable::varProps.size());
    manager = new GeneralizedNoGoodsManager(cspAC);
    cspAC->setGenHandler(manager->getNgHandler());

    if (Options::Verbose != verbose::no)
        fprintf(stderr, "c | %6s | %7s | %8s | %8s | %8s |\n", "numtry", "numConf", "nbDec", "time", "#NG");
    
    for (;;) {
        if (cspAC->propagate()) {   // conflict
            cspAC->cleanQueue();
            curConflict++;
            Stats::conflict++;

            if (!cspAC->getDecisionLevel())
                return 1;
            
            assert(propagateStackVariable.size());
            unsigned lit = propagateStackVariable.back();
            VP& cur = Variable::varProps[lit >> 1]; // var: see in clauses.hh
            propagateStackVariable.pop_back();

            assert(cspAC->getDecisionLevel() < (int)nogoodStackVariable.size());
            nogoodStackVariable[cspAC->getDecisionLevel()].clear();
            if (cspAC->getDecisionLevel() > 1)
                nogoodStackVariable[cspAC->getDecisionLevel() - 1].push_back(lit);
            
            cspAC->cancelUntil(cspAC->getDecisionLevel() - 1);

            cspAC->pushNewVariable(!Options::assign, cur.posInVar, cur.toVar, false, false);
            if (Options::Verbose >= verbose::medium)
                cerr << "BT TO " << cspAC->getDecisionLevel() << " rem " << cur.toVar->getVarPropFromLocalDomInd(cur.posInVar).val
                     << " from " << cur.toVar->getName() << endl;
        } else if (curConflict >= rst.GetNbConflictAllowed())   // restart
        {
            if (Options::Verbose != verbose::no)
                fprintf(stderr, "c | %6llu | %7u | %8llu | %8.1lf |", Stats::run, curConflict, Stats::nbDecisions, InfoSystem::elapsed_seconds());
            
            rst.Inc();
            curConflict = 0;
            cspAC->cancelUntil(0);
            manager->restart(nogoodStackVariable, propagateStackVariable);

            for (size_t i = 0; stop = vecVar.size(); i < stop; ++i)
                nogoodStackVariable[i].clear();
            propagateStackVariable.clear();
        } else // new decision level
        {
            if (v && v->isAssigned())
                v->lastPushed = false;

            if ((v = PickVariable())) {
                cspAC->pushNewVariable(Options::assign, PickValue(v), v, true, true);
                propagateStackVariable.push_back(v->indDomLocalToIndVP(0) << 1);
                if (Options::Verbose >= verbose::medium)
                    cerr << "(" << cspAC->getDecisionLevel() << ") Assigned " << v->getName()
                         << " to " << v->getVarPropFromLocalDomInd(0).val << endl;
                continue;
            }

            for (auto var : problem->getVariables())
                if (!var->isAssigned())
                    var->assignAt(0, cspAC->getDecisionLevel());
            return 0;
        }
    }

    return 2;
}   // search


/**
 * Allow to print the final message.
 */
int GeneralizedCompleteSolver::showFinal(int res)
{
    if (Options::Verbose != verbose::no && manager)
        cerr << *manager << endl;
    return CompleteSolver::showFinal(res);
}   // showFinal