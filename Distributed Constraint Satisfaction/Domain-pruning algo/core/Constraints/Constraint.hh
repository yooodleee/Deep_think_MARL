
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

    /// Getter for the name given by the original instance
    inline std::string getName() { return name; }

    /// Getter for the unique ID
    inline int getId() { return id; }

    /// Getter for the constraint stamp
    inline unsigned long long getStamp() { return stamp; }

    /// Update constraint stamp with the max of its variables
    inline void updateStamp() {
        for (auto v : scope)
            if (stamp < v->ctrStamp)
                stamp = v->ctrStamp;
    }

    /// Implement if constraint need initialization
    virtual bool init() { return false; }

    /// Must be implemented for backtrackable constraints
    virtual void propagate(int level, Variable* cur, std::vector<Variable*>& touched) = 0;

    /// Must be implemented for backtrackable constraints
    virtual void backtrack(int level){};

    virtual void getConflict(std::vector<unsigned>& cl);

    virtual void getReason(unsigned lit, Expl& litExpl, int level, std::set<std::pair<unsigned, Expl*>, varOrderCmp>& stack, std::vector<unsigned>& cl);
};


#include "Clauses.hh"



class RefClauses: public Constraint {
private:
    std::set<int> uniqueVar;

public:
    static Clauses* dbClauses;  // share the same db
    static double cla_inc;
    static double clause_decay;

    static inline void claDecayActivity() {
        RefClauses::cla_inc *= (1 / RefClauses::clause_decay);
    }

    unsigned ref;
    int isReason = 0;
    double activity = .0;

    void claBumpActivity();

    /// Constructor
    RefClauses(unsigned r)
        : Constraint("ng" + std::to_string(r))
        , ref(r){};

    void getConflict(std::vector<unsigned>& cl) override;

    void getReason(unsigned lit, Expl& litExpl, int level, std::set<std::pair<unsigned, Expl*>, varOrderCmp>& stack, std::vector<unsigned>& cl) override;

    /// Implemenentation of the constraint dedicated propagator
    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched) { assert(false); };
};


class Domain : public Constraint {
public:
    /// Constructor
    Domain()
        : Constraint("dom")
    {
        Stats::nbConstraints--;
    };

    static Domain* domCtr;

    void getConflict(std::vector<unsigned>& cl) override { assert(false); };

    void getReason(unsigned lit, Expl& litExpl, int level, std::set<std::pair<unsigned, Expl*>, varOrderCmp>& stack, std::vector<unsigned>& cl) override;

    /// Implementation of the constraint dedicated propagator
    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched) { assert(false); };
};



#endif  // CONSTRAINT_H_