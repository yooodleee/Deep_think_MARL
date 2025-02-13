
#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_


#include "Options.hh"
#include "Stats.hh"
#include "Variable.hh"


#include <string>
#include <vector>



class Constraint {
protected:
    std::vector<Variable*> scope;
    std::string name;
    int id;
    unsigned long long stamp;
    size_t listSize;

    inline bool fixToValue(Variable* v, int value, int level) { return v->assignTo(value, level, this); }
    inline bool deleteValue(variable* v, indDomLocal pos, int level) { return v->remoteAt(pos, level, this); }

public:
    /// Constructor
    Constraint(std::string n)
        : name(n)
        , stamp(0)
        , listSize(0)
    {
        id = Stats::nbConstraints++;
    };

    
    /// Constructor
    Constraint(std::string n, std::vector<Variable*> vars, int sz = -1)
        : scope(vars)
        , name(n)
        , stamp(0)
    {
        if (sz == -1)
            listSize = vars.size();
        else
            listSize = sz;

        if (Stats::maxArity < vars.size())
            Stats::maxArity = vars.size();
        if (Stats::minArity > vars.size())
            Stats::minArity = vars.size();

        id = Stats::nbConstraints++;
    };


    /// Getter for the scope for constraint
    inline std::vector<Variable*>& getScope() { return scope; }

    inline size_t getUsableScopeSize() { return listSize; }

    
}