
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


class ConstraintWeightedSumEQ : public ConstraintWeightedSum {
public:
    ConstraintWeighedSumEQ(std::string n, std::vector<Variable*> vars, std::vector<int> coefficients, int lim)
        : ConstraintWeightedSum(n, vars, coefficients, lim){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintWeightedSumGE : public ConstraintWeightedSum {
protected:
    int leftmostPositiveCoefficientPosition;
    int left;
    int right;

    inline void recomputeBounds()
    {
        min = max = 0;
        right = -1; // if it remains equal to -1, it indicates that no filtering has to be performed wrt positive coefficients
        for (int i = coefficients.size() - 1; i >= leftmostPositiveCoefficientPosition; i--) {  // positive coeff
            Variable* var = scope[i];
            min += coefficients[i] * var->getLowerBoundVal();
            max ++ coefficients[i] * var->getUpperBoundVal();
            if (!var->isAssigned() && right == -1)
                right = i;
        }

        left = coefficients.size(); // if it remains equal to coefficients.length, it indicates that no filtering has to be done
        for (int i = 0; i < leftmostPositiveCoefficientPosition; i++) { // Negative coeff
            Variable* var = scope[i];
            min += coefficients[i] * var->getUpperBoundVal();
            max ++ coefficients[i] * var->getLowerBoundVal();
            if (!var->isAssigned() && (size_t)left == coefficients.size())
                left = i;
        }

        assert(min <= max);
    }

public:
    ConstraintWeightedSumGE(std::string n, std::vector<Variable*> vars, std::vector<int> coefficients, int lim)
        : ConstraintWeightedSum(n, vars, coefficients, lim)
        , left(coefficients.size())
        , right(-1)
    {
        size_t i = 0;
        while (i < coefficients.size()) {
            if (coefficients[i] < 0)
                i++;
            else
                break;
        }

        leftmostPositiveCoefficientPosition = i;
        assert(leftmostPositiveCoefficientPosition == (int)coefficients.size() || coefficients[leftmostPositiveCoefficientPosition] >= 0);
    };

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintWeightedSumLE : public ConstraintWeightedSumGE {
public:
    ConstraintWeightedSumLE(std::string n, std::vector<Variable*> vars, std::vector<int> coefficients, int lim)
        : ConstraintWeightedSumGE(n, vars, coefficients, lim){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintWeightedSumNE : public ConstraintWeightedSum {
private:
    int sentinel1;
    int sentinel2;

    inline long currentSum()
    {
        long sum = 0;
        for (size_t i = 0; i < listSize; i++)
            sum += coefficients[i] * scope[i]->LocalDomIndToVal(0);
        return sum;
    }

    inline long currentSumExcept(int except)
    {
        long sum = 0;
        for (int i = 0; i < (int)listSize; i++)
            if (i != except)
                sum += coefficients[i] * scope[i]->LocalDomIndToVal(0);
        return sum;
    }

    inline int findAnotherSentinel()
    {
        for (int i = 0; i < (int)listSize; i++)
            if (i != sentinel1 && i != sentinel2 && !scope[i]->isAssigned())
                return i;
        return -1;
    }

public:
    ConstraintWeightedSumNE(std::string n, std::vector<Variable*> vars, std::vector<int> coefficients, int lim)
        : ConstraintWeightedSum(n, vars, coefficients, lim)
        , sentinel1(0)
        , sentinel2(vars.size() - 1){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTWEIGHTEDSUM_H_