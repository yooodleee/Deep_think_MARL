
#include "ConstraintSumScalarBoolean.hh"

using namespace std;


bool SumScalarBooleanLE::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();

    if (max <= limit)
        return false;
    if (min > limit)
        return true;

    // this is the only case where can filter
    if (min == limit)
        filter(level, touched, 1);

    return false;
}


