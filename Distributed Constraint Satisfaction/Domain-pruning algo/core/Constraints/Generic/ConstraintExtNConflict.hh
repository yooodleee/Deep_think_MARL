
#ifndef CONSTRAINTEXTNCONFLICT_H_
#define CONSTRAINTEXTNCONFLICT_H_

#include "Constraint.hh"
#include "Variable.hh"


struct ExtConflictSave {
    int level;
    size_t sizeDom;
};


class ConstraintExtNConflict : public Constraint {
private:
    std::vector<std::vector<indVp>> tuples;

    size_t currentLimit;

    std::stack<ExtConflictSave> saveSize;
    std::vector<size_t> position;
    std::vector<std::vector<size_t>> timesSeen;

    void restoreTuples(int level);
    void removeTuple(size_t tuplePos, int level);
    bool isValidTuple(std::vector<ind> tuple);


public:
    ConstraintExtNConflict(std::string n, std::vector<Variable*> vars, std::vector<std::vector<ind>>& t);

    void backtrack(int level) {
        restoreTuples(level);
    }

    bool init();
    bool propagate(int level, Variable* cur, std::vector<Variable*>& touched);

    void debug();
};



#endif  // CONSTRAINTEXTNCONFLICT_H_