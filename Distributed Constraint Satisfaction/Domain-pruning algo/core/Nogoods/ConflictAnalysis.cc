
#include "ConflictAnalysis.hh"
#include "Constraint.hh"
#include "Options.hh"
#include "Variable.hh"

using namespace std;


unsigned long long ConflictAnalysis::gblCaStamp = 1;


void ConflictAnalysis::printHeaderAnalyse(Constraint* ctr)
{
    if ((unsigned)ctr->getId() >= Stats::saveNbConstraint) {
        cerr << "fail from " << ctr->getId() << "(" << ctr->getName() << ") ";
        vector<unsigned>& curCl = RefClause::dbClauses->clauses[static_cast<RefClause*>(ctr)->ref];
        for (size_t i = 0; i < curCl.size(); ++i)
            cerr << ((curCl[i] & 1) ? "¬" : "") << (curCl[i] >> 1) << " ";
    } else {
        cerr << "fail from ctr " << ctr->getId() << "(" << ctr->getName() << ") " << ctr->getUsableScopeSize() << "/" << ctr->getScope().size() << ". scp : ";
        for (auto v : ctr->getScope()) {
            cerr << v->getName() << "(" << v->domainStart << "-" << v->domainStart + v->domainInitSize - 1 << ") ";
        }
    }
    cerr << endl;
}


void ConflictAnalysis::buildStack(vector<unsigned>& conflictCl, int level)
{
    for (auto lit : conflictCl) {
        indVp ivp = lit >> 1;
        Expl& cur = Variable::vpExpl[ivp];

        assert(Variable::varProps[ivp].state != UNDEF);

        if (!cur.level) // level 0 decision, does not belong to explanation
            continue;   // conflictCl isn't ordered by level, so continue instead of breaking
        else if (cur.level == level) {
            if (Options::Verbose >= verbose::high)
                cerr << "is+" << ((lit & 1) ? "¬" : "") << ivp << " ";
            stack.insert(make_pair(lit, &cur)); // put the same sign as the conflictCl because it's handled by getConflict method
        } else {
            assert(!Variable::varProps[ivp].marked);
            Variable::varProps[ivp].marked = true;
            if (Options::Verbose >= verbose::high)
                cerr << "ic+" << ((lit & 1) ? "¬" : "") << ivp << " ";
            cl.push_back(lit);  // put the same sign as the conflictCl because it's handled by getConflict method
        }
    }
}


