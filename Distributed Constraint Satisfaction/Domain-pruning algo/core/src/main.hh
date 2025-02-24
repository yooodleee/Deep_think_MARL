
#ifndef ARCH_XCSP3Callbacks_H_
#define ARCH_XCSP3Callbacks_H_


#include "CompleteSolver.hh"
#include "ConflictAnalysisSolver.hh"
#include "Constraint.hh"
#include "ConstraintAllEqual.hh"
#include "ConstraintAmong.hh"
#include "ConstraintCount.hh"
#include "ConstraintElement.hh"
#include "ConstraintExtBinary.hh"
#include "ConstraintExtN.hh"
#include "ConstraintExtNConflict.hh"
#include "ConstraintExtUnary.hh"
#include "ConstraintExtremum.hh"
#include "ConstraintInt.hh"
#include "ConstraintLexicographic.hh"
#include "ConstraintNotAllEqual.hh"
#include "ConstraintOrdered.hh"
#include "ConstraintPrimitive.hh"
#include "ConstraintSum.hh"
#include "ConstraintSumScalarBoolean.hh"
#include "ConstraintWeightedSum.hh"
#include "GeneralizedCompleteSolver.hh"
#include "IncNGCompleteSolver.hh"
#include "LecoutreCompleteSolver.hh"
#include "Options.hh"
#include "Problem.hh"
#include "Solver.hh"
#include "Variable.hh"


#include "XCSP3CoreCallbacks.h"
#include "XCSP3Variable.h"



namespace XCSP3Core {

class XCSP3Callbacks : public XCSP3CoreCallbacks {

public:
    XCSP3Callbacks()
    {
        recognizeSpecialIntensionCases = true;
        intensionUsingString = false;
        recognizeSpecialCountCases = true;
        recognizeNValuesCases = true;
        normalizeSum = true;
    }

    virtual void beginInstance(InstanceType type) override;

    virtual void buildVariableInteger(string id, int minValue, int maxValue) override;
    virtual void buildVariableInteger(string id, vector<int>& values) override;


    // GENERIC
    virtual void buildConstraintExtension(string id, vector<XVariable*> int, vector<vector<int>>& tuples, bool support, bool hasStar) override;
    virtual void buildConstraintExtension(string id, XVariable* variable, vector<int>& tuples, bool support, bool hasStar) override;
    virtual void buildConstraintExtensionAs(string id, vector<XVariable*> list, bool support, bool hasStar) override;

    virtual void buildConstraintIntension(string id, string expr) override;
    virtual void buildConstraintIntension(string id, Tree* tree) override;

    virtual void buildConstraintPrimitive(string id, OrderType op, XVariable* x, int k, XVariable* y) override;
    virtual void buildConstraintPrimitive(string id, OrderType op, XVariable* x, int k) override;
    virtual void buildConstraintPrimitive(string id, XVariable* x, bool in, int min, int max) override;


