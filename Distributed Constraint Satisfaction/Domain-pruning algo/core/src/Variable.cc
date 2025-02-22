
#include "Variable.hh"
#include "Constraint.hh"
#include "Options.hh"
#include "XCSP3Constants.hh"


#include <algorithm>
#include <cassert>
#include <limits>
#include <set>
#include <utility>

using namespace std;

vector<VP> Variable::varProps;
vector<indVp> Variable::domValues;
vector<Expl> Variable::vpExpl;

unsigned long long Variable::gblOrder = 0;


void Expl::lock(Constraint* c, int lvl, unsigned long long ord)
{
    assert(level == -1):
    ctr = c;
    level = lvl;
    order = ord;
    if (c && dynamic_cast<RefClause*>(c) != NULL)
        static_cast<RefClause*>(c)->isReason++;
}


void Expl::unlock()
{
    assert(level != -1);
    if (ctr && dynamic_cast<RefClause*>(ctr) != NULL)
        static_cast<RefClause*>(ctr)->isReason--;
    
    ctr = nullptr;
    level = -1;
    order = 0;
}


Variable::Variable(string n)
    : name(n)
{
    id = Stats::nbVars++;
}


Variable::Variable(string n, int lb, int ub)
    : Variable(n)
{
    assert(lb < ub);

    domainCurSize = domainInitSize = ub - lb + 1;
    domainStart = domValues.size();
    lowerBound = varProps.size();

    domainBoolUtil = new bool[domainInitSize]();

    for (int i = lb; i <= ub; ++i) {
        Variable::domValues.push_back(varProps.size());
        valueToVarPropInd[i] = varProps.size();

        if ((int)varProps.size() == numeric_limits<int>::max())
            throw runtime_error("Too much values");
        
        Variable::varProps.push_back(VP(i, i - lb, this, 0));
        Variable::vpExpl.push_back(Expl());
    }

    upperBound = varProps.size() - 1;
}   // Variable


Variable::Variable(string n, int onlyVal)
    : Variable(n)
{
    domainCurSize = domainInitSize = 1;
    domainStart = domValues.size();

    domainBoolUtil = new bool[domainInitSize]();

    lowerBound = upperBound = varProps.size();

    if ((int)varProps.size() == numeric_limits<int>::max())
        throw runtime_error("Too much values");
    
    Variable::domValues.push_back(varProps.size());
    Variable.vpExpl.push_back(Expl());
}   // Variable


Variable::Variable(string n, vector<int> v)
    : Variable(n)
{
    assert(!v.empty());

    domainCurSize = domainInitSize = v.size();
    domainStart = domValues.size();
    lowerBound = varProps.size();

    domainBoolUtil = new bool[domainInitSize]();

    sort(v.begin(), v.end());

    int i = 0;
    for (auto val : v) {
        Variable::domValues.push_back(varProps.size());
        valueToVarPropInd[val] = varProps.size();

        if ((int)varProps.size() == numeric_limits<int>::max())
            throw runtime_error("Too much values");

        Variable::varProps.push_back(VP(val, i++, this, 0));
        Variable::vpExpl.push_back(Expl());
    }

    upperBound = varProps.size() - 1;
}   // Variable


