
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


class SumScalarBooleanLE : public ConstraintSumScalarBoolean {
private:
    int limit;

    inline void recomputeBounds()
    {
        min = max = 0;
        limitSet = -1;

        for (int i = 0; i < half; i++) {
            Variable* v1 = scope[i];
            Variable* v2 = scope[i + half];

            if (v1->getUpperBoundVal() && v2->getUpperBoundVal()) { // if one 1 is missing to do because the product is necessarly 0
                max++;
                if (v1->getLowerBoundVal() && v2->getLowerBoundVal())   // if v1 and v2 assigned to 1
                    min++;
                else if (v1->isAssigned() || v2->isAssigned())
                    set01vs1[++limitSet] = i;   // add i if (0, 1) versus 1 (or equivalently 1 versus (0, 1)) ; the only way ro filter here
            }
        }
    }


public:
    SumScalarBooleanLE(std::string n, std::vector<Variable*> vars, int lim)
        : ConstraintSumScalarBoolean(n, vars)
        , limit(lim){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class SumScalarBooleanEQ : public ConstraintSumScalarBoolean {
private:
    int limit;

    int* set01vs01;
    int limitSetEq;

    inline void recomputeBounds()
    {
        min = max = 0;
        limitSet = limitSetEq = -1;

        for (int i = 0; i < half; i++) {
            Variable* v1 = scope[i];
            Variable* v2 = scope[i + half];

            if (v1->getUpperBoundVal() && v2->getUpperBoundVal()) { // if one 1 is missing nothing to do because the product is necessarly 0
                max++;
                if (v1->getLowerBoundVal() && v2->getLowerBoundVal())
                    min++;
                else if (v1->isAssigned() || v2->isAssigned())
                    set01vs1[++limitSet] = i;   // add i iff have (0, 1) versus 1 (or equivalently 1 versus (0, 1)); the only way to filter here

                else
                    set01vs01[++limitSetEq] = i;    //add i because have (0, 1) versus (0, 1)
            }
        }
    }


public:
    SumScalarBooleanEQ(std::string n, std::vector<Variable*> vars, int lim)
        : ConstraintSumScalarBoolean(n, vars)
        , limit(lim)
    {
        set01vs01 = new int[half];
        for (int i = 0; i < half; ++i)
            set01vs01[i] = i;
        limitSetEq = -1;
    };

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class SumScalarBooleanLEVar : public ConstraintSumScalarBoolean {
private:
    Variable* limit;

    inline void recomputeBounds()
    {
        min = max = 0;
        limitSet = -1;

        for (int i = 0; i < half; i++) {
            Variable* v1 = scope[i];
            Variable* v2 = scope[i + half];

            if (v1->getUpperBoundVal() && v2->getUpperBoundVal()) { // if one 1 is missing nothing to do because the product is necessarly 0
                max++;
                if (v1->getLowerBoundVal() && v2->getLowerBoundVal())
                    min++;
                else if (v1->isAssigned() || v2->isAssigned())
                    set01vs1[++limitSet] = i;   // add i iff have (0, 1) versus 1 (or equivalently 1 versus (0,1)); the only way to filter here
            }
        }
    }

public:
    SumScalarBooleanLEVar(std::string n, std::vector<Variable*> vars, Variable* lim)
        : ConstraintSumScalarBoolean(n, vars)
        , limit(lim){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTSUMSCALARBOOLEAN_H_