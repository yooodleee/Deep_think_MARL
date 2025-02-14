
#ifndef CONSTRAINTCOUNT_H_
#define CONSTRAINTCOUNT_H_

#include "Constraint.hh"
#include "Variable.hh"

#include <string>
#include <unordered_map>
#include <vector>



class ConstraintExactlyKVariable : public Constraint {
private:
    Variable* var;
    int value;
    int positionOfVarInList;    // -1 if not present


public:
    ConstraintExactlyKVariable(std::string n, std::vector<Variable*> vars, int val)
        : Constraint(n, vars, vars.size() - 1)
        , var(vars.back())
        , value(val)
        , positionOfVarInList(-1)
    {
        for (size_t i = 0; i < listSize; ++i)
            if (vars[i] == vars.back()) {
                positionOfVarInList = i;
                break;
            }
    };

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};