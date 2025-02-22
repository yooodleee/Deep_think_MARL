
#ifndef PROBLEM_H_
#define PROBLEM_H_


#include "Constraint.hh"
#include "Options.hh"
#include "Stats.hh"
#include "Variable.hh"


#include <vector>


/**
 * Problem
 */
class Problem {
private:
    std::vector<Variable*> variables;
    std::vector<Constraint*> constraints;
    std::vector<Constraint*> backtrackableConstraints;


public:
    /// Getter for the prob variables
    inline std::vector<Variable*>& getVariables() { return variables; }

    /// Add a variable to the prob
    inline void addVariable(Variable* var) { variables.push_back(var); }

    /// Getter for the prob constraints
    inline std::vector<Constraint*>& getConstraints() { return constraints; }

    /// Setter for the backtrackable constraints of the prob
    inline void setBTConstraints(std::vector<Constraint*> vecCont) { backtrackableConstraints = vecCont; }

    /// Setter for the constraints of the prob and register them in their scope
    inline void setConstraints(std::vector<Constraint*> vecCont)
    {
        constraints = vecCont;
        for (auto c : constraints)
            for (auto v : c->getScope())
                v->addConsts(c);
        Stats::saveNbConstraints = Stats::nbConstraints;
    }

    inline void init(std::vector<Constraint*>& vecCont, std::vector<Constraint*>& vecBTCont)
    {
        for (auto v : variables)
            v->init();
        Stats::nbVarProps = Stats::saveNbVarProps = Variable::varProps.size();

        setConstraints(vecCont);
        setBTConstraints(vecBTCont);
    }


    /// Perform backtrack to @level on all variables and backtrackable constraints
    void backtrack(int level)
    {
        for (size_t i = 0, stop = Variable::vpExpl.size(); i < stop; ++i)
            if (Variable::vpExpl[i].level > level)
                Variable::vpExpl[i].unlock();
        
        for (auto v : variables)
            v->backtrackToLevel(level);
        for (auto c : backtrackableConstraints)
            c->backtrack(level);
    }
};



#endif  // PROBLEM_H_