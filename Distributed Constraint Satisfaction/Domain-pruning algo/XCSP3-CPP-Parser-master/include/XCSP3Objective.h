
#ifndef XOBJECTIVE_H_
#define XOBJECTIVE_H_


#include <vector>
#include <iostream>


#include "XCSP3Constants.h"


namespace XCSP3Core {

    class XObjective {
    public:
        ObjectiveGoal goal;
        ExpressionObjective type;
        vector<XVariable*> list;
        vector<int> coeffs;
        string expression;
    };
}



#endif  // XOBJECTIVE_H_