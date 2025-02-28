
#ifndef COSOCO_XMANAGER_H_
#define COSOCO_XMANAGER_H_


#include <XCSP3CoreCallbacks.h>
#include "XCSP3Constants.h"
#include "XCSP3Variable.h"
#include "XCSP3Constraint.h"
#include "XCSP3Objective.h"


#include <string>
#include <regex>
#include <map>


namespace XCSP3Core {

    class PrimitivePattern;
    class XCSP3Manager {
    
    public:
        XCSP3CoreCallbacks& callback;
        std::map<std::string, XEntity*>& mapping;
        std::string blockClasses;

        inline bool discardedClasses(string classes) {
            return callback->discardedClasses(blockClasses) || callback->discardedClasses(classes);
        }

    private:
        std::vector<XCSP3Core::PrimitivePattern*> patterns;
        bool recognizePrimitives(std::string id, Tree* tree);
        void createPrimitivePatterns();
        void destroyPrimitivePatterns();

    public:
        // XCSP3CoreCallbacks* c, std::map<std::string, XEntity*>& m, bool
        XCSP3Manager(XCSP3CoreCallbacks* c, std::map<std::string, XEntity*>& m, bool = true) : callback(c), mapping(m), blockClasses("") {}

        void beginInstance(InstanceType type) {
            callback->beginInstance(type);
        }
        void endInstance() {
            callback->endInstance();
        }
        void beginVariables() {
            callback->beginVariables();
        }
        void endVariables() {
            callback->endVariables();
        }
        
        void buildVariable(XVariable* variable);

        void beginVariableArray(std::string id) {
            callback->beginVariableArray(id);
        }
        void endVariableArray() {
            callback->endVariableArray();
        }
        
        void buildVariableArray(XVariableArray* variable);

        void beginConstraints() {
            if (callback->recognizeSpecialIntensionCases)
                createPrimitivePatterns();
            callback->beginConstraints();
        }
        void endConstraints() {
            callback->endConstraints();
            if (callback->recognizeSpecialIntensionCases)
                destroyPrimitivePatterns();
        }

        void beginSlide(std::string id, bool circular) {
            callback->beginSlide(id, circular);
        }
        void endSlide() {
            callback->endSlide();
        }

        void beginObjectives() {
            callback->beginObjectives();
        }
        void endObjectives() {
            callback->endObjectives();
        }

        void beginAnnotations() {
            callback->beginAnnotations();
        }
        void endAnnotations() {
            callback->endAnnotations();
        }


        // ##########################################################
        // Basic Constraints
        // ##########################################################


        void newConstraintExtension(XConstraintExtension* constraint);

        void newConstraintExtensionAsLastOne(XConstraintExtension* constraint);

        void newConstraintIntension(XConstraintIntension* constraint);


        // ##########################################################
        // Languages Constraints
        // ##########################################################


        void newConstraintRegular(XConstraintRegular* constraint);

        void newConstraintMDD(XConstraintMDD* constraint);


        // ##########################################################
        // Comparison Constraints
        // ##########################################################


        void newConstraintAllDiff(XConstraintAllDiff* constraint);

        void newConstraintAllDiffMatrix(XConstraintAllDiffMatrix* constraint);

        void newConstraintAllDiffList(XConstraintAllDiffList* constraint);

        void newConsraintAllEqual(XConstraintAllEqual* constraint);

        void newConstraintOrdered(XConstraintOrdered* constraint);

        void newConstraintLex(XConstraintLex* constraint);

        void newConstraintLexMatrix(XConstraintLexMatrix* constraint);


        // ##########################################################
        // Summin and Counting Constraints
        // ##########################################################
    protected:
        void normalizeSum(vector<XVariable*>& list, vector<int>& coeffs);


    public:
        void newConstraintSum(XConstraintSum* constraint);

        void newConstraintCount(XConstraintCount* constraint);

        void newConstraintNValues(XConstraintNValues* constraint);

        void newConstraintCardinality(XConstraintCardinality* constraint);


        // ##########################################################
        // Connection Constraints
        // ##########################################################


        void newConstraintMinimum(XConstraintMinimum* constraint);

        void newConstraintMaximum(XConstraintMaximum* constraint);

        void newConstraintElement(XConstraintElement* constraint);

        void newConstraintChannel(XConstraintChannel* constraint);


        // ##########################################################
        // Packing and Scheduling Constraints
        // ##########################################################


        void newConstraintStretch(XConstraintStretch* constraint);

        void newConstraintNoOverlap(XConstraintNoOverlap* constraint);

        void newConstraintNoOverlapKDim(XConstraintNoOverlap* constraint);

        void newConstraintCumulative(XConstraintCumulative* constraint);


        // ##########################################################
        // Instantiation Constraint
        // ##########################################################


        void newConstraintInstantiation(XConstraintInstantiation* constraint);


        // ##########################################################
        // Graph Constraints
        // ##########################################################


        void newConstraintCircuit(XConstraintCircuit* constraint);


        // ##########################################################
        // Block of Constraints
        // ##########################################################


        void beginBlock(string classes) {
            blockClasses = classes;
            callback->beginBlock(classes);
        }

        void endBlock() {
            blockClasses = "";
            callback->endBlock();
        }


        // ##########################################################
        // Group Constraints
        // ##########################################################


        void beginGroup(std::string id) {
            callback->beginGroup(id);
        }

        void endGroup() {
            callback->endGroup();
        }

        template<class T> void unfoldConstraint(XConstraintGroup* group, int i, void(XCSP3Manager::&newConstraint)(T* ));

        void newConstraintGroup(XConstraintGroup* group);


        // ##########################################################
        // Objective Constraints
        // ##########################################################


        void addObjective(XObjective* objective);


        // ##########################################################
        // Annotation : Decision Variables
        // ##########################################################


        void buildAnnotationDecision(vector<XVariable*>& list) {
            callback->buildAnnotationDecision(list);
        }
    };
}



#endif  // COSOCO_XMANAGER_H_