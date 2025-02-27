
#ifndef COSOCO_XCSP3CORECALLBACKS_H_
#define COSOCO_XCSP3CORECALLBACKS_H_


#include "XCSP3Constants.h"
#include "XCSP3Variable.h"
#include "XCSP3Constraint.h"
#include "XCSP3Tree.h"


#include <vector>
#include <string>


namespace XCSP3Core {

    using namespace std;

    class XCSP3CoreCallbacks {
    protected:
        vector<string> classesToDiscard;

    public:

        /**
         * If ture, the intension constraint are retrieved with an expression (nothing is done in order to help)
         * (false by default)
         * Otherwise, the callback that take a canonized tree of the expression is called. 
         */
        bool intensionUsingString;

        /**
         * If true, the parse recongnizes special intensional constraints such as x + k op y and call a specific callback. 
         */
        bool recognizeSpecialIntensionCases;


        /**
         * If true, the parser recognizes special count constraints: atleast, almost, exactly, among, exctalyVariable
         * and call a specific callback. 
         */
        bool recognizeSpecialCountCases;

        /**
         * If true, the parser recognizes special nvalues consraints: AllEqual, NotAllEqual, AllDiff
         * and call a specific callback. 
         */
        bool recognizeNValuesCases;

        /**
         * if true, sum are normalized: merge same variables, remove variables with coef equal to zero.. 
         */
        bool normalizeSum;


        XCSP3CoreCallbacks() {
            intensionUsingString = false;
            recognizeSpecialIntensionCases = true;
            recognizeSpecialCountCases = true;
            recognizeNValuesCases = true;
            normalizeSum = true;
        }


        /**
         * remove specific classes such as symmetryBreaking, clues... 
         * @param cl
         */
        void addClassToDiscard(string cl) {
            classesToDiscard.push_back(cl);
        }


        bool discardedClasses(string classes) {
            if (classes == "")
                return false;

            for (string c : classesToDiscard)
                if (classes.find(c) != std::string::npos)
                    return true;
            
            return false;
        }


        // All these callbacks are called when the tag starts and when it ends.
        // This is not necessary to override them.


        /**
         * Start to parse a XCSP intence. 
         * Related to tag <instance type="CSP/COP"> 
         * See http://xcsp.org/specifications/skeleton
         * @param type COP or CSP
         */
        virtual void beginInstance(InstanceType type) {}


        /**
         * End of parsing
         * Related to tag </instance> 
         * See http://xcsp.org/specifications/skeleton
         */
        virtual void endInstance() {}


        /**
         * Start to parse variables
         * Related to tag <variables>
         * See http://xcsp.org/specifications/skeleton
         */
        virtual void beginVariables() {}


        /**
         * The end of parsing variables
         * Related to tag </variables> 
         * See http://xcsp/org/specifications/skeleton
         */
        virtual void endVariables() {}


        /**
         * Start to parse an array of variables
         * Related to tag <array>
         * See http://xcsp.org/specifications/arrays
         * Note that for each variable in the array a call is done to one of the functions #buildVariableInteger
         * 
         * @param id the id (name) of the array variable
         */
        virtual void beginVariableArray(string id) {}   // beginArray


        /**
         * End of parsing an array of variables
         * Related to tag </array> 
         * See http://xcsp.org/specifications/arrays
         */
        virtual void endVariableArray() {}


        /**
         * Start to parse constraints
         * Related to tag <constraints> 
         * See http://xcsp.org/specifications/skeleton
         */
        virtual void beginConstraints() {}


        /**
         * The end of parsing constraints
         * Related to tag </constraints> 
         * See http://xcsp.org/specifications/skeleton
         */
        virtual void endConstraints() {}


        /**
         * Start to parse a group of constraints
         * Related to tag <group> 
         * See http://xcsp.org/specifications/groups
         * Note that for each constraint of the group a call is done to the related callback
         * 
         * @param in the id (name) of the group
         */
        virtual void beginGroup(string id) {}


        /**
         * The end of parsing group of constraints
         * Related to tag </group> 
         * See http://xcsp.org/specifications/groups
         */
        virtual void endGroup() {}


