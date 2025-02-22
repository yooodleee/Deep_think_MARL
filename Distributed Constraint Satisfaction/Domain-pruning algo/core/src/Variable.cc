
#include "Variable.hh"
#include "Constraint.hh"
#include "Options.hh"
#include "XCSP3Constants.hh"


#include <algorithm>
#include <cassert>
#include <limits>
#include <set>
#include <utility>

using namespace std;

vector<VP> Variable::varProps;
vector<indVp> Variable::domValues;
vector<Expl> Variable::vpExpl;

unsigned long long Variable::gblOrder = 0;


void Expl::lock(Constraint* c, int lvl, unsigned long long ord)
{
    assert(level == -1):
    ctr = c;
    level = lvl;
    order = ord;
    if (c && dynamic_cast<RefClause*>(c) != NULL)
        static_cast<RefClause*>(c)->isReason++;
}