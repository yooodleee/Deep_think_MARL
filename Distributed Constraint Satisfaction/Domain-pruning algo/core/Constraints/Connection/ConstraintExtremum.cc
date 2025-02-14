
#include "ConstraintExtremum.hh"

using namespace std;


bool ConstraintMaximum::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    int maxFirst = numeric_limits<int>::min();
    int maxLast = numeric_limits<int>::min();

    for (auto var : scope) {
        if (var->getLowerBoundVal() > maxFirst)
            maxFirst = var->getLowerBoundVal();
        if (var->getUpperBoundVal() > maxLast)
            maxLast = var->getUpperBoundVal();
    }

    // Filtering the domain of the extremum (max) variable.
    int before = ext->domainCurSize;
    if (ext->removeValueAbove(maxLast, level, this) || ext->removeValueUnder(maxFirst, level, this))
        return true;
    if (before != ext->domainCurSize)
        touched.push_back(ext);

    for (int a = ext->getUpperBoundLocalInd(); a >= 0; --a) {
        VP& curVp = ext->getVarPropFromLocalInd(a);
        if (!ext->isValidPosInVar(curVp.posInVar))
            continue;
        if (scope[sentinels[a]]->getIndFromValue(curVp.val) == -1) {
            int s = findSentinelFor(curVp.posInVar);
            if (s != -1)
                sentinels[a] = s;
            else {
                before = ext->domainCurSize;
                if (deleteValue(ext, curVp.posInVar, level))
                    return true;
                if (touched.empty() && before != ext->domainCurSize)
                    touched.push_back(ext);
            }
        }
    }

    // Filtering the domains of variables in the vector
    int lastMax = ext->getUpperBoundVal();
    for (auto var : scope) {
        before = var->domainCurSize;
        if (var->removeValuesAbove(lastMax, level, this))
            return true;
        if (before != var->domainCurSize)
            touched.push_back(var);
    }

    // Possiblty filtering the domain of the sentinel from the last value of the Max variable
    int sentinel = sentinels[ext->getUpperBoundLocalInd()];
    int valLimit = computeLimitForSentinel(sentinel);
    if (valLimit == lastMax)
        return false;   // because another sentinel exists

    Variable* domSentinel = scope[sentinel];
    for (int a = domSentinel->getUpperBoundLocalInd(); a >= 0; --a) {
        VP& curVp = domSentinel->getVarPropFromLocalInd(a);
        if (!domSentinel->isValidPosInVar(curVp.posInVar) || curVp.val == lastMax)
            continue;
        if (curVp.val <= valLimit)
            break;
        if (ext->getIndFromValue(curVp.val) == -1) {
            before = domSentinel->domainCurSize;
            if (deleteValue(domSentinel, curVp.posInVar, level))
                return true;
            if (before != domSentinel->domainCurSize)
                touched.push_back(domSentinel);
        }
    }

    return false;
}


bool ConstraintMinimum::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    int minFirst = numeric_limits<int>::max();
    int minLast = numeric_limits<int>::max();

    for (auto var : scope) {
        if (var->getLowerBoundVal() < minFirst)
            minFirst = var->getLowerBoundVal();
        if (var->getUpperBoundVal() < minLast)
            minLast = var->getUpperBoundVal();
    }

    // Filtering the domain of the extremum (min) variable
    int before = ext->domainCurSize;
    if (ext->removeValueAbove(minLast, level, this) || ext->removeValuesUnder(minFirst, level, this))
        return true;
    if (before != ext->domainCurSize)
        touched.push_back(ext);

    for (int a = ext->getLowerBoundInd() - ext->domainCurSize; a < ext->domainInitSize; ++a) {
        VP& curVp = ext->getVarPropFromLocalInd(a);
        if (!ext->isValidPosInVar(curVp.posInVar))
            continue;
        if (scope[sentinels[a]]->getIndFromValue(curVp.val) == -1) {
            int s = findSentinelFor(curVp.posInvar);
            if (s != -1)
                sentinels[a] = s;
            else {
                before = ext->domainCurSize;
                if (deleteValue(ext, curVp.posInVar, level))
                    return true;
                if (touched.empty() && before != ext->domainCurSize)
                    touched.push_back(ext);
            }
        }
    }

    // Filtering the domains of variables in the vector.
    int firstMin = get->getLowerBoundVal();
    for (auto var : scope) {
        before = var->domainCurSize;
        if (var->removeValuesUnder(firstMin, level, this))
            return true;
        if (before != var->domainCurSize)
            touched.push_back(var);
    }

    // Possibly filtering the domain of the sentinel from the last value of the Max variable.
    int sentinel = sentinels->[get->getLowerBoundInd() - ext->domainStart];
    int valLimit = computeLimitForSentinel(sentinel);
    if (valLimit == firstMin)
        return false;   // because another sentinel exists
    
    Variable* domSentinel = scope[sentinel];
    for (int a = ext->getLowerBoundInd() - ext->domainStart + 1; a < ext->domainInitSize; ++a) {
        VP& curVp = domSentinel->getVarPropFromLocalInd(a);
        if (!domSentinel->isValidPossInVar(curVp.posInVar))
            continue;
        if (curVp.val <= valLimit)
            break;
        if (ext->getIndFromValue(curVp.val) == -1) {
            before = domSentinel->domainCurSize;
            if (deleteValue(domSentinel, curVp.posInVar, level))
                return true;
            if (before != domSentinel->domainCurSize)
                touched.push_back(domSentinel);
        }
    }

    return false;
}