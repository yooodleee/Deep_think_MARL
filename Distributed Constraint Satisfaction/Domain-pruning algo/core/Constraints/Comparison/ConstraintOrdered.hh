
#ifndef CONSTRAINTORDERED_H_
#define CONSTRAINTORDERED_H_

#include "Constraint.hh"
#include "Variable.hh"


class ConstraintOrdered : public Constraint {
public:
    ConstraintOrdered(std::string n, std::vector<Variable*> vars)
        : Constraint(n, vars){};

    virtual bool propagate(int level, Variable* cur, std::vector<Variable*>& touched) = 0;
};


class ConstraintOrderedLT : public ConstraintOrdered {
public:
    ConstraintOrderedLT(std::string n, std::vector<Variable*> vars)
        : ConstraintOrdered(n, vars){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintOrderedLE : public ConstraintOrdered {
public:
    ConstraintOrderedLE(std::string n, std::vector<Variable*> vars)
        : ConstraintOrdered(n, vars){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintOrderedGE : public ConstraintOrdered {
public:
    ConstraintOrderedGE(std:;string n, std::vector<Variable*> vars)
        : ConstraintOrdered(n, vars){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintOrderedGT : public ConstraintOrdered {
public:
    ConstraintOrderedGT(std::string n, std::vector<Variable*> vars)
        : ConstraintOrdered(n, vars){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


# endif // CONSTRAINTORDERED_H_