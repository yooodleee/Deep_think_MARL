
#ifndef COSOCO_XCSP3PRINTCALLBACKS_H
#define COSOCO_XCSP3PRINTCALLBACKS_H


#include <XCSP3Tree.h>
#include "XCSP3CoreCallbacks.h"
#include "XCSP3Variable.h"

/**
 * This is an example that prints useful infos of a XCSP3 instance. 
 * Need to create own class and to override functions of the callbacks. 
 * Suggest to make a map between XVariable and own variable in order to 
 * facilitate the constructs of constraints. 
 * 
 * See main.cc to show declaration of the parser
 * 
 */

namespace XCSP3Core {

    class XCSP3PrintCallbacks : public XCSP3CoreCallbacks {
    public:
        XCSP3PrintCallbacks();

        virtual void beginInstance(InstanceType type) override;
        virtual void endInstance() override;

        virtual void beginVariables() override;
        virtual void endVariables() override;

        virtual void beginVariableArray(string id) override;
        virtual void endVariableArray() override;

        virtual void beginConstraints() override;
        virtual void endConstraints() override;

        virtual void beginGroup(string id) override;
        virtual void endGroup() override;

        virtual void beginBlock(string classes) override;
        virtual void endBlock() override;

        virtual void beginSlide(string id, bool circular) override;
        virtual void endSlide() override;

        virtual void beginObjectives() override;
        virtual void endObjectives() override;

        virtual void beginAnnotations() override;
        virtual void endAnnotations() override;

        virtual void buildVariableInteger(string id, int minValue, int maxValue) override;
        virtual void buildVariableInteger(string id, vector<int> &values) override;

        virtual void buildConstraintExtension(string id, vector<XVariable*> list, vector<vector<int>>& tuples, bool support, bool hasStar) override;
        virtual void buildConstraintExtension(string id, XVariable* variable, vector<int>& tuples, bool support, bool hasStar) override;

        virtual void buildConstraintExtensionAs(string id, vector<XVariable*> list, bool support, bool hasStar) override;

        virtual void buildConstraintIntension(string id, string expr) override;
        virtual void buildConstraintIntension(string id, Tree* tree) override;

        virtual void buildConstraintPrimitive(string id, OrderType op, XVariable* x, int k, XVariable* y) override;
        virtual void buildConstraintPrimitive(string id, OrderType op, XVariable* cx, int k) override;
        virtual void buildConstraintPrimitive(string id, XVariable* x,  bool in, int min, int max) override;

        virtual void buildConstraintRegular(string id, vector<XVariable*>& list, string st, vector<string>& final, vector<XTransition>& transitions) override;

        virtual void buildConstraintMDD(string id, vector<XVariable*>& list, vector<XTransition>& transitions) override;

        virtual void buildConstraintAlldifferent(string id, vector<XVariable*>& list) override;

        virtual void buildConstraintAlldifferentExcept(string id, vector<XVariable*>& list, vector<int>& except) override;

        virtual void buildConstraintAlldifferent(string id, vector<Tree*>& list) override;

        virtual void buildConstraintAlldifferentList(string id, vector<vector<XVariable*>>& lists) override;

        virtual void buildConstraintAlldifferentMatrix(string id, vector<vector<XVariable*>>& matrix) override;

        virtual void buildConstraintAllEqual(string id, vector<XVariable*>& list) override;

        virtual void buildConstraintNotAllEqual(string id, vector<XVariable*>& list) override;

        virtual void buildConstraintOrdered(string id, vector<XVariable*>& list, OrderType order) override;
        virtual void buildConstraintOrdered(string id, vector<XVariable*>& list, vector<int>& lengths, OrderType order) override;

        virtual void buildConstraintLex(string id, vector<vector<XVariable*>>& lists, OrderType order) override;

        virtual void buildConstraintLexMatrix(string id, vector<vector<XVariable*>>& matrix, OrderType order) override;

        virtual void buildConstraintSum(string id, vector<XVariable*>& list, vector<int>& coeffs, XCondition& cond) override;
        virtual void buildConstraintSum(string id, vector<XVariable*>& list, XCondition& cond) override;
        virtual void buildConstraintSum(string id, vector<XVariable*>& list, vector<XVariable*>& coeffs, XCondition& cond) override;
        virtual void buildConstraintSum(string id, vector<Tree*>& list, vector<int>& coeffs, XCondition& cond) override;
        virtual void buildConstraintSum(string id, vector<Tree*>& list, XCondition& cond) override;

        virtual void buildConstraintAtMost(string id, vector<XVariable*>& list, int value, int k) override;

        virtual void buildConstraintAtLeast(string id, vector<XVariable*>& list, int value, int k) override;

        virtual void buildConstraintExactlyK(string id, vector<XVariable*>& list, int value, int k) override;

        virtual void buildConstraintAmong(string id, vector<XVariable*>& list, vector<int>& values, int k) override;

        virtual void buildConstraintExactlyVariable(string id, vector<XVariable*>& list, int value, XVariable* x) override;

