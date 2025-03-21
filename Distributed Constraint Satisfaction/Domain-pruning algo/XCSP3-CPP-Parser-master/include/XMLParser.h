
#ifndef _XMLParser_h_
#define _XMLParser_h_


/**
 * @file XMLParser.h 
 * @brief defines the XMLParser class to parse a CSP instance in XML format
 */


#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <deque>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <cstring>


#include "XCSP3Constants.h"
#include "XCSP3Domain.h"
#include "XCSP3Variable.h"
#include "XCSP3Constraint.h"
#include "XCSP3utils.h"
#include "XCSP3Objective.h"
#include "XCSP3Manager.h"


#include "UTF8String.h"
#include "AttributeList.h"


/**
 * @namespace CSPXMLParser
 * @brief this namespace encloses all definitions relative to the
 * CSP XML format parser. 
 */


namespace XCSP3Core {

    using namespace std;

    /**
     * @brief contains a parser for the CSP XML format. 
     * 
     * This class proposes a parser in SAX mode (to use less memory than
     * in DOM mode)
     * 
     * 
     * 
     */
    class XMLParser {
    public:
        // list of attributes and values for a tag
        map<string, XEntity*> variablesList;
        vector<XDomainInteger*> allDomains;
        vector<XConstraint*> constraints;
        XCSP3Manager* manager;


        // stack of operands to construct list, dictionaries, predicate
        // parameters and so on

    private:

        /************************************************************
         * @brief the action to be performed (callback) when a tag is 
         * read in the XML file
         ***********************************************************/
        class TagAction {
        protected:
            bool activated;
            XMLParser* parser;
            string tagName;

        public:
            TagAction(XMLParser* parser, string name) : parser(parser), tagName(name) { activated = false; }

            virtual ~TagAction() { }
            virtual activate() { activated = true; }
            virtual deactivate() { activated = false; }
            
            bool isActivated() { return activated; }


            /**
             * return the name of this tag (constant)
             */
            const char *getTagName() { return tagName.c_str(); }


            // AttributeList & attributes
            virtual void beginTag(const AttributeList&) { }


            /**
             * 
             * @param last must be true if this is the last chunk of text
             */
            // UTF8String txt, bool last
            virtual void text(const UTF8String txt, bool) {
                if(!txt.isWhiteSpace())
                    throw runtime_error("<" + tagName + "> tag should not have meaninful text");
            }

            virtual void endTag() { }


        protected:

            /**
             * Check that the parent tag in the XML file has the indicated name
             * 
             * when name is NULL, check that this tag has no parent in the XMl file
             * 
             * @param parentTag: the expected name of the parent tag or NULL to
             * check that we have no parent
             * @param n: 1 for parent, 2 for grand-parent and so on
             */
            void checkParentTag(const char* parentTag, int n = 1) {
                if (this->parser->getParentTagAction(n) == NULL) {
                    if (parentTag != NULL) 
                        throw runtime_error("tag has no parent but it should have one");
                    else
                        return;
                }

                if (parentTag == NULL)
                    throw runtime_error("tag should have no parent");

#ifdef debug
                cout << "parent tag="
                     << this->parser->getParentTagAction(n)->getTagName() << endl;
#endif
                if (strcmp(this->parser->getParentTagAction(n)->getTagName(),
                            parentTag) != 0)
                    throw runtime_error("wrong parent for tag");
            }

        };

        typedef map<UTF8String, TagAction*> TagActionList;

        TagActionList tagList;

        struct State {
            bool subtagAllowed;

            State() {
                subtagAllowed = true;
            }
        };

        class ListTagAction;

        // the top of the following stacks is at the FRONT
        deque<TagAction*> actionStack;
        deque<State> stateStack;
        vector<vector<XVariable*>> lists;   // used to store many lists of variables (usefull with lex, channel...)
        vector<vector<XVariable*>> matrix;  // Used in case of matrix tag
        vector<vector<int>> patterns;

        vector<XVariable*> args;    // used to store a list of args
        vector<XVariable*> values;  // used to store a list of variables
        vector<XVariable*> occurs;  // used in cardinality

        vector<int> integers;   // used to store a list of coefficients

        vector<XVariable*> origins; // used to store a origins in noOverlap/cumulative Constraint
        vector<XVariable*> lengths; // used to store lengths in noOverlap/cumulative Constraint
        vector<XVariable*> ends;    // used to store a origins in cumulative Constraint
        vector<XVariable*> heights; // used to store a origins in cumulative Constraint
        vector<XIntegerEntity*> widths; // used to store lengths in stretch constraint

        vector<int> currentTuple;
        ListTagAction* listTag; // The List tag action call

        string classes;

        bool zeroIgnored;   // for nooverlap
        string condition;   // used to store a condition in prefix form
        bool star;          // true if the extension contain star in tuples
        int startIndex, startIndex2;    // used in some list tag
        XVariable* index;   // used with tag index
        RankType rank;      // used with rank tag
        OrderType op;
        string expr;
        string start, final;    // used in regular constraint
        vector<XTransition> transitions;    // used in regular and mdd constraints
        int nbParameters;
        bool closed;
        vector<XEntity*> toFree;
        vector<XIntegerEntity*> toFreeEntity;
        bool keepIntervals;
        
        void registerTagAction(TagActionList& tagList, TagAction& action) {
            tagList[action->getTagName()] = action;
        }


        /**
         * Parse a sequence of tokens. Each token can represent a compact list of array 
         * variables, or a basic entity, or a template parameter
         */
        
        void parseSequence(const UTF8String& txt, vector<XVariable*>& list, vector<char> delimiters = vector<char>());

        void parseDomain(const UTF8String& txt, XDomainInteger& domain);

        void parseListOfIntegerOrInterval(const UTF8String& txt, vector<XIntegerEntity*>& listToFill);

        bool parseTuples(const UTF8String& txt, vector<vector<int>>& tuples);


        /************************************************************
         * a handler to silently ignore unknown tags
         ***********************************************************/
        class UnknownTagAction : public TagAction {
        public:
            UnknownTagAction(XMLParser* parser, string name) : TagAction(parser, name) { }

            // AttributeList& attributes
            virtual void beginTag(const AttributeList&) {
                throw runtime_error("Unknown Tag");
            }
        };


        /************************************************************
         * Actions performed on INSTANCE tag
         ***********************************************************/
        class InstanceTagAction : public TagAction {
        public:
            InstanceTagAction(XMLParser* parser, string name) : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on VARIABLES tag
         ***********************************************************/
        class VariablesTagAction : public TagAction {
        public:
            VariablesTagAction(XMLParser* parser, string name) : TagAction(parser, name) { }
            virtual void beginTag(const AttributeList& attributes) override;
            virtual void endTag() override;
        };


        /************************************************************
         * Actions performed on VAR tag
         ***********************************************************/
        class VarTagAction : public TagAction {
        private:
            XVariable* variable;
            XVariableArray* variableArray;
            XDomainInteger* domain;
            string id, classes;

        public:
            VarTagAction(XMLParser* parser, string name) 
                : TagAction(parser, name)
                , variable(NULL)
                , variableArray(NULL)
                , domain(NULL) { }
            void beginTag(const AttributeList* attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on ARRAY tag
         ***********************************************************/
        class ArrayTagAction : public TagAction {
        public:
            XVariableArray* varArray;
            XDomainInteger* domain;
            string id, classes;
            vector<int> sizes;


            ArrayTagAction(XMLParser* parser, string name)
                : TagAction(parser, name)
                , varArray(NULL)
                , domain(NULL) { }
            
            void beginTag(const AttributeList* attributes) override;
            void endTag() override;


            // UTF8String txt, bool last
            void text(const UTF8String txt, bool) override {
                this->parser->parseDomain(txt, *domain);
            }
        };

        class DomainTagAction : public TagAction {
        public:
            XDomainInteger* d;
            string forAttr;

            DomainTagAction(XMLParser* parser, string name) : TagAction(parser, name) { }

            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on CONSTRAINTS tag
         ***********************************************************/
        class ConstraintsTagAction : public TagAction {
        public:
            COnstraintsTagAction(XMLParser* parser, string name) : TagAction(parser, name) { }

            // AttributeList& attributes
            virtual void beginTag(const AttributeList&) {
                this->checkParentTag("instance");
                this->parser->manager->beginConstraints();
            }

            virtual void endTag() {
                this->parser->manager->endConstraints();
            }
        };


        /************************************************************
         * All constraint MUST inherit this tagAction
         ***********************************************************/
        class BasicConstraintTagAction : public TagAction {
        public:
            XConstraintGroup* group;
            string id;
            BasicConstraintTagAction(XMLParser* parser, string name) : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
        };


        /************************************************************
         * Actions performed on ExTENSION tag
         ***********************************************************/
        class ExtensionTagAction : public BasicConstraintTagAction {
        public:
            XConstraintExtension* constraint;
            ExtensionTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on INTENSION tag
         ***********************************************************/
        class IntensionTagAction : public BasicConstraintTagAction {
        protected:
            UTF8String fnc;

        public:
            XConstraintIntension* constraint;
            IntensionTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         ************************************************************
         *            CONSTRAINTS DEFINED ON LANGAUGE
         ************************************************************
         ***********************************************************/


        /************************************************************
         * Actions performed on REGULAR tag
         ***********************************************************/
        class RegularTagAction : public BasicConstraintTagAction {
        public:
            XConstraintRegular* constraint;
            RegularTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on MDD tag
         ***********************************************************/
        class MDDTagAction : public BasicConstraintTagAction {
        public:
            XConstraintMDD* constraint;
            MDDTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         ************************************************************
         *            CONSTRAINTS DEFINED ON COMPARISON
         ************************************************************
         ***********************************************************/


        /************************************************************
         * Actions performed on ALLDIFF tag / ALLEQUAL
         ***********************************************************/
        class AllDiffEqualTagAction : public BasicConstraintTagAction {
        public:
            XConstraintAllDiff* alldiff;
            XConstraintAllEqual* allequal;
            XConstraint* ct;
            AllDiffEqualTagAction(XMLParser* parser, string tag)
                : BasicConstraintTagAction(parser, tag) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on ORDERED tag
         ***********************************************************/
        class OrderedTagAction : public BasicConstraintTagAction {
        public:
            XConstraintOrdered* constraint;
            OrderedTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on LEX tag
         ***********************************************************/
        class LexTagAction : public BasicConstraintTagAction {
        public:
            XConstraintLex* constraint;
            LexTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         ************************************************************
         *            COUNTING AND SUMMING CONSTRAINTS
         ************************************************************
         ***********************************************************/


        /************************************************************
         * Actions performed on SUM tag
         ***********************************************************/
        class SumTagAction : public BasicConstraintTagAction {
        public:
            XConstraintSum* constraint;
            SumTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on NVALUES tag
         ***********************************************************/
        class NValuesTagAction : public BasicConstraintTagAction {
        public:
            XConstraintNValues* constraint;
            NValuesTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on COUNT tag
         ***********************************************************/
        class CountTagAction : public BasicConstraintTagAction {
        public:
            XConstraintCount* constraint;
            CountTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on CARDINALITY tag
         ***********************************************************/
        class CardinalityTagAction : public BasicConstraintTagAction {
        public:
            XConstraintCardinality* constraint;
            CardinalityTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         ************************************************************
         *                  CONNECTION CONSTRAINTS
         ************************************************************
         ***********************************************************/


        /************************************************************
         * Actions performed on CHANNEL tag
         ***********************************************************/
        class ChannelTagAction : public BasicConstraintTagAction {
        public:
            XConstraintChannel* constraint;
            ChannelTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on ELEMENT tag
         ***********************************************************/
        class ElementTagAction : public BasicConstraintTagAction {
        public:
            XConstraintElement* constraint;
            ElementTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on MAXIMUM or MINIMUM tag
         ***********************************************************/
        class MinMaxTagAction : public BasicConstraintTagAction {
        public:
            XConstraintMaximum* constraint;
            MinMaxTagAction(XMLParser* parser, string tag)
                : BasicConstraintTagAction(parser, tag) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         ************************************************************
         *           PACKING and SCHEDULING CONSTRAINTS
         ************************************************************
         ***********************************************************/


        /************************************************************
         * Actions performed on STRETCH tag
         ***********************************************************/
        class StretchTagAction : public BasicConstraintTagAction {
        public:
            XConstraintStretch* constraint;
            StretchTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on NOOVERLAP tag
         ***********************************************************/
        class NoOverlapTagAction : public BasicConstraintTagAction {
        public:
            bool diffn;
            XConstraintNoOverlap* constraint;
            NoOverlapTagAction(XMLParser* parser, string name) 
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on CUMULATIVE tag
         ***********************************************************/
        class CumulativeTagAction : public BasicConstraintTagAction {
        public:
            XConstraintCumulative* constraint;
            CumulativeTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         ************************************************************
         *                      OBJECTIVES
         ************************************************************
         ***********************************************************/
        
        class ObjectivesTagAction : public TagAction {
        public:
            XObjective* objective;
            ObjectivesTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        class MinimizeOrMaximizeTagAction : public TagAction {
        public:
            MinimizeOrMaximizeTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            XObjective* obj;
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on instantiation tag
         ***********************************************************/
        class InstantiationTagAction : public BasicConstraintTagAction {
        public:
            InstantiationTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            XConstraintInstantiation* constraint;
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on LIST tag
         ***********************************************************/
        class ListTagAction : public TagAction {
        public:
            int nbCallsToList;
            // The first call stores datas inside this->parser->list
            // the ith call (i>1) stores datas inside this->parser->manyLists[i-1];
            ListTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on SUPPORTS CONFLICTS tag
         ***********************************************************/
        class ConflictOrSupportTagAction : public TagAction {
        protected:
        public:
            ConflictOrSupportTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on COEFFS tag
         ***********************************************************/
        class ConditionTagAction : public TagAction {
        public:
            ConditionTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            
            // UTF8String txt, bool last
            virtual void text(const UTF8String txt, bool) {
                this->parser->condition += txt.to(this->parser->condition);
            }

            virtual void endTag() {
                this->parser->condition = trim(this->parser->condition);
            }
        };


        /************************************************************
         * Actions performed on CONDITION tag
         ***********************************************************/
        class ListOfIntegerTagAction : public TagAction {
        public:
            ListOfIntegerTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            virtual void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on VALUES / VALUE tag
         ***********************************************************/
        class ListOfVariablesOrIntegerTagAction : public TagAction {
            
            vector<XVariable*>& listToFill;
        public:
            ListOfVariablesOrIntegerTagAction(XMLParser* parser, string nm, vector,XVariable*>& ltf)
                : TagAction(parser, nm)
                , listToFill(ltf) { }
            virtual void beginTag(const AttributeList& attributes) override;
            virtual void text(const UTF8String txt, bool last) override;
        };

        class ListOfVariablesOrIntegerOrIntervalTagAction : public TagAction {
            
            vector<XVariable*>& listToFill;
        public:
            ListOfVariablesOrIntegerOrIntervalTagAction(XMLParser* parser, string nm, vector<XVariable*>& ltf)
                : TagAction(parser, nm)
                , listToFill(ltf) { }
            virtual void beginTag(const AttributeList& attributes) override;
            virtual void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on ORIGINS tag
         ************************************************************/
        class OriginsTagAction : public TagAction {

            vector<XVariable*>& listToFill;
        public:
            OriginsTagAction(XMLParser* parser, string nm, vector<XVariable*>& ltf)
                : TagAction(parser, nm)
                , listToFill(ltf) { }
            virtual void beginTag(const AttributeList& attributes) override;
            virtual void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on WIDTH tag
         ***********************************************************/
        class ListOfIntegerOrIntervalTagAction : public TagAction {
        
            vector<XIntegerEntity*>& listToFill;
        public:
            ListOfIntegerOrIntervalTagAction(XMLParser* parser, string nm, vector<XIntegerEntity*>& ltf)
                : TagAction(parser, nm)
                , listToFill(ltf) { }
            virtual void beginTag(const AttributeList& attributes) override;
            virtual void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on GROUP tag
         ***********************************************************/
        class GroupTagAction : public TagAction {
        public:
            XConstraintGroup* group;
            GroupTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on SLIDE tag
         ***********************************************************/
        class SlideTagAction : public TagAction {
        public:
            XConstraintGroup* group;
            vector<XVariable*> list;

            int offset = 1;
            bool circular = false;

            SlideTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on GROUP tag
         ***********************************************************/
        class BlockTagAction : public TagAction {
        public:
            BlockTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on ARGS tag
         ***********************************************************/
        class ArgsTagAction : public TagAction {
        public:
            ArgsTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            virtual void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on Operator tag
         ***********************************************************/
        class OperatorTagAction : public TagAction {
        public:
            OperatorTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on INDEX tag
         ***********************************************************/
        class IndexTagAction : public TagAction {
        public:
            IndexTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on MATRIX TAG
         ***********************************************************/
        class MatrixTagAction : public TagAction {
        public:
            MatrixTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on START or FINAL tag
         ***********************************************************/
        class StringTagAction : public TagAction {
        public:
            StringTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on TRANSITIONS tag
         ***********************************************************/
        class TransitionsTagAction : public TagAction {
        public:
            int nb, val;
            std::string from, to;
            TransitionsTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
        };


        /************************************************************
         * Actions performed on TRANSITIONS tag
         ***********************************************************/
        class PatternsTagAction : public TagAction {
        public:
            vector<XVariable*>& listToFill;
            PatternsTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
        };


        class CircuitTagAction : public BasicConstraintTagAction {
            XConstraintCircuit* constraint;
        public:
            CircuitTagAction(XMLParser* parser, string name)
                : BasicConstraintTagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


        /************************************************************
         * Actions performed on ANNOTATIONS TAG
         ***********************************************************/
        class AnnotationsTagAction : public TagAction {
        public:
            AnnotationsTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTag(const AttributeList& attributes) override;
            void endTag() override;
        };


        class DecisionTagAction : public TagAction {
            vector<XVariable*> list;
        public:
            DecisionTagAction(XMLParser* parser, string name)
                : TagAction(parser, name) { }
            void beginTap(const AttributeList& attributes) override;
            void text(const UTF8String txt, bool last) override;
            void endTag() override;
        };


    public:
        XMLParser(XCSP3CoreCallbacks* cb);
        ~XMLParser();


        /**
         * get the parent tag action that is n levels higher in the current
         * branch of the XML parse tree
         */
        TagAction* getParentTagAction(int n = 1) {
            if (n < 0 || n >= (int) actionStack.size())
                return NULL;
            
            return actionStack[n];
        }


        /**
         * callbacks from the XML parser
         */
        void startDocument() {
            clearStacks();
        }
        void endDocument() { }
        void startElement(UTF8String name, const AttributeList& attributes);
        void endElement(UTF8String name);
        void characters(UTF8String chars);
        void handleAbridgeNotation(UTF8String chars, bool lastChunk);


    protected:
        void clearStacks() {
            stateStack.clear();
        }

        /**
         * text which is left for the next call to characters()
         * because it may not be a complete token
         */
        UTF8String textLeft;

        // specific actions
        VarTagAction* varTagAction;

        // DictTagAction* dictTagAction;
        TagAction* unknownTagHandler;   // handler to help ignore all unknown tags


    };  // clear XMLParser

}   // namespace


// Local Variables;
// mode: C++
// End:


#endif  // _XMLParser_h_