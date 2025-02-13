
#ifndef CONSTRAINTLEXICOGRAPHIC_H_
#define CONSTRAINTLEXICOGRAPHIC_H_

#include <string>
#include <vector>

#include "Constraint.hh"
#include "Variable.hh"


class ConstraintLexicographic : public Constraint {
private:
    std::vector<int> list1;
    std::vector<int> list2;
    bool strictOrdering;    // If true then <= (le) else < (lt)

    int* times; // times[p] gives the time at which the variable has been set (pseudo-assigned)
    int* vals;  // vals[p] gives the value of the variable (at position p) set at time times[p]
    size_t alpha;
    int time;

    inline void setRes(int p, int v)
    {
        assert(p >= 0 && p < (int)listSize);
        times[p] = time;
        vals[p] = v;
    }

    bool isNotConsistentPair(int x, int v, int y, int w);

    std::vector<Variable*> createScope(std::vector<Variable*> l1, std::vector<Variable*> l2);

    std::vector<int> varsToScope(std::vector<Variable*> list);

public:
    ConstraintLexicographic(std::string n, std::vector<Variable*> l1, std::vector<Variable*> l2, bool isStrict)
        : Constraint(n)
        , strictOrdering(isStrict)  // strictOrdering LT = true LE = false
        , alpha(0)
        , time(0)
    {
        assert(l1.size() == l2.size());

        scope = createScope(l1, l2);
        listSize = scope.size();
        list1 = varsToScope(l1);
        list2 = varsToScope(l2);

        times = new int[listSize];
        vals = new int[listSize];

        for (size_t i = 0; i < listSize; i++)
            times[i] = vals[i] = 0;
    }

    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTLEXICOGRAPHIC_H_