
#ifndef XCONSTRAINT_H_
#define XCONSTRAINT_H_


#include "XCSP3Variable.h"
#include "XCSP3utils.h"
#include "XCSP3Constants.h"


#include <typeinfo>
#include <regex>
#include <map>


namespace XCSP3Core {

    using namespace std;

    class XConstraintGroup;

    class XConstraintIntension;

    /****************************************************************
     * A constraint XValue
    ****************************************************************/
    class XConstraint {
    public:
        std::string id;
        vector<XVariable *> list;
        string classes;

        XConstraint(std::string dd, std::string c) : id(dd), classes(c) {}

        virtual ~XConstraint() {}

        virtual void unfoldParameters(XConstraintGroup *group, vector<XVariable *> &arguments, XConstraint *original);
   };


   /*****************************************************************
    * A constraint XValue
    ****************************************************************/
    class XConstraint {
    public:
        std::string id;
        vector<XVariable*> list;
        string classes;

        XConstraint(std::string dd, std::string c) : id(dd), classes(c) {}
        
        virtual ~XConstraint() {}

        virtual void unfoldParameters(XConstraintGroup *group, vector<XVariable*>& arguments, XConstraint *original);
   };


   /*****************************************************************
    * A constraint group
    * Only basic abstraction is implemented: All parameters occur in the same tag
    ****************************************************************/
    class XConstraintGroup : public XConstraint {
    public:
        XConstraint *Constraint;                    // A Constraint group contains a constraint
        vector<vector<XVariable*>> arguments;       // The list of all args
        ConstraintType type;                        // Use it to discover the type of constraint... and perform cast
        map<string, XVariable*> toArguments;


        XConstraintGroup(std::string idd, std::string c) : XConstraint(idd, c), constraint(NULL), type(UNKNOWN) {}

        virtual ~XConstraintGroup() { delete constraint; }

        void unfoldVector(vector<XVariable*>& toUnfold, vector<XVariable*>& args, vector<XVariable*>& initial);
        void unfoldString(string &toUnfold, vector<XVariable*>& args);
        void unfoldArgumentNumber(int, XConstraint *builtConstraint);
   };


   // All different structures related to XVariables used in constraints
   // This facilitates the unfolding of group constraint

    class XCondition {
    public:
        OrderType op;
        OperandType operandType;
        int val;
        int min, max;
        string var;

        friend ostream &operator<<(ostream &f, const XCSP3Core::XCondition &ie);
   };


    class XInitialCondition {
    public:
        string condition;

        void unfoldParameters(XConstraintGroup *group, vector<XVariable*>& arguments, XConstraint *original);
        void extractCondition(XCondition& xc);  // Create the op and the operand (which can be a value, an interval or a XVariable)
    };


    class XValues {
    public:
        vector<XVariable*> values;  // Only XVariable or XInteger

        void unfoldParameters(XConstraintGroup* group, vector<XVariable*>& arguments, XConstraint* original);
    };


    class XValue {
    public:
        XVariable* value;

        XValue() : value(0) {}

        void unfoldParameters(XConstraintGroup* group, vector<XVariable*>& arguments, XConstraint* original);
    };


    class XIndex {
    public:
        XVariable* index;

        XIndex() : index(0) {}

        void unfoldParameters(XConstraintGroup* group, vector<XVariable*>& arguments, XConstraint* original);
    };


    class XLength {
    public:
        vector<XVariable*> lengths;

        void unfoldParameters(XConstraintGroup* group, vector<XVariable*>& arguments, XConstraint* original);
    };


    /****************************************************************
     * constraint extension
    ****************************************************************/
    class XConstraintExtension : public XConstraint {
    public:
        vector<vector<int>> tuples;
        bool isSupport;
        bool containStar;

        XConstraintExtension(std::string idd, std::string c) : XConstraint(idd, c), containStar(false) {}

        void unfoldParameters(XConstraintGroup* group, vector<XVariable*>& arguments, XConstraint* original) override;
    };


    /****************************************************************
     * constraint intension
    ****************************************************************/
    class XConstraintIntension : public XConstraint {
    public:
        string function;

        XConstraintIntension(std::string idd, std::string c) : XConstraint(idd, c) {}

        void unfoldParameters(XConstraintGroup* group, vector<XVariable*>& arguments, XConstraint* original) override;
    };



    /****************************************************************
     ****************************************************************
     *                CONSTRAINTS DEFINED ON LANGUAGES              *
     ****************************************************************
     ***************************************************************/


    /****************************************************************
     * constraint regular
     ***************************************************************/
    class XTransition {
    public:
        XTransition(string f, int v, string t) : from(f), val(v), to(t) {}

        string from;
        int val;
        string to;
    };

    static vector<XTransition> tr;  // Not beautiful but remove code to fixed data in group constraint.
    static string st;
    static vector<string> fi;


    class XConstraintRegular : public XConstraint {
    public:
        string &start;
        vector<string> &final;
        vector<XTransition>& transitions;

        XConstraintRegular(std::string idd, std::string c) : XConstraint(idd, c), start(st), final(fi), transitions(tr) {}
    };


    /****************************************************************
     * constraint mdd
     ***************************************************************/
    class XConstraintMDD : public XConstraint {
    public:
        vector<XTransition>& transitions;

        XConstraintMDD(std::string idd, std::string c) : XConstraint(idd, c), transitions(tr) {}
    };


    /****************************************************************
     ****************************************************************
     *              COMPARISON BASED CONSTRAINTS                    *
     ****************************************************************
     ***************************************************************/
    static vector<int> _except;

    class XConstraintAllDiff : public XConstraint {
    public:
        vector<int>& except;

        XConstraintAllDiff(std::string idd, std::string c) : XConstraint(idd, c), except(_except) {}
    };


    
}