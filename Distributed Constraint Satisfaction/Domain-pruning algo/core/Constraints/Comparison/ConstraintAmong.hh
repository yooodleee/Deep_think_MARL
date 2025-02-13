
#ifndef CONSTRAINTAMONG_H_
#include CONSTRAINTAMONG_H_

#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variable.hh"


class ConstraintAmong : public Constraint {
private:
    std::vector<int> values;
    int k;

    unsigned* sparse;
    unsigned* dense;
    int limit;

    inline bool isPresentInValues(int value) {
        return std::find(values.begin(), values.end(), value) != values.end();
    }

public:
    ConstraintAmong(std::string n, std::vector<Variable*> vars, std::vector<int> vals, int lim)
        : Constraint(n, vars)
        , values(vals)
        , k(lim)
        , limit(-1)
    {
        dense = new unsigned[vars.size()];
        sparse = new unsigned[vars.size()];
        for (size_t i = 0; i < vars.size(); ++i) {
            dense[i] = i;
            sparse[i] = i;
        }
    };

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTAMONG_H_