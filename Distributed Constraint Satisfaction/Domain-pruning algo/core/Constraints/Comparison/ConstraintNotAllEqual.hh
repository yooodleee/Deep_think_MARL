
#ifndef CONSTRAINTNOTALLEQUAL_H_
#define CONSTRAINTNOTALLEQUAL_H_

#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variable.hh"


class ConstraintNotAllEqual : public Constraint {

public:
    ConstraintNotAllEqual(std::string n, std::vector<Variable*> vars)
        : Constraint(n, vars){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTNOTALLEQUAL_H_