        /**
         * Start to parse a block of constraints
         * Related to tag <block> 
         * See http://xcsp.org/specifications/blocks
         * Note that for each constraint of the block a call is done to the related callback
         * 
         * Note that if want to discard some blocks, need to call the function addClassToDiscard with the related class
         * 
         * @param classes the classes related to the block symmetryBreaking, clues... 
         */
        virtual void beginBlock(string classes) {}


        /**
         * The end of parsing a block of constraints
         * Related to tag </block> 
         * See http://xcsp.org/specifications/blocks
         */
        virtual void endBlock() {}


        /**
         * Start to parse a slide constraint
         * Related to tag <slide> 
         * See http://xcsp.org/specifications/slide
         * Note that for each constraint of the block a call is done to the related callback
         * 
         * @param id the id (name) of the slide
         * @param circular is the slide circular?
         */
        virtual void beginSlide(string id, bool circular) {}


        /**
         * The end of parsing a slide constraint
         * Related to tag </slide> 
         * See http://xcsp.org/specifications/slide
         */
        virtual void endSlide() {}


        /**
         * Start to parse objectives
         * Related to tag <objectives> 
         * See http://xcsp.org/specifications/objectives
         */
        virtual void beginObjectives() {}


        /**
         * The end of parsing objectives
         * Related to tag </objectives> 
         * See http://xcsp.org/specifications/objectives
         */
        virtual void endObjectives() {}


        /**
         * The start of parsing annotations
         * Related to tag </annotation> 
         */
        virtual void beginAnnotations() {}


        /**
         * The end of parsing annotations
         * Related to tag </annotations> 
         */
        virtual void endAnnotations() {}


        // ##########################################################
        // Build Variable. Must be implmented.
        // ##########################################################


        /**
         * The callback function related to an integer variable with a range domain
         * See http://xcsp.org/specifications/integers
         * 
         * Example: <var id="bar">0..6 </var> 
         * 
         * @param id the id (name) of the group
         * @param minvalue the minimum value in the range
         * @param maxvalue the maximum value in the range
         */
        virtual void buildVariableInteger(string id, int minValue, int maxvalue) = 0;


        /**
         * The callback function related to an integer variable with a domain consisting in a sequence of integers
         * See http://xcsp.org/specifications/integers
         * 
         * Example <var id="bar"> 1 3 5 10 </var> 
         * 
         * @param id the id (name) of the group
         * @param values the set of values in the domain
         */
        virtual void buildVariableInteger(string id, vector<int>& values) = 0;


        /**
         * All callbacks related to constraints. 
         * Note that the variables related to a constraint are #XVariable instances. A XVariable contains an id
         * the related domain. 
         * (see XCSP3Variable.h)
         * 
         */



        // ##########################################################
        // Universal Constraints
        // ##########################################################


        /**
         * The special constraint always true
         * Nothing to do
         * @param id
         * @param list
         */
        virtual void buildConstraintTrue(string id) {}


        /**
         * The special constraint always false
         * The problem is unsatisfiable
         * @param id
         * @param list
         */
        virtual void buildConstraintFalse(string id) {
            std::cout << "c constraint " + id + " is always false (see during parsing)\n";
            std::cout << "s UNSATISFIABLE\n";
        }


        // ##########################################################
        // Basic constraints
        // ##########################################################


        /**
         * The callback function related to an constraint in extension
         * See http://xcsp.org/specifications/extension
         * 
         * Example: 
         * <extension> 
         *      <list> y1 y2 y3 y4 </list> 
         *      <conflicts> (1,2,3,4)(3,1,3,4) </conflicts> 
         * </extension> 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param tuples the set of tuples in the constraint
         * @param support support or conflicts?
         * @param hasStar is the tuples contain star values?
         */
        virtual void buildConstraintExtension(string id, vector<XVariable*> list, vector<vector<int>>& tuples, bool support, bool hasStar) {
            std::cout << "WARNING: tuples are not checked wrt domains" << std::endl;
            throw runtime_error("extension constraint is not yet supported");
        }


        /**
         * The callback function related to an constraint in extension
         * Note that this callback is related to an unary constraint
         * See http://xcsp.org/specifications/extension
         * 
         * Example: 
         * <extension> 
         *      <list> x </list> 
         *      <conflicts> 2 6 </conflicts> 
         * </extension> 
         * 
         * @param id the id (name) of the constraint
         * @param variable the variable
         * @param tuples the set of tuple (here just set of ints)
         * @param support support or conflicts?
         * @param hasStar is the tuples contain star values?
         */
        virtual void buildConstraintExtension(string id, XVariable* variable, vector<int>& tuples, bool support, bool hasStar) {
            throw runtime_error("unary extension constraint is not yet supported");
        }


        /**
         * The callback function related to a constraint in extension where the set of tuples is exactly the same
         * than the previous one. 
         * It is the case when a group of constraint contains an extension constraint. 
         * This is useful to save space and share the set of tuples of all constraints. 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param support support or conflicts?
         * @param hasStar is the tuples contain star values?
         */
        virtual void buildConstraintExtensionAs(string id, vector<XVariable*> list, bool support, bool hasStar) {
            throw runtime_error("This extension constraint contains exactly the same tuples than previous one");
        }


        /**
         * The callback function related to a constraint in intension
         * Only called if intensionUsingString is set to true (otherwise the next function is called
         * See http://xcsp.org/specifications/intension
         * 
         * Example: 
         * <intension> eq(add(x,y),z) </intension> 
         * 
         * If need a class that is able to manage expressions can use the class Tree (see includes/XCSP3Tree.h)
         * And an exmaple is given in samples/testRee.cc 
         * 
         * In such a case, set intensionUsingString to false and make a callback to the next function
         * 
         * @param id the id (name) of the constraint
         * @param expr the expression
         */
        virtual void buildConstraintIntension(string id, string expr) {
            throw runtime_error("intension constraint is not yet supported");
        }


        /**
         * The callback function related to a constraint in intension
         * Only called if intensionUsingString is set to false (otherwise the previous function is called
         * See http://xcsp.org/specifications/intension
         * 
         * Example: 
         * <intension> eq(add(x,y,),z) </intension> 
         * 
         * @param id the id (name) of the constraint
         * @param tree the canonized from related to the tree
         */
        virtual void buildConstraintIntension(string id, Tree* tree) {
            throw runtime_error("intension constraint using a tree is not yet supported (choose the rihgt way)");
        }


        /**
         * If #recognizeSpecialIntensionCases is enabled (that is the case by default)
         * intensional constraint of the form : x +-k op y is recognized. 
         * If such a intensional constraint is recognized, a callback to this function is done and not to #buildConstraintIntension
         * 
         * @param id the id (name) of the constraint
         * @param op the order LE, LT... 
         * @param x the variable
         * @param k the constraint
         * @param y the other variable
         */
        virtual void buildConstraintPrimitive(string id, OrderType op, XVariable* x, int k, XVariable* y) {
            throw runtime_error(
                "primitive constraint x +-k op y is not yet supported. "
                "You can use classical intension constraint by assigning recognizeSpecialIntensionCases to false ");
        }


        /**
         * if #recognizeSpecialIntensionCases is enabled (this is the case by default)
         * intensional constraint of the form " x op k is recognized.
         * If such a intensional constraint is recognized, a callback to this function is done and not to #buildConstraintIntension
         * 
         * @param id the id (name) of the constraint
         * @param op the order LE or GE (EQ and NE are performed using #buildConstraintExtension)
         * @param x the variable
         * @param k the constraint
         */
        virtual void buildConstraintPrimitive(string id, OrderType op, XVariable* x, int k) {
            throw runtime_error(
                "primitive constraint x op k is not yet supported. "
                "You can use classical intension constraint by assiging recognizeSpecialIntensionCases to false ");
        }


        /**
         * If #recognizeSpecialIntensionCases is enabled (this is the case by default)
         * intensional constraint of the form : x in/notin [min max] are recognized
         * If such a intensional constraint is recognized, a callback to this function is done and not to #buildConstraintIntension
         * 
         * @param id the id (name) of the constraint
         * @param x the variable
         * @param in true if x is in this interval
         * @param min the constraint
         * @param max the constraint
         */
        virtual void buildConstraintPrimitive(string id, XVariable* x, bool in, int min, int max) {
            throw runtime_error(
                "primitive constraint x in/notin [min, max] is not yet supported. "
                "You can use classical intension constraint by assigning recognizeSpecialIntensionCases to false "
            );
        }



        // ##########################################################
        // Language Constraints
        // ##########################################################


        /**
         * The callback function related to a regular constraint. 
         * See http://xcsp.org/specifications/regular
         * 
         * Example: 
         * <regular> 
         *  <list> x1 x2 x3 x4 x5 x6 x7 </list> 
         *  <transitions>
         *      (a,0,a)(a,1,b)(b,1,c)(c,0,d)(d,0,d)(d,1,e)(e,0,e)
         *  </transitions> 
         *  <start> a </start> 
         *  <final> e </final> 
         * </regular> 
         * 
         * XTransition is an obj with 3 fields: from (string), val(int) and to(string)
         * Then, in the first transition of the example from=a, to=a and val=0
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param start the starting node
         * @param final the set of final nodes
         * @param transitions the set of transitions
         */
        virtual void buildConstraintRegular(string id, vector<XVariable*>& list, string start, vector<string>& final, vector<XTransition>& transitions) {
            throw runtime_error("regular constraint is not yet supported");
        }


        /**
         * The callback function related to a MDD constraint. 
         * See http://xcsp.org/specifications/mdd
         * 
         * Example: 
         * <mdd> 
         *  <list> x1 x2 x3 </list> 
         *  <transitions> 
         *      (r,0,n1)(r,1,n2)(r,2,n3)
         *      (n1,2,n4)(n2,2,n4)(n3,0,n5)
         *      (n4,0,t)(n5,0,t)
         *  </transitions>
         * </mdd> 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param transitions the set of transitions
         */
        virtual void buildConstraintMDD(string id, vector<XVariable*>& list, vector<XTransition>& transitions) {
            throw runtime_error("MDD constraint is not yet supported");
        }

        // ##########################################################
        // Comparison Constraints
        // ##########################################################


        /**
         * The callback function related to a all different constraint.
         * See http://xcsp.org/specifications/alldifferent
         * 
         * Example: 
         * <allDifferent> 
         *  x1 x2 x3 x4
         * </allDifferent> 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         */
        virtual void buildConstraintAlldifferent(string id, vector<XVariable*>& list) {
            throw runtime_error("AllDiff constraint is not yet supported");
        }


        /**
         * The callback function related to a alldifferent constraint with expression
         * See http://xcsp.org/specifications/alldifferent
         * 
         * Example: 
         * <allDifferent> 
         *      add(q[0],0) add(q[1],1) add(q[2],2) add(q[3],3) add(q[4],4) add(q[5],5) add(q[6],6) add(q[7],7)
         * </allDifferent> 
         * 
         * @param id the id (name) of the constraint
         * @param list the trees of the constraint
         */
        virtual void buildConstraintAlldifferent(string id, vector<Tree*>& list) {
            throw runtime_error("AllDiff constraint with expression is not yet supported");
        }

        /**
         * The callback function related to a alldifferent with some excepted values constraint
         * See http://xcsp.org/specifications/alldifferent
         * 
         * Example: 
         * <allDifferent> 
         *      x1 x2 x3 x4 x5
         *      <except>0</except> 
         * </allDifferent> 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param except the set of excepted values
         */
        virtual void buildConstraintAlldifferentExcept(string id, vector<XVariable*>& list, vector<int>& except) {
            throw runtime_error("AllDiff constraint with exception is not yet supported");
        }


        /**
         * The callback function related to a alldifferent list constraint
         * See http://xcsp.org/specifications/alldifferent
         * 
         * Example: 
         * <allDifferent id="c1"> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <list> y1 y2 y3 y4 </list> 
         * <allDifferent> 
         * 
         * @param id the id (name) of the constraint
         * @param lists the set of lists (not the scope, a vairable may appear at different place!)
         */
        virtual void buildConstraintAlldifferentList(string id, vector<vector<XVariable*>>& lists) {
            throw runtime_error("AllDiff list constraint is not yet supported");
        }


        /**
         * The callback function related to a alldifferent matrix constraint
         * See http://xcsp.org/specifications/alldifferent
         * 
         * Example: 
         * <allDifferent id="c1"> 
         *      <matrix> 
         *          (x1,x2,x3,x4,x5)
         *          (y1,y2,y3,y4,y5)
         *          (z1,z2,z3,z4,z5)
         *      </matrix> 
         * </allDifferent> 
         * 
         * @param id the id (name) of the constraint
         * @param matrix the matrix (not the scope, a variable may appear at different place!)
         */
        virtual void buildConstraintAlldifferentMatrix(string id, vector<vector<XVariable*>>& matrix) {
            throw runtime_error("AllDiff matrix constraint is not yet supported");
        }


        /**
         * The callback function related to a allequal constraint
         * See http://xcsp.org/specifications/allEqual
         * 
         * Example: 
         * <allEqual> 
         *      x1 x2 x3 x4 x5
         * </allEqual> 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         */
        virtual void buildConstraintAllEqual(string id, vector<XVariable*>& list) {
            throw runtime_error("Allequal constraint is not yet supported");
        }


        /**
         * The callback function related to a not all equal constraint
         * This is a special case of nvalues constraint
         * Recognized if #recognizeNValuesCases is enabled (this is the case by default)
         * See http://xcsp.org/specifications/nValues
         * 
         * Example: 
         * <nValues id="c1"> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <condition> (gt,1) </condition> 
         * </nValues> 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constrant
         */
        virtual void buildConstraintNotAllEqual(string id, vector<XVariable*>& list) {
            throw runtime_error("NotAllequal constraint is not yet supported");
        }


        /**
         * The callback function related to an ordered constraint
         * See http://xcsp.org/specifications/ordered
         * 
         * Ordered is LE, LT, GE, GT... see OrderType in XCSPConstraints.h 
         * 
         * Example: 
         * <ordered> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <operator> lt </operator> 
         * </ordered> 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param order the order LT, LE...
         */
        virtual void buildConstraintOrdered(string id, vector<XVariable*>& list, OrderType order) {
            throw runtime_error("Ordered constraint is not yet supported");
        }


        /**
         * The callback function related to an ordered constraint
         * See http://xcsp.org/specifications/ordered
         * 
         * Ordered is LE, LT, GE, GT... See OrderType in XCSPConstraints.h
         * 
         * Example: 
         * <ordered> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <operator> lt </operator> 
         * </ordered> 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param order the order LT, LE... 
         */
        virtual void buildConstraintOrdered(string id, vector<XVariable*>& list, vector<int>& lengths, OrderType order) {
            throw runtime_error("Ordered constraint wirt lenghts is not yet supported");
        }


        /**
         * The callback function related to an ordered list constraint (this is a lex constraint)
         * See http://xcsp.org/specifications/ordered
         * 
         * Example: 
         * <ordered> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <list> y1 y2 y3 y4 </list> 
         *      <operator> lt </operator> 
         * </ordered> 
         * 
         * @param id the id (name) of the constraint
         * @param lists the set of lists (not the scope, a variable may appear at different place!)
         * @param order the order LT, LE... 
         */
        virtual void buildConstraintLex(string id, vector<vector<XVariable*>>& lists, OrderType order) {
            throw runtime_error("Lex constraint is not yet supported");
        }


