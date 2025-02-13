
#include "ConsistencyHandler.hh"
#include "ConflictAnalysis.hh"
#include "Constraint.hh"
#include "IncNoGoodsManager.hh"
#include "InfoSystem.hh"
#include "Status.hh"
#include "Variable.hh"


#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <string.h>
#include <vector>

using namespace std;


bool ConsistencyHandler::init() {
    cout << "c MACRE 1.0.5" << endl;

    cleanQueue();

    if (Options::Verbose != verbose::no)
        cout << "c Init constraints...";
    for (auto c : problem->getConstraints())
        if (c->init())
            return true;
    if (Options::Verbose != verbose::no)
        cout << "Done" << endl
             << "c Arc consistency is performing...";

    for (auto v : problem->getVariables()) {
        if (v->getConsts().empty() || v->domainInitSize == 1) {
            v->useless = true;
            Status::uselessVars++;
            v->assignAt(0, 0);
        } else 
            addToQueue(v);
    }

    if (propagate())
        return true;

    if (Options::Verbose != verbose::no)
        cout << "Done" << endl;
             << "c Initialization done in " << InfoSystem::timeElapsed() << " seconds" << endl;
    
    if (Options::Verbose != verbose::no)
        Status::print_after_init();

    return false;
}   // init


Variable* ConsistencyHandler::pickVariable() {
    assert(!propagateQueue.empty());

    Variable* vTmp = nullptr;
    ind domSize = std::numeric_limits<int>::max();

    auto toRem = propagateQueue.end();
    for (auto it = propagateQueue.begin(); it != propagateQueue.end(); ++it) {
        int curDom = (*it)->domainCurzie;
        if (curDom < domSize) {
            domSize = curDom;
            vTmp = *it;
            toRem = it;
        }
    }

    assert(vTmp);
    assert(toRem != propagateQueue.end());
    propagateQueue.erase(toRem);
    vTmp->propagateStamp = 0;
    return vTmp;
}


void ConsistencyHandler::pushNewVariable(bool sigTrue, indDomLocal posV, Variable* var, bool count, bool newDec) {
    assert(propagateQueue.empty());

    if (newDec) {
        if (count)
            Stats::nbDecisions++;
        decisionLevel++;
    }

    if (sigTrue)
        var->assignAt(posV, decisionLevel);
    else
        var->removeAt(posV, decisionLevel);

    addToQueue(var);
}   // pushNewVariable


void ConsistencyHandler::cancelUntil(int lev) {
    if (decisionLevel == lev)
        return;
    assert(decisionLevel > lev);

    if (Options::Verbose >= verbose::medium)
        cerr << "Backjump to " << lev << " from " << decisionLevel << endl;

    decisionLevel = lev;
    problem->backtrack(lev);
}   // cancleUntil


bool ConsistencyHandler::propagate(bool incConflict) {
    touched.clear();
    for (; !propagateQueue.empty();) {
        Variable* vTmp = pickVariable();

        if (genHandler && genHandler->propagate(decisionLevel, vTmp, touched) != -1)
            return true;
        for (auto v : touched)
            addToQueue(v);
        touched.clear();

        if (vTmp->isAssigned()) {
            if (ngHandler && ngHandler->propagate(decisionLevel, vTmp, touched) != -1)
                return true;
            for (auto v : touched)
                addToQueue(v);
            touched.clear();

            if (incNgHandler && incNgHandler->revise(vTmp, touched))
                return true;
            for (auto v : touched)
                addToQueue(v);
            touched.clear();
        }

        for (auto c : vTmp->getConsts()) {
            if (c->getStamp() < vTmp->ctrStamp) {
                Stats::nbPropCalls++;

                if (c->propagate(decisionLevel, vTmp, touched)) {
                    if (incConflict)
                        increase(c);

                    return true;
                }

                for (auto v : touched)
                    addToQueue(v);
                touched.clear();

                c->updateStamp();
            }
        }
    }

    return false;
}   // propagate