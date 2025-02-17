
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


