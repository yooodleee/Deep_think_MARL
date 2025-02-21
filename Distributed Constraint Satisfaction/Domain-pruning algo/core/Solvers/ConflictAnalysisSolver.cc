
#include "ConflictAnalysisSolver.hh"
#include "InfoSystem.hh"
#include "Options.hh"

using namespace std;


int ConflictAnalysisSolver::search(int zeroing)
{
    VP* vp = nullptr;
    int curConflict = 0;
    vector<unsigned> learnt_clause;
    int bjLevel = -1;
    int uniqVar = -1;
    AnalysisConsistencyHandler* AChandler = static_cast<AnalysisConsistencyHandler*>(cspAC);

    if (Options::Verbose != verbose::no) {
        if (Options::optRst == rstTypes::noRst)
            fprintf(stderr, "c | %6s | %7s | %8s | %7s | %10s |\n", "numtry", "numConf", "nbDec", "time", "Mem(Mb)", "#uLit");
        else
            fprintf(stderr, "c | %6s | %7s | %8s | %8s | %10s | \n", "numtry", "numConf", "nbDec", "time", "#NG", "#uLit");
    }

    for (;;) {
        Constraint* c = AChandler->propagate();

        if (c != nullptr) {     // conflict
            AChandler->cleanQueue();
            curConflict++;
            Stats::conflict++;
            if (Options::optRst == rstTypes::noRst && Options::Verbose != verbose::no && !(Stats::conflict % 10000))
                fprintf(stderr, "c | %6llu | %7d | %8llu | %7.1lf | %10.3lf | %10llu |\n",
                    Stats::run, curConflict, Stats::nbDecision, InfoSystem::timeElapsed(), (double)Infosystem::getMemUsed() / 1000, Stats::unit);
            
            if (!AChandler->getDecisionLevel()) // conflict at level 0 => UNSAT
                return 1;
            
            learnt_clause.clear();
            learnt_clause = confAnalysis->analyse(c, AChandler->getDecisionLevel(), bjLevel, uniqVar);

            if (learnt_clause.empty())
                return 1;
            
            AChandler->cancelUntil(bjLevel);

            Constraint* ref = nullptr;
            if (learnt_clause.size() != 1)
                ref = manager->getNgHandler()->addClause(learnt_clause, uniqVar);

            VP& vp = Variable::varProps[learnt_clause[0] >> 1];

            if (learnt_clause.size() == 1)
                Stats::unit++;

            if ((learnt_clause[0] & 1))
                vp.toVar->removeAt(vp.posInVar, AChandler->getDecisionLevel(), ref);
            else
                vp.toVar->assignAt(vp.posInVar, AChandler->getDecisionLevel(), ref);
            Stats::nbSupp++;

            AChandler->addToQueue(vp.toVar);

            RefClause::claDecayActivity();
            
        } else if (curConflict >= rst.GetNbConflictAllowed()) { // restart
            if (Options::Verbose != verbose::no)
                fprintf(stderr, "c | %6llu | %7d | %8llu | %8.1lf | %8llu | %10llu |\n",
                    Stats::run, curConflict, Stats::nbDecisions, InfoSystem::elapsed_seconds(), Stats::nbNoGoods, Stats::unit);
            
            rst.Inc();
            curConflict = 0;
            AChandler->cancelUntil(0);
        } else {    // new decision level
            if (vp && vp->toVar->isAssigned())
                vp->toVar->lastPushed = false;

            Variable* v = PickVariable();

            if (v) {
                indDomLocal idl = PickValue(v);
                vp = &(v->getvarPropFromLocalDomInd(idl));

                vp->lastAssigned = Options::assign;
                AChandler->pushNewVariable(Options::assign, idl, v, true, true);
                if (Options::Verbose == verbose::medium)
                    cerr << "(" << AChandler->getDecisionLevel() << ") " << vp->toVar->getName()
                         << (vp->lastAssigned ? "=" : "!=") << vp->toVar->getVarPropFromLocalDomInd(vp->posInVar).val
                         << " ~ "<< vp->toVar->indDomLocalToIndVP(vp->posInVar) << endl;
                continue;
            }

            for (auto var : problem->getVariables())
                if (!var->isAssigned())
                    var->assignAt(0, AChandler->getDecisionLevel());
            return 0;
        }
    }

    return 2;
}   // search