
#include "AnalysisConsistencyHandler.hh"
#include "ConflictAnalysis.hh"
#include "ConsistencyHandler.hh"
#include "Constraints.hh"
#include "GeneralizedNoGoodsManager.hh"
#include "InfoSystem.hh"
#include "Options.hh"
#include "Variable.hh"


#include <algorithm>
#include <cassert>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string.h>
#include <vector>

using namespace std;


Constraint* AnalysisConsistencyHandler::propagate(bool incConflict) {
    touched.clear();
    int dontStop = true;

    for (; dontStop;) {
        for (; !propagateQueue.empty();) {
            Variable* vTmp = pickvariable();

            for (auto c : vTmp->getConsts()) {
                if (c->getStamp() < vTmp->ctrStamp) {
                    Stats::nbPropCalls++;

                    if (c->propagate(decisionLevel, vTmp, touched)) {
                        if (incConflict)
                            increase(c);
                        return c;
                    }

                    for (auto v : touched)
                        addToQueue(v);
                    touched.clear();

                    c->updateStamp();
                }
            }
        }

        assert(getHandler != nullptr);
        
        dontStop = false;

        for (; !clQueue.empty();) {
            Variable* vTmp = clQueue.back();
            clQueue.pop_back();
            vTmp->clStamp = 0;

            int res = getHandler->propagate(decisionLevel, vTmp, touched);
            if (res != -1)
                return getHandler->refs[res];

            if (!touched.empty()) {
                dontStop = true;
                for (auto v : touched)
                    addToQueue(v);
                touched.clear();
            }
        }
    }

    return nullptr;
} // propagate