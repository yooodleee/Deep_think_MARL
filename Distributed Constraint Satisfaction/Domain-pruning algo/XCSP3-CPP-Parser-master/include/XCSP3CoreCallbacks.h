
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

        // 527
    }
}