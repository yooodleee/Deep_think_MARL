
#ifndef CONSTRAINTEXTREMUM_H_
#define CONSTRAINTEXTREMUM_H_

#include <limits>
#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variable.hh"



class ConstraintExtremum : public Constraint {
protected:
    Variable* ext;
    size_t* sentinels;  // sentinels[a] denotes the sentinel for the value at index a in the domain of the extremum variable.

    int findSentinelFor(int v, size_t except)
    {
        for (size_t i = 0; i < listSize; ++i)
            if (i != except && scope[i]->isValidValue(v))
                return i;
            return -1;
    }

    int findSentinelFor(int v)
    {
        for (size_t i = 0; i < listSize; ++i)
            if (scope[i]->isValidValue(v))
                return i;
            return -1;
    }

public:
    ConstraintExtremum(std::string n, std::vector<Variable*> vars, int sz)
        : Constraint(n, vars, sz)
        , ext(vars[sz])
    {
        sentinels = new size_t[ext->domainInitSize];
        for (int i = 0; i < ext->domainInitSize; ++i)
            sentinels[i] = findSentinelFor(ext->getVarPropFromLocalInd(i).val);
    }

    virtual bool propagate(int level, Variable* cur, std::vector<Variable*>& touched) = 0;
};

// ****************************************************************
// ******************** Constraint maximum ************************
// ****************************************************************


class ConstraintMaximum : public ConstraintExtremum {
private:
    int computeLimitForSentinel(size_t sentinel)
    {
        for (int a = ext->getUpperBoundLocalInd(); a >= 0; --a) {
            VP& curVp = ext->getVarPropFromLocalInd(a);
            if (!ext->isValidPosInVar(curVp.posInVar))
                continue;
            if (sentinels[a] != sentinel || findSentinelFor(curVp.val, sentinel) != -1)
                return curVp.val;
        }

        return std::numeric_limits<int>::min();
    }

public:
    ConstraintMaximum(std::string n, std::vector<Variable*> vars, int sz)
        : ConstraintExtremum(n, vars, sz){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


// **************************************************************
// ******************** Constraint minimum **********************
// **************************************************************

class ConstraintMinimum : public ConstraintExtremum {
private:
    int computeLimitForSentinel(size_t sentinel)
    {
        for (int a = ext->getLowerBoundLocalInd(); a < ext->getUpperBoundLocalInd(); ++a) {
            VP& curVp = ext->getVarPropFromLocalInd(a);
            if (!ext->isValidPosInVar(curVp.posInVar))
                continue;
            if (sentinels[a] != sentinel || findSentinelFor(curVp.val, sentinel) != -1)
                return curVp.val;
        }

        return std::numeric_limits<int>::max();
    }

public:
    ConstraintMinimum(std::string n, std::vector<Variable*> vars, int sz)
        : ConstraintExtremum(n, vars, sz){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTEXTREMUM_H_