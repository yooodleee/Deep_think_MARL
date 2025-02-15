
#include <cassert>
#include <set>

#include "ConstraintExtNConflict.hh"
#include "Options.hh"

using namespace std;


ConstraintExtNConflict::ConstraintExtNConflict(string n, vector<Variable*> vars, vector<vector<ind>>& t)
    : Constraint(n, vars)
    , tuples(t)
{
    currentLimit = t.size();
    for (size_t i = 0; i < currentLimit; ++i)
        position.push_back(i);
    assert(vars.size());
    assert(vars.size() == t[0].size());
}