        /**
         * The callback function related to an ordered matrix constraint
         * See http://xcsp.org/specifications/ordered
         * 
         * Example: 
         * <ordered> 
         *      <matrix> 
         *          (x1,x2,x3)
         *          (y1,y2,y3)
         *          (z1,z2,z3)
         *      </matrix> 
         *      <operator> lt </operator> 
         * </ordered> 
         * 
         * @param id the id (name) of the constraint
         * @param matrix the matrix (not the scope, a variabl may appear at different place!)
         * @param order the order LT, LE... 
         */
        virtual void buildConstraintLexMatrix(string id, vector<vector<XVariable*>>& matrix, OrderType order) {
            throw runtime_error("Lex matrix constraint is not yet supported");
        }


        // ##########################################################
        // Summing and counting constraints
        // ##########################################################


        /**
         * The callback function related to an sum constraint
         * See http://xcsp.org/specifications/sum
         * 
         * XCondition is an obj with on OrderType (LE, LT...) an operandType (INTEGER, INTERVAL or VARIABLE)
         * depending the val of this field, either val (if operandType is INTEGER), main/max (INTERVAL) or var (VARIABLE)
         * is useful. 
         * 
         * 
         * Example: 
         * <sum> 
         *      <list> x1 x2 x3 </list> 
         *      <coeffs> 1 2 3 </coeffs> 
         *      condition> (gt,y) </condition> 
         * </sum> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param coeffs the coefficients (here int)
         * @param cond the condition (See XCondition obj)
         */
        virtual void buildConstraintSum(string id, vector<XVariable*>& list, vector<int>& coeffs, XCondition& cond) {
            throw runtime_error("sum constraint is not yet supported");
        }

        /**
         * The callback function related to an sum constraint with all coeffs are equal to one
         * See http://xcsp.org/specifications/sum
         * 
         * 
         * Example: 
         * <sum> 
         *      <list> x1 x2 x3 </list> 
         *      <condition> (gt,y) </condition> 
         * </sum> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param cond the condition (See XCondition obj)
         */
        virtual void buildConstraintSum(string id, vector<XVariable*>& list, XCondition& cond) {
            throw runtime_error("unweighted sum constraint is not yet supported");
        }


        /**
         * The callback function related to a sum constraint with trees in list
         * 
         * 
         * Example: 
         * <sum> 
         *      <list>or(eq(x[5],0),eq(x[7],0)) or(eq(x[1],0),eq(x[2],0),eq(x[8],0)) or(eq(x[0],0),eq(x[3],0),eq(x[4],0),eq(x[6],0),eq(x[9],0))</list>
         *      <condition> (gt,y) </condition> 
         * </sum>  
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the different trees
         * @param cond the condition (See XCondition obj)
         */
        virtual void buildConstraintSum(string id, vector<Tree*>& trees, XCondition& cond) {
            throw runtime_error("sum constraint with expressions not yet supported");
        }


        /**
         * The callback function related to a sum constraint with trees in list
         * 
         * 
         * Example: 
         * <sum> 
         *      <list>or(eq(x[5],0),eq(x[7],0)) or(eq(x[1],0),eq(x[2],0),eq(x[8],0)) or(eq(x[0],0),eq(x[3],0),eq(x[4],0),eq(x[6],0),eq(x[9],0))</list>
         *      <coeffs>1 2 3</coeffs> 
         *      <condition> (gt,y) </condition> 
         * </sum> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the different trees
         * @param coeffs the coeffs. 
         * @param cond the condition (See XCondition obj)
         */
        virtual void buildConstraintSum(string id, vector<Tree*>& trees, vector<int>& coeffs, XCondition& cond) {
            throw runtime_error("sum constraint with expressions and coefs not yet supported");
        }


        /**
         * The callback function related to a atmost constraint
         * This is a special count constraint
         * This callback is called if #recognizeSpecialCountCases is enabled (this is the case by default)
         * See http://xcsp.org/specifications/count
         * 
         * 
         * Example: 
         * <count id="c4"> 
         *      <list> y1 y2 y3 y4 </list> 
         *      <values> 0 </values> 
         *      <condition> (le,2) </condition> 
         * </count> 
         * 
         * Here at most 2 variables from y1...y4 can have value 0
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param value the value
         * @param k the maximum num of variables that can take the value
         */
        virtual void buildConstraintAtMost(string id, vector<XVariable*>& list, int value, int k) {
            throw runtime_error("atmost constraint is not yet supported");
        }