        virtual void buildConstraintCount(string id, vector<XVariable*>& list, vector<int>& values, XCondition& xc) override;
        virtual void buildConstraintCount(string id, vector<XVariable*>& list, vector<XVariable*>& values, XCondition& xc) override;

        virtual void buildConstraintNValues(string id, vector<XVariable*>& list, vector<int>& except, XCondition& xc) override;
        virtual void buildConstraintNValues(string id, vector<XVariable*>& list, XCondition& xc) override;

        virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<int> values, vector<int>& occurs, bool closed) override;
        virtual void buildConstraintCardinality(string id, vector<XVariable*> &list, vector<int> values, vector<XVariable*>& occurs, bool closed) override;
        virtual void buildConstraintCardinality(string id, vector<XVariable*> &list, vector<int> values, vector<XInterval>& occurs, bool closed) override;
        virtual void buildConstraintCardinality(string id, vector<XVariable*> &list, vector<XVariable*> values, vector<int>& occurs, bool closed) override;
        virtual void buildConstraintCardinality(string id, vector<XVariable*> &list, vector<XVariable*> values, vector<XVariable*>& occurs, bool closed) override;
        virtual void buildConstraintCardinality(string id, vector<XVariable*> &list, vector<XVariable*> values, vector<XInterval>& occurs, bool closed) override;

        virtual void buildConstraintMinimum(string id, vector<XVariable*>& list, XCondition& xc) override;
        virtual void buildConstraintMinimum(string id, vector<XVariable*>& list, XVariable* index, int startIndex, RankType rank, XCondition& xc) override;
        virtual void buildConstraintMaximum(string id, vector<XVariable*>& list, XCondition& xc) override;
        virtual void buildConstraintMaximum(string id, vector<XVariable*>& list, XVariable* index, int startIndex, RankType rank, XCondition& xc) override;

        virtual void buildConstraintElement(string id, vector<XVariable*>& list, int value) override;
        virtual void buildConstraintElement(string id, vector<XVariable*>& list, XVariable* value) override;
        virtual void buildConstraintElement(string id, vector<XVariable*>& list, int startIndex, XVariable* index, RankType rank, int value) override;
        virtual void buildConstraintElement(string id, vector<XVariable*>& list, int startIndex, XVariable* index, RankType rank, XVariable* value) override;
        virtual void buildConstraintElement(string id, vector<int>& list, int startIndex, XVariable* index, RankType rank, XVariable* value) override;

        virtual void buildConstraintChannel(string id, vector<XVariable*>& list, int startIndex) override;
        virtual void buildConstraintChannel(string id, vector<XVariable*>& list1, int startIndex1, vector<XVariable*>& list2, int startIndex2) override;
        virtual void buildConstraintChannel(string id, vector<XVariable*>& list, int startIndex, XVariable*value) override;

        virtual void buildConstraintStretch(string id, vector<XVariable*>& list, vector<int>& values, vector<XInterval>& widths) override;
        virtual void buildConstraintStretch(string id, vector<XVariable*>& list, vector<int>& values, vector<XInterval>& widths, vector<vector<int>>& patterns) override;

        virtual void buildConstraintNoOverlap(string id, vector<XVariable*>& origins, vector<int>& lengths, bool zeroIgnored) override;
        virtual void buildConstraintNoOverlap(string id, vector<XVariable*>& origins, vector<XVariable*>& lengths, bool zeroIgnored) override;
        virtual void buildConstraintNoOverlap(string id, vector<vector<XVariable*>>& origins, vector<vector<int>>& lengths, bool zeroIgnored) override;
        virtual void buildConstraintNoOverlap(string id, vector<vector<XVariable*>>& origins, vector<vector<XVariable*>>& lengths, bool zeroIgnored) override;

        virtual void buildConstraintInstantiation(string id, vector<XVariable*>& list, vector<int>& values) override;


        virtual void buildConstraintCircuit(string id, vector<XVariable*>& list, int startIndex) override;
        virtual void buildConstraintCircuit(string id, vector<XVariable*>& list, int startIndex, int size) override;
        virtual void buildConstraintCircuit(string id, vector<XVariable*>& list, int startIndex, XVariable* size) override;


        virtual void buildObjectiveMinimizeExpression(string expr) override;
        virtual void buildObjectiveMaximizeExpression(string expr) override;

        virtual void buildObjectiveMinimizeVariable(XVariable* x) override;

        virtual void buildObjectiveMaximizeVariable(XVariable* x) override;

        virtual void buildObjectiveMinimize(ExpressionObjective type, vector<XVariable*>& list, vector<int>& coefs) override;

        virtual void buildObjectiveMaximize(ExpressionObjective type, vector<XVariable*>& list, vector<int>& coefs) override;

        virtual void buildObjectiveMinimize(ExpressionObjective type, vector<XVariable*>& list) override;

        virtual void buildObjectiveMaximize(ExpressionObjective type, vector<XVariable*>& list) override;

        virtual void buildAnnotationDecision(vector<XVariable*>& list) override;

        bool canonize;
    };


}


