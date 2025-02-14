
#ifndef CONSTRAINTEXTBINARY_H_
#define CONSTRAINTEXTBINARY_H_

#include "Constraint.hh"
#include "Variable.hh"
#include "XCSP3Constraint.h"



class ConstraintExtBinary : public Constraint {
protected:
    std::vector<int> resx;  // residue for x
    std::vector<int> resy;  // residue for y
    Variable *x, *y;
    std::vector<std::vector<intVp>> tuples;

    std::vector<std::vector<bool>> matrix;
    bool isSupport;


public:
    ConstraintExtBinary(std::string n, std::vector<Variable*> vars, std::vector<std::vector<indVp>>& tupels, bool support)
        : Constraint(n, vars)
        , x(vars[0])
        , y(vars[1])
        , tuples(tupels)
        , isSupport(support)
    {
        int xSz = x->domainCurSize;
        int ySz = y->domainCurSize;

        for (int i = 0; i < xSz; ++i)
            matrix.push_back(std::vector<bool>(ySz, false));

        for (auto t : tupels) {
            if (t[0] == STAR) {
                assert(t[1] != STAR);
                for (int i = 0; i < xSz; ++i)
                    matrix[i][y->indVpToIndVpLocal(t[1])] = true;
            } else if (t[1] == STAR) {
                for (int i = 0; i < ySz; ++i)
                    matrix[x->indVpToIndVpLocal(t[0])][i] = true;
            } else 
                matrix[x->indVpToIndVpLocal(t[0])][y->indVpToVpLocal(t[1])] = true;
        }

        resx.resize(xSz, -1);
        resy.resize(ySz, -1);
    };

    bool init()
    {
        if (isSupport)
            for (size_t j = 0; j < listSize; ++j)
                if (scope[j]->keepOnlyVarProp(tuples, j, 0, this))
                    return true;

        return false;
    };

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTEXTBINARY_H_