        /**
         * The callback function related to a atleast constraint
         * Ths is a special count constraint
         * This callback is called if #recognizeSpecialCountCases is enabled
         * See http://xcsp.org/specifications/count
         * 
         * 
         * Example: 
         * <count id="c3"> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <values> 1 </values> 
         *      <condition> (ge,3) </condition> 
         * </count> 
         * 
         * Here at least 3 variables from x1...x5 must have value 1
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param value the value
         * @param k the minimum num of variables that can take the value
         */
        virtual void buildConstraintAtLeast(string id, vector<XVariable*>& list, int value, int k) {
            throw runtime_error("atleast constraint is not yet supported");
        }


        /**
         * The callback function related to a exactly k constraint
         * THis is a special count counstraint
         * This callback is called if #recognizeSpecialCountCases is enabled
         * See http://xcsp.org/specifications/count
         * 
         * 
         * Example: 
         * <count id="c5"> 
         *      <list> z1 z2 z3 </list> 
         *      <values> 0 </values> 
         *      <condition> (eq,2) </condition> 
         * </count> 
         * 
         * Here exactly 2 variables from z1...z4 must have value 0
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param value the value
         * @param k the exact num of variables that can take the value
         */
        virtual void buildConstraintExactlyK(string id, vector<XVariable*>& list, int value, int k) {
            throw runtime_error("exactly K constraint is not yet supported");
        }


        /**
         * The callback function related to a exactly K variable constraint
         * THis is a special count constraint
         * This callback is called if #recognizeSpecialCountCases is enabled
         * See http://xcsp.org/specifications/count
         * 
         * 
         * Example: 
         * <count id="c5"> <!-- exactly --> 
         *      <list> z1 z2 z3 z4 </list> 
         *      <values> 0 </values> 
         *      <condition> (eq,z5) </condition> 
         * </count> 
         * 
         * 
         * Here exactly z5 variables from z1...z4 must have value 0
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param value the value
         * @param x the exact num of variables that can take the value (here it is a vairable)
         */
        virtual void buildConstraintExactlyVariable(string id, vector<XVariable*>& list, int value, XVariable* x) {
            throw runtime_error("exactly Variable constraint is not yet supported");
        }


        /**
         * The callback function related to a among constraint
         * This is a special count constraint
         * This callback is called if #recognizeSpecialCountCases is enabled
         * See http://xcsp.org/specification/count
         * 
         * 
         * Example: 
         * <count id="c2"> 
         *      <list> w1 w2 w3 w4 </list> 
         *      <values> 1 5 8 </list> 
         *      <condition> (eq, k2) </condition> 
         * </count> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param value the value
         * @param k
         */
        virtual void buildConstraintAmong(string id, vector<XVariable*>& list, vector<int>& values, int k) {
            // TODO AMONG
            throw runtime_error("Among constraint is not yet supported");
        }


        /**
         * The callback function related to a count constraint with integer values
         * See http://xcsp.org/specifications/count
         * 
         * 
         * Example: 
         * <count id="c1">
         *      <list> v1 v2 v3 v4 </list> 
         *      <values> 2 4 </values> 
         *      <condition> (ne,k1) </condition> 
         * </count> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param value the set of values (here set of ints)
         * @param k the num of variables
         * @param xc the condition (see #XCondition)
         */
        virtual void buildConstraintCount(string id, vector<XVariable*>& list, vector<int>& values, XCondition& xc) {
            throw runtime_error("count with integer values constraint is not yet supported");
        }


        /**
         * The callback function related to a count constraint with integer variables
         * See http://xcsp.org/specifications/count
         * 
         * 
         * Example: 
         * <count id="c1"> 
         *      <list> v1 v2 v3 v4 </list> 
         *      <values> x1 x2 </values> 
         *      <condition> (ne,k1) </condition> 
         * </count> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param value the set of values (here set of variables)
         * @param k the num of vairables
         * @param xc the condition (see #XCondition)
         */
        virtual void buildConstraintCount(string id, vector<XVariable*>& list, vector<XVariable*>& values, XCondition& xc) {
            throw runtime_error("count with variables values constraint is not yet supported");
        }


