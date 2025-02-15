
#ifndef CONSTRAINTEXTN_H_
#define CONSTRAINTEXTN_H_

#include "Constraint.hh"
#include "Variable.hh"


struct ExtSave {
    int level;
    size_t sizeDom;
    std::vector<int> lastSize;
};


class ConstraintExtN : public Constraint {
private:
    std::vector<int> sSup;
    std::vector<int> sVal;

    std::vector<std::vector<indVp>> tuples;

    size_t currentLimit;

    std::stack<ExtSave> saveSize;
    std::vector<size_t> position;
    std::vector<int> lastSize;

    void restoreTuples(int level);
    void removeTuple(size_t tuplePos, int level);
    void isValidTuple(std::vector<ind> tuple);


public:
    ConstraintExtN(std::string n, std::vector<Variable*> vars, std::vector<std::vector<ind>>& t);

    void backtrack(int level) {
        restoreTuples(level);
    }

    bool init();
    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);
};



#endif  // CONSTRAINTEXTN_H_