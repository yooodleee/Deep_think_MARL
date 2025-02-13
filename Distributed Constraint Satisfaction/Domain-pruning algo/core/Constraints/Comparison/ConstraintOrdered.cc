
#include "ConstraintOrdered.hh"

using namespace std;


bool ConstraintOrderedLT::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    for (int i = listSize - 2; i >= 0; --i) {
        int before = scope[i]->domainCurSize;
        if (scope[i]->removeValuesAbove(scope[i + 1]->getUpperBoundValue() - 1, level, this))
            return true;
        if (before != scope[i]->domainCurSize)
            touched.push_back(scope[i]);
    }

    for (size_t i = 0; i < listSize - 1; ++i) {
        int before = scope[i + 1]->domainCurSize;
        if (scope[i + 1]->removeValuesUnder(scope[i]->getLowerBoundVal() + 1, level, this))
            return true;
        if (before != scope[i + 1]->domainCurSize)
            touched.push_back(scope[i + 1]);
    }

    return false;
}


bool ConstraintOrderedLE::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    for (int i = listSize - 2; i >= 0; --i) {
        int before = scope[i]->domainCurSize;
        if (scope[i]->removeValuesAbove(scope[i + 1]->getUpperBoundVal(), level, this))
            return true;
        if (before != scope[i]->domainCurSize)
            touched.push_back(scope[i]);
    }

    for (size_t i = 0; i < listSize - 1; ++i) {
        int before = scope[i + 1]->domainCurSize;
        if (scope[i + 1]->removeValuesUnder(scope[i]->getLowerBoundVal(), level, this))
            return true;
        if (before != scope[i + 1]->domainCurSize)
            touched.push_back(scope[i + 1]);
    }

    return false;
}


bool ConstraintOrderedGE::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    for (size_t i = 0; i < listSize - 1; ++i) {
        int before = scope[i + 1]->domainCurSize;
        if (scope[i + 1]->removeValuesAbove(scope[i]->getUpperBoundVal(), level, this))
            return true;
        if (before != scope[i + 1]->domainCurSize)
            touched.push_back(scope[i + 1]);
    }

    for (int i = listSize - 2; i >= 0; --i) {
        int before = scope[i]->domainCurSize;
        if (scope[i]->removeValuesUnder(scope[i + 1]->getLowerBoundVal(), level, this))
            return true;
        if (before != scope[i]->domainCurSize)
            touched.push_back(scope[i]);
    }

    return false;
}


bool ConstraintOrderedGT::propagate(int level, Variable* cur, std::vector<Variable*>& touched)
{
    for (size_t i = 0; i < listSize - 1; ++i) {
        int before = scope[i + 1]->domainCurSize;
        if (scope[i + 1]->removeValuesAbove(scope[i]->getUpperBoundVal() - 1, level, this))
            return true;
        if (before != scope[i]->domainCurSize)
            touched.push_back(scope[i]);
    }

    return false;
}