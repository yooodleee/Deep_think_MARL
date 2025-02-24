
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
    vir
}
}