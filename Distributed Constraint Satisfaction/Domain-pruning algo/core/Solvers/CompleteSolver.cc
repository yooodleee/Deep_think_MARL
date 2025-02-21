
#include "CompleteSolver.hh"
#include "Constraint.hh"
#include "InfoSystem.hh"
#include "Options.hh"
#include "Solver.hh"
#include "Variable.hh"


#include <cassert>
#include <cstring>
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;


int CompleteSolver::search(int zeroing)
{
    Variable* v = nullptr;
    vector<VP*> propagateStackVariable;
    int curConflict = 0;

    InfoSystem::elapsed_seconds();

    if (Options::Verbose != verbose::no)
        fprintf(stderr, "c | %6s | %7s | %8s | %7s | %10s |\n", "numtry", "numConf", "nbDec", "time", "Mem(Mb)");
    for (;;) {
        if (cspAC->propagate()) {   // conflict
            cspAC->cleanQueue();
            curConflict++;
            Stats::conflict++;
            if (Options::optRst == rstTypes::noRst && Options::Verbose != verbose::no && !(Stats::conflict % 10000))
                fprintf(stderr, "c | %6llu | %7d | %8llu | %7.1lf | %10.3lf |\n",
                    Stats::run, curConflict, Stats::nbDecisions, InfoSystem::timeElapsed(), (double)InfoSystem::getMemUsed() / 1000);
            
            if (cspAC->getDecisionLevel() == zeroing)
                return 1;

            cspAC->cancelUntil(cspAC->getDecisionLevel() - 1);

            assert(propagateStackVariable.size());

            VP cur = *(propagateStackVariable.back());
            propagateStackVariable.pop_back();

            cspAC->pushNewVariable(!Options::assign, cur.posInVar, cur.toVar, false, false);
            if (Options::Verbose >= verbose::medium)
                cerr << "BT TO " << cspAC->getDecisionLevel() << cur.toVar->getName() << endl;
        } else if (curConflict >= rst.GetNbConflictAllowed()) {     // restart
            propagateStackVariable.clear();

            if (Options::Verbose != verbose::no)
                fprintf(stderr, "c | %6llu | %7d | %8llu | %7.1lf | %10.3lf |\n",
                    Stats::run, curConflict, Stats::nbDecisions, InfoSystem::elapsed_seconds(), (double)InfoSystem::getMemUsed() / 1000);
            
            rst.Inc();
            curConflict = 0;
            cspAC->cancleUntil(zeroing);
        } else {    // new decision level
            if (v && v->isAssigned())   
                v->lastPushed = false;

            if ((v = PickVariable())) {
                indDomLocal iDom = PickValue(v);
                propagateStackVariable.push_back(&(v->getVarPropFromLocalDomInd(iDom)));
                if (Options::Verbose >= verbose::medium)
                    cerr << "(" << cspAC->getDecisionLevel() << (Options::assign ? ") Assigned " : ") Refuted ") << v->getName()
                         << (Options::assign ? " to " : " from ") << v->getVarPropFromLocalDomInd(iDom).val << endl;
                cspAC->pushNewVariable(Options::assign, iDom, v, true, true);
                continue;
            }

            if (Options::countSols == countTypes::no) {
                for (auto var : problem->getVariables()) {
                    if (!var->isAssigned())
                        var->assignAt(0, cspAC->getDecisionLevel());
                }
                return 0;
            }

            Stats::nbSols++;

            if (Options::Verbose >= verbose::high) {
                cout << "v <instantiation> <list> ";
                for (auto var : problem->getVariable())
                    cout << var->getName() << " ";
                cout << "</list> <values>";
                for (auto var : problem->getVariables())
                    cout << var->getVarPropFromLocalDomInd(0).val << " ";
                cout << "</values> </instantiation>" << endl
                     << endl;
            }

            cspAC->cleanQueue();
            if (cspAC->getDecisionLevel() == zeroing)
                return 1;

            cspAC->cancelUntil(cspAC->getDecisionlevel() - 1);

            assert(propagateStackVariable.size());

            VP cur = *(propagateStackVariable.back());
            propagateStackVariable.pop_back();
            if (Options::Verbose >= verbose::medium)
                cerr << "sol found removing val " << cur.toVar->getVarPropFromLocalDomInd(cur.posInVar).val
                     << " from " << cur.toVar->getName() << endl;
            cspAC->pushNewVariable(!Options::assign, cur.posInVar, cur.toVar, false, false);
        }
    }

    return 2;
}   // search



indDomLocal CompleteSolver::PickValue(Variable* cur)
{
    if (Options::saving && cur->isValidLastChoice())
        return cur->lastChoiceToDomLocalInd();
    
    if (Options::valueSelection == valueSelectionTypes::min)
        return cur->getLowerBoundLocalDomInd();

    if (Options::valueSelection == valueSelectionTypes::zero)
        return 0;

    if (Options::valueSelection == valueSelectionTypes::max)
        return cur->getUpperBoundLocalDomInd();
    
    if (Options::valueSelection == valueSelectionTypes::last)
        return cur->domainCurSize - 1;
    
    if (Options::valueSelection == valueSelectionTypes::vrand) {
        return rand() % cur->domainCurSize;
    }

    throw runtime_error("Something went wrong (Options compatibility maybe!?)");
}   // PickVariable



/**
 * Method permettant de choisir la prochaine variable à affecter.
   @return une variable, ou nullptr si il n'y a plus de variable à affecter.
 */
Variable* CompleteSolver::PickVariable()
{
    if (Options::varHeuristic == variableHeuristic::domwdeg)
        return heuristicDomDeg();

    if (Options::varHeuristic == variableHeuristic::dom)
        return heuristicDom();

    if (Options::varHeuristic == variableHeuristic::domdeg)
        return heuristicDomDeg();

    throw runtime_error("Something went wrong (Options compatibility maybe!?)");
}   // PickVariable



Variable* CompleteSolver::heuristicDom()
{
    Variable* retV = nullptr;
    int best = numeric_limits<int>::max();
    int weight;

    for (auto vTmp : problem->getVariables()) {
        if (vTmp->isAssigned())
            continue;

        weight = vTmp->domainCurSize;
        if (weight < best) {
            best = weight;
            retV = vTmp;
        }
    }

    return retV;
}



