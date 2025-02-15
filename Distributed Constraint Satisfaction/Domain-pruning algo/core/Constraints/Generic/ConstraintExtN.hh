
#ifndef CONSTRAINTEXTN_H_
#define CONSTRAINTEXTN_H_

#include "Constraint.hh"
#include "Variable.hh"


struct ExtSave {
    int level;
    size_t sizeDom;
    std::vector<int> lastSize;
};


