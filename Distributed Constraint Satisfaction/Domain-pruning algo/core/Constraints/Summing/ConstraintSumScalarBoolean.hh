
#ifndef CONSTRAINTSUMSCALARBOOLEAN_H_
#define CONSTRAINTSUMSCALARBOOLEAN_H_

#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variable.hh"



class ConstraintSumScalarBoolean : public Constraint {
protected:
    long min;
    long max;
    int half;

    int* set01vs1;
    int limitSet;

    inline void filter(int level, std::vector<Variable*>& touched, int valToRm)
    {
        for (int i = limitSet; i >= 0; i--) {
            int j = set01vs1[i];
            assert((scope[j]->domainCurSize == 2 && scope[half + j]->isAssignedTo(1)) || (scope[half + j]->domainCurSize == 2 && scope[j]->isAssignedTo(1)));

            if (scope[j]->domainCurSize == 2) {
                scope[j]->removeValue(valToRm, level, this);
                touched.push_back(scope[j]);
            } else {
                scope[half + j]->removeValue(valToRm, level, this);
                touched.push_back(scope[half + j]);
            }
        }
    }


public:
    ConstraintSumScalarBoolean(std::string n, std::vector<Variable*> vars)  // Coeffs are at the end of scope
        : Constraint(n, vars)
        , min(0)
        , max(0)
        , half(vars.size() / 2)
    {
        assert(vars.size() % 2 == 0):
        set01vs1 = new int[half];
        for (int i = 0; i < half; ++i)
            set01vs1[i] = i;
        limitSet = -1;
    };

    virtual bool propagate(int level, Variable* cur, std::vector<Variable*>& touched) = 0;
};


