
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


class ConstraintCount : public Constraint {
protected:
    int value;  // the specified (target) value
    int k;  // the specified number of occurrences used as limit


public:
    ConstraintCount(std::string n, std::vector<Variable*> vars, int v, int occ)
        : Constraint(n, vars)
        , value(v)
        , k(occ){};

    virtual bool propagate(int level, Variable* cur, std::vector<Variable*>& touched) = 0;
};


class ConstraintAtLeastK : public ConstraintCount {
protected:
    unsigned* sparse;   // Used for stroing (k+1) sentinals; stored variable positions in scope
    unsigned* dense;
    int limit;

    std::unordered_map<Variable*, int> varToScopePosition; // map between values and varProp indices


public:
    ConstraintAtLeastK(std::string n, std::vector<Variable*> vars, int v, int occ)
        : ConstraintCount(n, vars, v, occ)
    {
        dense = new unsigned[vars.size()];
        sparse = new unsigned[vars.size()];
        for (size_t i = 0; i < vars.size(); ++i) {
            dense[i] = i;
            varToScopePosition[scope[i]] = i;
            sparse[i] = i;
        }

        limit = -1;

        for (int e = 0; e <= k; ++e) {
            int i = sparse[e];
            if (i <= limit)
                continue;
            limit++;
            if (i > limit) {
                int f = dense[limit];
                dense[i] = f;
                dense[limit] = e;
                sparse[e] = limit;
                sparse[f] = i;
            }
        }
    }

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintAtLeast1 : public ConstraintCount {
private:
    // Two sentinels for tracking the presence of the value.
    Variable* sentinel1;
    Variable* sentinel2;

    inline Variable* findAnotherSentinel()
    {
        for (size_t i = 0; i < listSize; i++)
            if (scope[i] != sentinel1 && scope[i] != sentinel2 && scope[i]->isValidValue(value))
                return scope[i];
        
        return nullptr;
    }

public:
    ConstraintAtLeast1(std::string n, std::vector<Variable*> vars, int v)
        : ConstraintCount(n, vars, v, 1)
        , sentinel1(vars[0])
        , sentinel2(vars[1]){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintAtMostK : public ConstraintCount {
public:
    ConstraintAtMostK(std::string n, std::vector<Variable*> vars, int v, int occ)
        : ConstraintCount(n, vars, v, occ){};
    
    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintAtMost1 : public ConstraintAtMostK {
public:
    ConstraintAtMost1(std::string n, std::vector<Variable*> vars, int v)
        : ConstraintAtMostK(n, vars, v, 1){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintExactlyK : public Constraintcount {
public:
    ConstraintExactlyK(std::string n, std::vector<Variable*> vars, int v, int occ)
        : ConstraintCount(n, vars, v, occ){};

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};


class ConstraintExactly1 : public ConstraintExactlyK {
public:
    ConstraintExactly1(std::string n, std::vector<Variable*> vars, int v)
        : ConstraintExactlyK(n, vars, v, 1){};
};



#endif  // CONSTRAINTCOUNT_H_