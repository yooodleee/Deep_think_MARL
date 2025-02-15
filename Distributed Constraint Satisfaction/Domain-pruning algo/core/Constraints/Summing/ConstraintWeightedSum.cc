
#include "ConstraintWeightedSum.hh"

using namespace std;


bool ConstraintWeightedSumEQ::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();

    if (limit < min || max < limit)
        return true;

    int positionOfLastChange = -1;
    do {
        for (int i = 0; (size_t)i < listSize; i++) {
            if (i == positionOfLastChange) {
                positionOfLastChange = -1;
                break;
            }

            Variable* var = scope[i];
            int sizeBefore = var->domainCurSize;

            if (sizeBefore == 1)
                continue;

            int c = coefficients[i];
            if (c >= 0) {
                min -= c * var->getLowerBoundVal();
                max -= c * var->getUpperBoundVal();
            } else {
                min -= c * var->getUpperBoundVal();
                max -= c * var->getLowerBoundVal();
            }

            for (int j = var->domainCurSize - 1; j >= 0; j--) {
                int v = var->LocalDomIndToVal(j);
                if (max + v * c < limit || min + v * c > limit)
                    if (deleteValue(var, j, level))
                        return true;
            }

            if (sizeBefore != var->domainCurSize) {
                touched.push_back(var);
                positionOfLastChange = i;
            }

            if (c >= 0) {
                min += c * var->getLowerBoundVal();
                max += c * var->getUpperBoundVal();
            } else {
                min += c * var->getUpperBoundVal();
                max += c * var->getLowerBoundVal();
            }
        }
    } while (positionOfLastChange != -1);

    return false;
}


bool ConstraintWeightedSumGE::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();
    if (min >= limit)
        return false;
    if (max < limit)
        return true;

    for (int i = right; i >= leftmostPositiveCoefficientPosition; i--) {
        Variable* var = scope[i];
        int sizeBefore = var->domainCurSize;
        if (sizeBefore == 1)
            continue;
        
        int c = coefficients[i];
        long maxBase = max - var->getUpperBoundVal() * c;
        int minimumBefore = var->getLowerBoundVal();

        for (int j = var->domainCurSize - 1; j >= 0; j--)
            if (maxBase + var->LocalDomIndToVal(j) * c < limit)
                deleteValue(var, j, level);

        if (sizeBefore != var->domainCurSize)
            touched.push_back(var);

        assert(var->domainCurSize);
        if ((min += (var->getLowerBoundVal() - minimumBefore) * c) >= limit)
            return false;
    }

    for (int i = left; i < leftmostPositiveCoefficientPosition; i++) {
        Variable* var = scope[i];
        int sizeBefore = var->domainCurSize;
        if (sizeBefore == 1)
            continue;

        int c = coefficients[i];
        long maxBase = max - var->getLowerBoundVal() * c;
        int maxBefore = var->getUpperBoundVal();

        for (int j = var->domainCurSize - 1; j >= 0; j--)
            if (maxBase + var->LocalDomIndToVal(j) * c < limit)
                deleteValue(var, j, level);

        if (sizeBefore != var->domainCurSize)
            touched.push_back(var);

        assert(var->domainCurSize);
        if ((min += (var->getUpperBoundVal() - maxBefore) * c) >= limit)
            return false;
    }

    return false;
}


bool ConstraintWeightedSumLE::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    recomputeBounds();
    if (max <= limit)
        return false;
    if (min > limit)
        return true;

    for (int i = right; i >= leftmostPositiveCoefficientPosition; i--) {
        Variable* var = scope[i];

        int sizeBefore = var->domainCurSize;
        if (sizeBefore == 1)
            continue;
        
        int coeff = coefficients[i];
        max -= var->getUpperBoundVal() * coeff;

        int maxBase = min - coeff * var->getLowerBoundVal();
        for (int j = var->domainCurSize - 1; j >= 0; j--)
            if (maxBase + var->LocalDomIndToVal(j) * coeff > limit)
                deleteValue(var, j, level);

        if (sizeBefore != var->domainCurSize)
            touched.push_back(var);

        assert(var->domainCurSize);
        max += var->getUpperBoundVal() * coeff;

        if (max <= limit)
            return false;
    }

    for (int i = left; i < leftmostPositiveCoefficientPosition; i++) {
        Variable* var = scope[i];

        int sizeBefore = var->domainCurSize;
        if (sizeBefore == 1)
            continue;

        int coeff = coefficients[i];
        max -= var->getLowerBoundVal() * coeff;

        int maxBase = min - coeff * var->getUpperBoundVal();
        for (int j = var->domainCurSize - 1; j >= 0; j--)
            if (maxBase + var->LocalDomIndToVal(j) * coeff > limit)
                deleteValue(var, j, level);

        if (sizeBefore != var->domainCurSize)
            touched.push_back(var);

        assert(var->domainCurSize);
        max += var->getLowerBoundVal() * coeff;

        if (max <= limit)
            return false;
    }

    return false;
}


bool ConstraintWeightedSumNE::propagate(int level, Variable* cur, vector<Variable*>& touched)
{
    if (sentinel1 == -1 || scope[sentinel1]->isAssigned()) {
        sentinel1 = findAnotherSentinel();
        if (sentinel1 == -1) {  // No new non singleton variable
            if (sentinel2 == -1 || scope[sentinel2]->isAssigned())
                return currentSum() == limit;

            int sizeBefore = scope[sentinel2]->domainCurSize;
            if (scope[sentinel2]->removeValue((limit - currentSumExcept(sentinel2)) * coefficients[sentinel2], level, this))
                return true;

            if (sizeBefore != scope[sentinel2]->domainCurSize)
                touched.push_back(scope[sentinel2]);
            return false;
        }
    }

    if (sentinel2 == -1 || scope[sentinel2]->isAssigned()) {
        sentinel2 = findAnotherSentinel();

        if (sentinel2 == -1) {
            int sizeBefore = scope[sentinel1]->domainCurSize;
            if (scope[sentinel1]->removeValue((limit - currentSumExcept(sentinel1)) * coefficients[sentinel1], level, this))
                return true;

            if (sizeBefore != scope[sentinel1]->domainCurSize)
                touched.push_back(scope[sentinel1]);
            return false;
        }
    }

    return false;
}