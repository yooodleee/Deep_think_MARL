
#ifndef CONSTRAINTSUM_H_
#define CONSTRAINTSUM_H_

#include <vector>
#include <string>

#include "Constraint.hh"
#include "Variable.hh"



class ConstraintSum : public Constraint {
protected:
    long min;
    long max;
    long limit;

    inline void recomputeBounds()
    {
        min = max = 0;
        for (size_t i = 0; i < listSize; i++) {
            min += scope[i]->getLowerBoundVal();
            max += scope[i]->getUpperBoundVal();
        }
    }


public:
    ConstraintSum(std::string n, std::vector<Variable*> vars, int lim)
        : Constraint(n, vars)
        , min(0)
        , max(0)
        , limit(lim){};

    virtual bool propagate(int level, Variable* cur, std::vector<Variable*>* touched) = 0;
};


class ConstraintSumEQ : public ConstraintSum {
public:
    COnstraintSumEQ(std::string n, std::vector<Variable*> vars, int lim)
        : ConstraintSum(n, vars, lim){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