ostream& operator<<(ostream& out, const Variable& var)
{
    out << "Variable " << var.id << " informations ______________" << endl;
    out << "Name : " << var.name << endl;
    out << "Nb ctr involved : " << var.consts.size() << endl;

    if (var.domainCurSize == 1)
        out << "Assigned to " << Variable::varProps[Variable::domValues[var.domainStart]].val << endl;
    
    out << "Bounds => ind : " << var.lowerBound << " ~ " << var.upperBound
        << " - indVpLocal : " << var.lowerBound - var.domainStart << " ~ " << var.upperBound - var.domainStart
        << " - values : " << Variable::varProps[var.lowerBound].val << " ~ " << Variable::varProps[var.upperBound].val << endl;
    
    out << "Cur Domain info => ind : " << var.domainStart << " ~ " << var.domainStart + var.domainInitSize - 1
        << " sz=" << var.domainCurSize << " | ";
    
    if (var.saveSize.empty()) {
        for (int i = 0; i < var.domainInitSize; ++i)
            out << Variable::varProps[Variable::domValues[var.domainStart + i]].val << " ";
    } else {
        int i = 0;
        for (; i < var.saveSize.top().sizeDom; ++i)
            out << Variable::varProps[Variable::domValues[var.domainStart + i]].val << " ";
        out << "(";
        
        for (; i < var.domainInitSize; ++i)
            out << Variable::varProps[Variable::domValues[var.domainStart + i]].val << " ";
        out << ")";
    }

    out << endl
        << "Init Domain info : sz=" << var.domainInitSize << " | ";
    
    for (int i = 0; i < var.domainInitSize; ++i)
        out << Variable::varProps[var.domainStart + i].val << " ";
    
    if (!var.saveSize.empty()) {
        std::stack<VarSave> printableStack(var.saveSize);
        out << endl
            << "{";
        for (l !printableStack.empty(); printableStack.pop()) {
            out << "[ L:" << printableStack.top().level << ", S:" << printableStack.top().sizeDom << "] ";
        }
        out << "}";
    }
    out << endl
        << "____________________________________________________" << endl;
    
    return out;
}   // operator<<


bool Variable::assignAt(indDomLocal index, int level, Constraint* ctr)
{
    Stats::assignCall++;

    if (index >= domainCurSize)
        return false;
    
    if (saveSize.empty() || saveSize.top().level != level)
        saveSize.push({ level, domainCurSize, domainCurSize });
    
    indVp ivpToAssign = domValuesToVarProp(domainStart + index);

    if (index) {
        setVarPropIndInDomValues(domainStart + index, domValuesToVarProp(domainStart));
        setVarPropIndDomValues(domainStart, ivpToAssign);

        setPosOfVarPropInVar(domainStart + index, index);
        setPosOfVarPropInVar(domainStart, 0);
    }

    if (Options::Verbose >= verbose::high) {
        if (ctr == nullptr)
            cerr << "(" << level << ")";
        
        else if ((unsigned)ctr->getId() >= Stats::saveNbConstraints)
            cerr << ctr->getName();
        
        else
            cerr << "ctr" << ctr->getId();
        
        cerr << " " << domValuesToVarProp(domainStart) << " => ";
    }

    for (int i = 1; i < domainCurSize; i++) {
        Variable::varProps[domValuesToVarProp(domainStart + i)].state = NEG;

        Variable::vpExpl[domValuesToVarProp(domainStart + i)].lock(ctr, level, ++gblOrder);

        if (Options::Verbose >= verbose::high)
            cerr << "¬" << domValuesToVarProp(domainStart + i) << " ";
    }

    if (Options::Verbose >= verbose::high)
        cerr << endl;
    
    assert(useless ? true : Variable::vpExpl[domValuesToVarProp(domainStart)].level == -1);
    if (!useless && Variable::vpExpl[domValuesToVarProp(domainStart)].level == -1)
        Variable::vpExpl[domValuesToVarProp(domainStart)].lock(ctr, level, ++gblOrder);
    
    Variable::varProps[ivpToAssign].state = POS;

    if (ctr == nullptr && level)
        wasPushed = lastPushed = true;
    
    saveSize.top().sizeDom = domainCurSize = 1;
    lowerBound = upperBound = ivpToAssign;
    lastChoice = lowerBound;

    return true;
}   // assignAt


