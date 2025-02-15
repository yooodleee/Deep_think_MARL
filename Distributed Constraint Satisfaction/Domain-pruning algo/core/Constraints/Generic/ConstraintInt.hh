
#ifndef CONSTRAINTINT_H_
#define CONSTRAINTINT_H_

#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variable.hh"
#include "XCSP3Tree.h"



class ConstraintInt : public Constraint {
private:
    XSCP3Core::Tree* t;
    std::vector<std::map<std::string, int>>* vpRes;

    std::map<std::string, int> tuple;
    std::vector<int> cptRet;

    bool computeNewTuple(int var, int value, int nValidTuplesMax);
    bool stillOK(std::map<std::string, int> tuple);


public:
    ConstraintInt(std::string n, std::vector<Variable*> vars, XCSP3Core::Tree* tree)
        : Constraint(n, vars)
        , t(tree)
    {
        sort(scope.begin(), scope.end(), [](const auto& lhs, const auto& rhs) {
            return lhs->domainInitSize < rhs->domainInitSize;
        });

        vpRes = new std::vector<std::map<std::string, int>>[listSize];
        for (size_t i = 0; i < listSize; ++i)
            vpRes[i] = std::vector<std::map<std::string, int>>(scope[i]->domainInitSize);
        
        cptRet.resize(listSize);
    }

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTINT_H_