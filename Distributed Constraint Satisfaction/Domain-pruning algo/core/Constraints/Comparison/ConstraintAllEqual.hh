
#ifndef CONSTRAINTALLEQUAL_H_
#define CONSTRAINTALLEQUAL_H_

#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variable.hh"


class ConstraintAllEqual : public Constraint {
protected:
    indVpLocal* frontier;

public:
    ConstraintAllEqual(std::string n, std::vector<Variable*> vars)
        : Constraint(n, vars)
    {
        frontier = new int[listSize];
        for (size_t i = 0; i < listSize; ++i)
            frontier[i] = scope[i]->domainCurSize;
    }

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
    void backtrack(int level)
    {
        for (size_t i = 0; i < listSize; ++i)
            frontier[i] = scope[i]->domaincurSize;
    }
};



#endif  // CONSTRAINTALLEQUAL_H_