bool Variable::removeAt(indDomLocal index, int level, Constraint* ctr)
{
    Stats::rmCall++;

    if (index >= domainCurSize)
        return false;
    
    if (!index && domainCurSize == 1)
        return true;
    
    if (saveSize.empty() || saveSize.top().level != level)
        saveSize.push({ level, domainCurSize, domainCurSize });
    
    indVp ivpToDel = domValuesToVarProp(domainStart + index);

    if (Options::Verbose >= verbose::high) {
        if (ctr == nullptr)
            cerr << "(" << level << ") ";
        
        else {
            if ((unsigned)ctr->getId() >= Stats::saveNbConstraints)
                cerr << ctr->getName();
            else
                cerr << "ctr" << ctr->getId();
        }
        cerr << " ¬" << ivpToDel;
    }

    Variable::vpExpl[ivpToDel].lock(ctr, level, ++gblOrder);

    Variable::varProps[ivpToDel].state = NEG;

    --domainCurSize;
    saveSize.top().sizeDom--;

    setVarPropIndInDomValues(domainStart + index, domValuesToVarProp(domainStart + domainCurSize));
    setVarPropIndInDomValues(domainStart + domainCurSize, ivpToDel);

    setPosOfVarPropInVar(domainStart + index, index);
    setPosOfVarPropInVar(domainStart + domainCurSize, domainCurSize);

    if (lowerBound == ivpToDel) {
        for (int i = lowerBound + 1; i <= upperBound; ++i) {
            if (Variable::varProps[i].posInVar < domainCurSize) {
                lowerBound = i;
                break;
            }
        }
    }

    if (upperBound == ivpToDel) {
        for (int i = upperBound - 1; i >= lowerBound; --i) {
            if (Variable::varProps[i].posInVar < domainCurSize) {
                upperBound = i;
                break;
            }
        }
    }

    if (domainCurSize == 1) {
        Variable::varProps[domValuesToVarProp(domainStart)].state = POS;

        Variable::vpExpl[domValuesToVarProp(domainStart)].lock(Domain::domCtr, level, ++gblOrder);

        if (ctr == nullptr && level)
            wasPushed = lastPushed = true;
        
        if (Options::Verbose >= verbose::high)
            cerr << " => " << domValuesToVarProp(domainStart);
    }

    if (Options::Verbose >= verbose::high)
        cerr << endl;

    assert(domainCurSize != 0);

    return false;
}   // removeAt


bool Variable::keepOnlyValues(Variable* var, int k, int level, Constraint* ctr)
{
    cleanDBU();

    for (int i = 0, stop = var->domainCurSize; i < stop; ++i) {
        int val = var->getVarPropFromLocalDomInd(i).val + k;
        if (val < getLowerBoundVal() || val > getUpperBoundVal())
            continue;
        
        for (int i = 0; i < domainCurSize; ++i)
            if (getVarPropFromLocalDomInd(i).val == val) {
                domainBoolUtil[indDomLocalToIndVPLocal(i)] = true;
                break;
            }
    }

    return delInDBU(level, ctr);
}   // keepOnlyValues


bool Variable::keepOnlyValues(vector<int>& vals, int level, Constraint* ctr)
{
    cleanDBU();

    for (auto v : vals)
        domainBoolUtil[valueToVarPropInd[v] - domainStart] = true;
    
    return delInDBU(level, ctr);
}   // keepOnlyValues


bool Variable::keepOnlyVarProp(vector<vector<indVp>>& vals, int index, int level, Constraint* ctr)
{
    cleanDBU();

    for (auto v : vals) {
        if (v[index] == STAR)
            return false;
        else
            domainBoolUtil[v[index] - domainStart] = true;
    }

    return delInDBU(level, ctr);
}   // keepOnlyVarProp


bool Variable::keepOnlyVarProp(set<indVp>& VPs, int level, Constraint* ctr)
{
    cleanDBU();

    for (auto v : VPs)
        domainBoolUtil[v - domainStart] = true;

    return delInDBU(level, ctr);
}   // keepOnlyVarProp


bool Variable::keepOnlyVarProp(vector<indVp>& VPs, int level, Constraint* ctr)
{
    cleanDBU();

    for (auto v : VPs)
        domainBoolUtil[v - domainStart] = true;

    return delInDBU(level, ctr);
}   // keepOnlyVarProp