    // LANGUAGE
    virtual void buildConstraintRegular(string id, vector<XVariable*>& list, string st, vector<string>& final, vector<XTransition>& transitions) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }   
    virtual void buildConstraintMDD(string id, vector<XVariable*>& list, vector<XTransition>& transitions) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }


    // COMPARISON
    virtual void buildConstraintAlldifferent(string id, vector<Tree*>& list) override;
    virtual void buildConstraintAlldifferent(string id, vector<XVariable*>& list) override;
    virtual void buildConstraintAlldifferentExcept(string id, vector<XVariable*>& list, vector<int>& except) override;
    virtual void buildConstraintAlldifferentList(string id, vector<vector<XVariable*>>& lists) override;
    virtual void buildConstraintAlldifferentMatrix(string id, vector<vector<XVariable*>>& matrix) override;

    virtual void buildConstraintAllEqual(string id, vector<XVariable*>& list) override;
    virtual void buildConstraintLex(string id, vector<XVariable*>>& lists, OrderType order) override;
    virtual void buildConstraintLexMatrix(string id, vector<vector<XVariable*>>& matrix, OrderType order) override;
    virtual void buildConstraintNotAllEqual(string id, vector<XVariable*>& list) override;

    virtual void buildConstraintOrdered(string id, vector<XVariable*>& list, OrderType order) override;
    virtual void buildConstraintOrdered(string id, vector<XVariable*>& list, vector<int>& lengths, OrderType order) override;


    // SUMMING
    virtual void buildConstraintSum(string id, vector<Tree*>& list, vector<int>& coeffs, XCondition& cond) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintSum(string id, vector<Tree*>& list, XCondition& cond) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintSum(string id, vector<XVariable*>& list, vector<int>& coeffs, XCondition& cond) override;
    virtual void buildConstraintSum(string id, vector<XVariable*>& list, vector<XVariable*>& coeffs, XCondition& cond) override;
    virtual void buildConstraintSum(string id, vector<XVariable*>& list, XCondition& cond) override;


    // COUNTING
    virtual void buildConstraintAmong(string id, vector<XVariable*>& list, vector<int>& values, int k) override;
    virtual void buildConstraintAtLeast(string id, vector<XVariable*>& list, int value, int k) override;
    virtual void buildConstraintAtMost(string id, vector<XVariable*>& list, int value, int k) override;
    virtual void buildConstraintCount(string id, vector<XVariable*>& list, vector<int>& values, XCondition& xc) override;
    virtual void buildConstraintCount(string id, vector<XVariable*>& list, vector<XVariable*>& values, XCondition& xc) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintExactlyK(string id, vector<XVariable*>& list, int value, int k) override;
    virtual void buildConstraintExactlyVariable(string id, vector<XVariable*>& list, int value, XVariable* x) override;
    virtual void buildConstraintNValues(string id, vector<XVariable*>& list, vector<int>& except, XCondition& xc) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintNValues(string id, vector<XVariable*>& list, XCondition& xc) override;

    virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<int> values, vector<int>& occurs, bool closed) override;
    virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<int> values, vector<XInterval>& occurs, bool closed) override;
    virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<int> values, vector<XVariable*>& occurs, bool closed) override;
    virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<XVariable*> values, vector<int>& occurs, bool closed) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<XVariable*> values, vector<XInterval>& occurs, bool closed) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<XVariable*> values, vector<XVariable*>& occurs, bool closed) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported") << endl;
    }


    // CONNECTION
    virtual void buildConstraintChannel(string id, vector<XVariable*>& list, int startIndex, XVariable* value) override;
    virtual void buildConstraintChannel(string id, vector<XVariable*>& list, int startIndex) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintChannel(string id, vector<XVariable*>& list1, int startIndex1, vector<XVariable*>& list2, int startIndex2) override;

    virtual void buildConstraintElement(string id, vector<int>& list, int startIndex, XVariable* index, RankType rank, XVariable* value) override;
    virtual void buildConstraintElement(string id, vector<XVariable*>& list, int startIndex, XVariable* index, RankType rank, int value) override;
    virtual void buildConstraintElement(string id, vector<XVariable*>& list, int startIndex, XVariable* index, RankType rank, XVariable* value) override;
    virtual void buildConstraintElement(string id, vector<XVariable*>& list, int value) override;
    virtual void buildConstraintElement(string id, vector<XVariable*>& list, XVariable* value) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintMaximum(string id, vector<XVariable*>& list, XCondition& xc) override;
    virtual void buildConstraintMaximum(string id, vector<XVariable*>& list, XVariable* index, int startIndex, RantType rank, XCondition& xc) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual bool buildConstraintMinimum(string id, vector<XVariable*>& list, XCondition& xc) override;
    virtual bool buildConstraintMinimum(string id, vector<XVariable*>& list, XVariable* index, int startIndex, RankType rank, XCondition& xc) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }


    // INISTANTIATION
    virtual void buildConstraintInstantiation(string id, vector<XVariable*>& list, vector<int>& values) override;

    // PACKING AND SCHEDULLING
    virtual void buildConstraintCircuit(string id, vector<XVariable*>& list, int startIndex, int size) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCircuit(string id, vector<XVariable*>& list, int startIndex, XVariable* size) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCircuit(string id, vector<XVariable*>& list, int startIndex) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintNoOverlap(string id, vector<vector<XVariable*>>& origins, vector<vector<int>>& lenghts, bool zeroIgnored) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintNoOverlap(string id, vector<vector<XVariable*>>& origins, vector<vector<XVariable*>>& lengths, bool zerogIgnored) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintNoOverlap(string id, vector<XVariable*>& origins, vector<int>& lenghts, bool zeroIgnored) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintNoOverlap(string id, vector<XVariable*>& origins, vector<XVariable*>& lengths, bool zeroIgnored) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }

    virtual void buildConstraintStretch(string id, vector<XVariable*>& list, vector<int>& values, vector<XInterval>& widths, vector<vector<int>>& patterns) override
    {
        std::cout << "s UNSUPPORTED" endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintStretch(string id, vector<XVariable*>& list, vector<int>& values, vector<XInterval>& widths) override
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }


    // CUMULATIVE
    virtual void buildConstraintCumulative(string id, vector<XVariable*>& origins, vector<int>& lenghts, vector<int>& heights, vector<XVariable*>& ends, XCondition& xc)
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCumulative(string id, vector<XVariable*>& origins, vector<int>& lenghts, vector<int>& heights, XCondition& xc)
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCumulative(string id, vector<XVariable*>& origins, vector<int>& lengths, vector<XVariable*>& varHeights, vector<XVariable*>& ends, XCondition& xc)
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCumulative(string id, vector<XVariable*>& origins, vector<int>& lenghts, vector<XVariable*>& varHeights, XCondition& xc)
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCumulative(string id, vector<XVariable*>& origins, vector<XVariable*>& lengths, vector<int>& heights, vector<XVariable*>& ends, XCondition& xc)
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCumulative(string id, vector<XVariable*>& origins, vector<XVariable*>& lengths, vector<int>& heights, XCondition& xc)
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCumulative(string id, vector<XVariable*>& origins, vector<XVariable*>& lenghts, vector<XVariable*> heights, vector<XVariable*> ends, XCondition& xc)
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
    virtual void buildConstraintCumulative(string id, vector<XVariable*>& lengths, vector<XVariable*>& heights, XCondition& xc)
    {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Unsupported");
    }
};
}


