
#ifndef CONSTRAINTPRIMITIVE_H_
#define CONSTRAINTPRIMITIVE_H_

#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variablehh"


class ConstraintPrimitive : public Constraint {
protected:
    Variable* x;
    Variable* y;
    int k;

public:
    ConstraintPrimitive(std::string n, Variable* varX, Variable* varY, int intk)
        : Constraint(n, std::vector<Variable*>({ varX, varY }))
        , x(varX)
        , y(varY)
        , k(intk){};

    virtual bool propagate(int level, Variable* cur, std::vector<Variable*>& touched) = 0;
};


class ConstraintPrimitiveEqual : public ConstraintPrimitive {
public:
    ConstraintPrimitiveEqual(std::string n, Variable* varX, Variable* varY, int intk)
        : ConstraintPrimitive(n, varX, varY, intk){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintPrimitiveLeeThan : public ConstraintPrimitive {
public:
    ConstraintPrimitiveLeeThan(std::string n, Variable* varX, Variable* varY, int intk)
        : ConstraintPrimitive(n, varX, varY, intk){};
    
    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintPrimitiveLessEqual : public ConstraintPrimitive {
public:
    ConstraintPrimitiveLessEqual(std::string n, Variable* varX, Variable* varY, int intk)
        : ConstraintPrimitive(n, varX, varY, intk){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintPrimitiveNotEqual : public ConstraintPrimitive {
public:
    ConstraintPrimitiveNotEqual(std::string n, Variable* varX, Variable* varY, int intk)
        : ConstraintPrimitive(n, varX, varY, intk){};
    
    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintDisjunctive : public ConstraintPrimitive {
    int key;

    bool fileterDomain(Variable* cur, int level, int lbValue, int ubValue);

public:
    ConstraintDisjunctive(std::string n, Variable* varX, Variable* varY, int kx, int intky)
        : ConstraintPrimitive(n, varX, varY, kx)
        , ky(intky){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTPRIMITIVE_H_