
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