using namespace XCSP3Core;

int typeCSP;

vector<Constraint*> vecCont;
vector<Constraint*> vecBtConst;
Problem* problem;

std::map<string, Variable*> mapping;


void toMyVariables(vector<XVariable*>& src, vector<Variable*>& dest)
{
    for (size_t i = 0, stop = src.size(); i < stop; ++i) {
        if (!mapping[src[i]->id])
            throw runtime_error("Unknow value " + src[i]->id);
        dest.push_back(mapping[src[i]->id]);
    }
}


string createExpression(string minmax, OrderType op, vector<XVariable*>& list, string value)
{
    string expr;

    switch (op) {
    case LT:
        expr = "lt";
        break;
    case LE:
        expr = "le";
        break;
    case GT:
        expr = "gt";
        break;
    case GE:   
        expr = "ge";
        break;
    case EQ:
        expr = "eq";
        break;
    case NE:
        expr = "ne";
        break;
    default:
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("not yet supported");
        break;
    }

    expr += "(" + value + "," + minmax + "(";
    for (size_t i = 0, stop = list.size(); i < stop; i++)
        expr += (i != 0 ? "," : "") + list[i]->id;
    expr += "))";
    
    return expr;
}


void XCSP3Callbacks::beginInstance(InstanceType type)
{
    if (type == COP) {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("COP instances unsupported");
    }

    typeCSP = type;
    problem = new Problem();
}


void XCSP3Callbacks::buildVariableInteger(string id, int minValue, int maxValue)
{
    mapping[id] = (minValue == maxValue) ? new Variable(id, maxValue) : new Variable(id, minValue, maxValue);
    problem->addVariable(mapping[id]);
}


void XCSP3Callbacks::buildVariableInteger(string id, vector<int>& values)
{
    if (values.size() == 1)
        mapping[id] = new Variable(id, values[0]);
    else
        mapping[id] = new Variable(id, values);
    problem->addVariable(mapping[id]);
}


