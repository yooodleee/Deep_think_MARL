
#ifndef CONSTRAINTWEIGHTEDSUM_H_
#define CONSTRAINTWEIGHTEDSUM_H_

#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variable.hh"


class ConstraintWeightedSum : public Constraint {
protected:
    long min;
    long max;
    long limit;
    std::vector<int> coefficients;

    inline void recomputeBounds()
    {
        min = max = 0;
        for (size_t i = 0; i < listSize; i++) {
            int c= coefficients[i];
            if (c >= 0) {
                min += c * scope[i]->getLowerBoundVal();
                max += c * scope[i]->getUpperBoundVal();
            } else {
                min += c * scope[i]->getUpperBoundVal();
                max += c * scope[i]->getLowerBoundVal();
            }
        }

        assert(min <= max);
    }

public:
    ConstraintWeightedSum(std::string n, std::vector<Variable*> vars, std::vector<int> coeff, int lim)
        : Constraint(n, vars)
        , min(0)
        , max(0)
        , limit(lim)
        , coefficients(coeff){};

    virtual bool propagate(int level, Variable* cur, std::vector<Variable*>& touched) = 0;
};


