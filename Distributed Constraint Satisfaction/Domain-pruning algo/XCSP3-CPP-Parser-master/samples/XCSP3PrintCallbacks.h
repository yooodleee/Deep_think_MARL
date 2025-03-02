
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


using namespace XCSP3Core;


XCSP3PrintCallbacks::XCSP3PrintCallbacks() : XCSP3CoreCallbacks(), canonize(true) {}


template<class T>
void displayList(vector<T>& list, string separator = " ") {
    if (list.size() > 8) {
        for (int i = 0; i < 3; i++)
            cout << list[i] << separator;
        cout << " ... ";
        for (unsigned int i = list.size() - 4; i < list.size(); i++)
            cout << list[i] << separator;
        cout << endl;
        return;
    }

    for (unsigned int i = 0; i < list.size(); i++)
        cout << list[i] << separator;
    cout << endl;
}


void displayList(vector<XVariable*>& list, string separator = " ") {
    if (list.size() > 8) {
        for (int i = 0; i < 3; i++)
            cout << list[i]->id << separator;
        cout << " ... ";
        for (unsigned int i = list.size() - 4; i < list.size(); i++)
            cout << list[i]->id << separator;
        cout << endl;
        return;
    }

    for (unsigned int i = 0; i < list.size(); i++)
        cout << list[i]->id << separator;
    cout << endl;
}


void XCSP3PrintCallbacks::beginInstance(InstanceType type) {
    cout << "Start Instance - tpye=" << type << endl;
}

void XCSP3PrintCallbacks::endInstance() {
    cout << "End SAX parsing " << endl;
}

void XCSP3PrintCallbacks::beginVariables() {
    cout << " start variables declaration" << endl;
}

void XCSP3PrintCallbacks::endVariables() {
    cout << " end variables declaration" << endl << endl;
}

void XCSP3PrintCallbacks::beginVariableArray(string id) {
    cout << "       array: " << id << endl;
}

void XCSP3PrintCallbacks::endVariableArray() {

}

void XCSP3PrintCallbacks::beginConstraints() {
    cout << " start constraints declaration" << endl;
}

void XCSP3PrintCallbacks::endConstraints() {
    cout << "\n end constraints declaration" << endl << endl;
}

void XCSP3PrintCallbacks::beginGroup(string id) {
    cout << "   start group of constraint " << id << endl;
}

void XCSP3PrintCallbacks::endGroup() {
    cout << "   end group of constraint" << endl;
}

void XCSP3PrintCallbacks::beginBlock(string classes) {
    cout << "   start block of constraint classes = " << classes << endl;
}

void XCSP3PrintCallbacks::endBlock() {
    cout << "   end block of constraint" << endl;
}


// string id, bool circular
void XCSP3PrintCallbacks::beginSlide(string id, bool) {
    cout << "   start slide" << id << endl;
}

void XCSP3PrintCallbacks::endSlide() {
    cout << "   end slide" << endl;
}

void XCSP3PrintCallbacks::beginObjectives() {
    cout << "   start Objective " << endl;
}

void XCSP3PrintCallbacks::endObjectives() {
    cout << "   end Objective " << endl;
}

void XCSP3PrintCallbacks::beginAnnotations() {
    cout << "   begin Annotations " << endl;
}

void XCSP3PrintCallbacks::endAnnotations() {
    cout << "   end Annotations " << endl;
}



void XCSP3PrintCallbacks::buildVariableInteger(string id, int minValue, int maxValue) {
    cout << "   var " << id << " : " << minValue << "..." << maxValue << endl;
}

void XCSP3PrintCallbacks::buildVariableInteger(string id, vector<int>& values) {
    cout << "   var " << id << " : ";
    cout << "       ";
    displayList(values);
}