vector<int> lastTuples;
vector<vector<int>> lastTuplesN;
bool lastExtUnary;


void createExt(string id, vector<XVariable*>& list, vector<vector<int>>& tuples, bool support)
{
    vector<Variable*> vars;
    toMyVariables(list, vars);

    vector<vector<ind>> t(tuples.size());
    for (size_t i = 0; i < tuples.size(); ++i) {
        vector<ind> tmp;
        for (size_t j = 0; j < tuples.size(); ++j)
            tmp.push_back((tuples[i][j] == STAR ? STAR : vars[i]->getVarPropIndFromValue(tuples[i][j])));
        t[i] = tmp;
    }

    if (list.size() == 2) {
        vecCont.push_back(new ConstraintExtBinary(id, vars, t, support));
    } else {
        Constraint* newCont = (support ? static_cast<Constraint*>(new ConstraintExtN(id, vars, t)) : static_cast<Constraint*>(new ConstraintExtNConflict(id, vars, t)));
        vecCont.push_back(newCont);
        vecBtConst.push_back(newCont);
    }
}


Variable* getMyVar(XVariable* x)
{
    Variable* var = mapping[x->id];
    if (!var)
        throw runtime_error("Unknow variable :" + x->id);
    return var;
}


void XCSP3Callbacks::buildConstraintExtension(string id, vector<XVariable*> list, vector<vector<int>>& tuples, bool support, bool hasStar)
{
    lastTuplesN = tuples;
    lastExtUnary = false;

    if (hasStar && !support) {
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Extension conflict constraint with star is not yet supported");
    }

    if (!support && !lastTuplesH.size())
        return;
    if (support && !lastTuplesN.size()) {
        std::cout << "s UNSATISFIABLE" << endl;
        throw runtime_error("UNSAT by default");
    }

    createExt(id, list, lastTuplesN, support);
}



void XCSP3Callbacks::buildConstraintExtension(string id, XVariable* variable, vector<int>& tuples, bool support, bool hasStar)
{
    lastTuples = tuples;
    lastExtUnary = true;

    if (hasStar) {
        if (support) {
            std::cout << "s UNSATISFIABLE" << endl;
            throw runtime_error("UNSAT by default");
        }

        return;
    }

    vecCont.push_back(new ConstraintExtUnary(id, getMyVar(variable, lastTuples, support)));
}


void XCSP3Callbacks::buildConstraintExtensionAs(string id, vector<XVariable*> list, bool support, bool hasStar)
{
    if (lastExtUnary) {
        assert(list.size() == 1);
        vecCont.push_back(new ConstraintExtUnary(id, getMyVar(list[0]), lastTuples, support));
    } else {
        if (!support && !lastTuplesN.size())
            return;
        createExt(id, list, lastTuplesN, support);
    }
}


void XCSP3Callbacks::buildConstraintIntension(string id, string expr)
{
    Tree* tree = new Tree(expr);
    buildConstraintIntension(id, tree);
}


void XCSP3Callbacks::buildConstraintIntension(string id, Tree* tree)
{
    std::vector<Variable*> scope;
    for (auto var_str : tree->listOfVariables) {
        Variable* var = mapping[var_str];
        if (!var) 
            throw runtime_error("Unknow value " + var_str);
        scope.push_back(var);
    }

    Constraint* newCont = static_cast<Constraint*>(new Constraint(id, scope, tree));
    vecCont.push_back(newCont);
}