        /**
         * The callback function related to a nValues constraint with exception 
         * See http://xcsp.org/specifications/nValues
         * 
         * 
         * Example: 
         * <nValues id="c3"> 
         *      <list> z1 z2 z3 z4 </list> 
         *      <except> 0 </except> 
         *      <condition> (eq,2) </condition> 
         * </nValues> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param except the set of excepted values
         * @param xc the condition (see #XCondition)
         */
        virtual void buildConstraintNValues(string id, vector<XVariable*>& list, vector<int>& except, XCondition& xc) {
            throw runtime_error("NValues with exception constraint is not yet supported");
        }


        /**
         * The callback function related to a nValues constraint with exception
         * See http://xcsp.org/specifications/nValues
         * 
         * 
         * Example: 
         * <nValues id="c3"> 
         *      <list> z1 z2 z3 z4 </list> 
         *      <condition> (eq,2) </condition> 
         * </nValues> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param xc the condition (see #XCondition)
         */
        virtual void buildConstraintNValues(string id, vector<XVariable*>& list, XCondition& xc) {
            throw runtime_error("NValues constraint is not yet supported");
        }


        /**
         * The callback function related to a cardinality constraint with int values and int occurs
         * See http://xcsp.org/specifications/cardinality
         * 
         * 
         * Example: 
         * <cardinality> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <values> 2 5 10 </values> 
         *      <occurs> 1 2 3 </occurs> 
         * </cardinality> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param values the set of values (here int)
         * @param occurs the num of occurences (here int)
         * @param clised is the constraint is closed
         */
        virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<int> values, vector<int>& occurs, bool closed) {
            throw runtime_error("cardinality with int values and int occurs constraint is not yet supported");
        }


        /**
         * The callback function related to a cardinality constraint with int values and variable occurs
         * See http://xcsp.org/specifications/cardinality
         * 
         * 
         * Exaple:
         * <cardinality> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <values> 0 1 2 3 </values>
         *      <occurs> z0 z1 z2 z3 </occurs>
         * </cardinality> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param values the set of values (here int)
         * @param occurs the num of occurences (here variables)
         * @param closed is the constraint is closed
         */
        virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<int> values, vector<XVariable*>& occurs, bool closed) {
            throw runtime_error("cardinality with int values and int occurs constraint is not yet supported");
        }


        /**
         * The callback function related to a cardinality constraint with int values and interval occurs
         * See http://xcsp.org/specifications/cardinality
         * 
         * 
         * Example: 
         * <cardinality> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <values> 2 5 10 </values> 
         *      <occurs> 0..1 1..3 2..3 </occurs> 
         * </cardinality> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the scope of the constraint
         * @param values the set of values (here int)
         * @param occurs the num of occurences (here intervals)
         * @param closed is the constraint is closed
         */
        virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<int> values, vector<XInterval>& occurs, bool closed) {
            throw runtime_error("cardinality with int values and int occurs constraint is not yet supported");
        }


        /**
         * The callback function related to a cardinality constraint with variable values and int occurs
         * See http://xcsp.org/specifications/cardinality
         * 
         * 
         * Example: 
         * <cardinality> 
         *      <list> x1 x2 x3 x4 </list> 
         *      <values> z1 z2 z3 </values> 
         *      <occurs> 1 2 3 </occurs> 
         * </cardinality> 
         * 
         * 
         * @param id the id (name) of the constraint
         * @param list the list of the constraint (not the scope...)
         * @param values the set of values (here variable)
         * @param occurs the num of occurences (here int)
         * @param closed is the constraint is closed
         */
        virtual void buildConstraintCardinality(string id, vector<XVariable*>& list, vector<XVariable*> values, vector<int>& occurs, bool closed) {
            throw runtime_error("cardinality with int values and int occurs constraint is not yet supported");
        }


        // 1149