// Recognize constraint with the form x +- k op y (op \in {eq, le, lt, ge, gt, ne}.
void XCSP3Callbacks::buildConstraintPrimitive(string id, OrderType op, XVariable* x, int k, XVariable* y)
{
    switch (op) {
    case LT:
        vecCont.push_back(new ConstraintPrimitiveLeeThan(id, getMyVar(x), getMyVar(y), k));
        break;
    case LE:
        vecCont.push_back(new ConstraintPrimitiveLessEqual(id, getMyVar(x), getMyVar(y), k));
        break;
    case GT:
        vecCont.push_back(new ConstraintPrimitiveLeeThan(id, getMyVar(y), getMyVar(x), -k));
        break;
    case GE:
        vecCont.push_back(new ConstraintPrimitiveLessEqual(id, getMyVar(y), getMyVar(x), -k));
        break;
    case EQ:
        vecCont.push_back(new ConstraintPrimitiveEqual(id, getMyVar(x), getMyVar(y), k));
        break;
    case NE:
        vecCont.push_back(new ConstraintPrimitiveNotEqual(id, getMyVar(x), getMyVar(y), k));
        break;
    default:
        std::cout << "s UNSUPPORTED" << endl;
        throw runtime_error("Primitive not supported");
        break;
    }
}


void XCSP3Callbacks::buildConstraintPrimitive(string id, OrderType op, XVariable* x, int k)
{
    assert(op == LE || op == GE);
    Variable* var = getMyVar(x);

    if (op == LE && var->removeValuesAbove(k, 0)) {
        std::cout << "s UNSATISFIABLE" << endl;
        throw runtime_error("Domain wipeout of " + x->id + " during parsing");
    } else if (op == GE && var->removeValuesUnder(k, 0)) {
        std::cout << "s UNSATISFIABLE" << endl;
        throw runtime_error("Domain wipeout of " + x->id + " during parsing");
    }
}


void XCSP3Callbacks::buildConstraintPrimitive(string id, XVariable* x, bool in, int min, int max)
{
    // x=>min and x<=max, x in/notin k
    Variable* var = getMyVar(x);

    if (in) {
        if (min == max)
            var->assignTo(min, 0);
        else {
            if (var->removeValuesAbove(max, 0)) {
                std::cout << "s UNSATISFIABLE" << endl;
                throw runtime_error("Domain wipeout of " + x->id + " during parsing");
            }
            if (var->removeValuesUnder(min, 0)) {
                std::cout << "s UNSATISFIABLE" << endl;
                throw runtime_error("Domain wipeout of " + x->id + " during parsing");
            }
        }
    } else if (min == max) {
        if (var->removeValues(min, 0)) {
            std::cout << "s UNSATISFIABLE" << endl;
            throw runtime_error("Domain wipeout of " + x->id + " during parsing");
        }
    }
}


void XCSP3Callbacks::buildConstraintAllEqual(string id, vector<XVariable*>& list)
{
    vector<Variable*> vars;
    toMyVariables(list, vars);

    ConstraintAllEqual* newCont = new ConstraintAllEqual(id, vars);

    vecCont.push_back(newCont);
    vecBtConst.push_back(newCont);
}


void XCSP3Callbacks::buildConstraintNotAllEqual(string id, vector<XVariable*>& list)
{
    vector<Variable*> vars;
    toMyVariables(list, vars);

    vecCont.push_back(new ConstraintNotAllEqual(id, vars));
}


void XCSP3Callbacks::buildConstraintAlldifferent(string id, vector<Tree*>& list)
{
    for (size_t i = 0; i < list.size() - 1; i++) {
        for (size_t j = i + 1; j < list.size(); j++) {
            NodeNE* n = new NodeNE();
            n->addParameter(list[i]->root);
            n->addParameter(list[j]->root);
            Tree* tmp = new Tree(n);

            // Problem with scope
            for (string s : list[i]->listOfVariables)
                tmp->listOfVariables.push_back(s);
            for (string s : list[j]->listOfVariables)
                if (std::find(tmp->listOfVariables.begin(), tmp->listOfVariables.end(), s) == tmp->listOfVariables.end())
                    tmp->listOfVariables.push_back(s);
            buildConstraintIntension(id, tmp);
        }
    }
}


void XCSP3Callbacks::buildConstraintAlldifferent(string id, vector<XVariable*>& list)
{
    vector<Variable*> vars;
    toMyVariables(list, vars);

    for (size_t i = 0; i < vars.size() - 1; ++i)
        for (size_t j = i + 1; j < vars.size(); ++j)
            vecCont.push_back(new ConstraintPrimitiveNotEqual(id, vars[i], vars[j], 0));
}


