
#include "XCSP3Manager.h"
#include "XCSP3Variable.h"
#include "XCSP3Constraint.h"
#include "XCSP3Constants.h"
#include "XCSP3Objective.h"
#include "XCSP3TreeNode.h"


#include <string>
#include <regex>
#include <map>


using namespace XCSP3Core;


static OrderType expressionTypeToOrderType(ExpressionType e)
{
    if(e == OLE) return LE;
    if(e == OLT) return LT;
    if(e == OGE) return GE;
    if(e == OGT) return GT;
    if(e == OEQ) return EQ;
    if(e == ONE) return NE;

    assert(false);
    return LE;
}


// ##################################################################
// Classes used to recognized expression.
// ##################################################################


class XCSP3Core::PrimitivePattern
{
    public:
        Tree *canonized, pattern;
        std::vector<int> constants;
        std::vector<std::string> variables;
        std::vector<ExpressionType> operators;
        XCSP3Manager &manager;
        std::string id;

        PrimitivePattern(XCSP3Manager &m, string expr)
            : pattern(expr)
            , manager(m) {}
        
        virtual ~PrimitivePattern() {}

        PrimitivePattern *seeTarget(std::string _id, Tree *c)
        {
            id = _id;
            canonized = c;
            return this;
        }

        virtual bool post() = 0;

        bool match()
        {
            constants.clear();
            variables.clear();
            operators.clear();

            if(Node::areSimilar(canonized->root, pattern.root, operators, constants, variables) && post())
                return true;

            return false;
        }
};


class PrimitiveUnary1 : public XCSP3Core::PrimitivePattern
{
    // x op k
    public:
        PrimitiveUnary1(XCSP3Manager &m) : PrimitivePattern(m, "eq(x,3)")
        {
            pattern.root->type = OFAKEOP;
        }

        bool post() override
        {
            if(operators.size() != 1 || isRelationalOperator(operators[0]) == false)
                return false;
            if(operators[0] == OEQ || operators[0] == ONE)
            {
                std::vector<int> values;
                values.push_back(constants[0]);
                manager.callback->buildConstraintExtension(id, (XVariable *) manager.mapping[variables[0]], values, operators[0] == OEQ, false);
                return true;
            }
            if(operators[0] == OLE)
            {
                manager.callback->buildConstraintPrimitive(id, LE, (XVariable *) manager.mapping[variables[0]], constants[0]);
                return true;
            }
            return false;
        }
};


class PrimitiveUnary2 : public XCSP3Core::PrimitivePattern
{
    // x op k
    public:
        PrimitiveUnary2(XCSP3Manager &m) : PrimitivePattern(m, "le(3,x)") {}

        bool post() override
        {
            manager.callback->buildConstraintPrimitive(id, GE, (XVariable *) manager.mapping[variables[0]], constants[0]);
            return true;
        }
};


class PrimitiveUnary3 : public XCSP3Core::PrimitivePattern
{
    // x in {1,3,5...}
    PrimitiveUnary3(XCSP3Manager &m) : PrimitivePattern(m, "in(x,set(1,3,5))")
    {
        pattern.root->type = OFAKEOP;
    }

    bool post() override
    {
        if(operators.size() != 1 || (operators[0] != OIN && operators[0] != ONOTIN))
            return false;
        
        std::vector<int> values;
        for(Node *n : canonized->root->parameters[1]->parameters)
            values.push_back((dynamic_cast<NodeConstant *>(n))->val);
        if(values.size() == 0)
        {
            if(operators[0] == OIN)
                manager.callback->buildConstraintFalse(id);
            else
                manager.callback->buildConstraintTrue(id);
            return true;
        }
        manager.callback->buildConstraintExtension(id, (XVariable *) manager.mapping[variables[0]], values, operators[0] == OIN, false);
        return true;
    }
};


class PrimitiveUnary4 : public XCSP3Core::PrimitivePattern
{
    // x >= 1 and x <= 4
    public:
        PrimitiveUnary4(XCSP3Manager &m) : PrimitivePattern(m, "and(le(x,1),le(4,x))")
        {
            pattern.root->type = OFAKEOP;
        }

        bool post() override
        {
            if(variables[0] != variables[1] || operators.size() != 1 || (operators[0] != OAND && operators[0] != OOR))
                return false;
            if(operators[0] == OAND)
            {
                if(constants[1] > constants[0])
                    manager.callback->buildConstraintFalse(id);
                else
                    manager.callback->buildConstraintPrimitive(id, (XVariable *) manager.mapping[variables[0]], true, constants[1], constants[0]);
                return true;
            }
            if(constants[0] > constants[1])
                manager.callback->buildConstraintTrue(id);
            else
                manager.callback->buildConstraintPrimitive(id, (XVariable *) manager.mapping[variables[0]], false, constants[0] + 1, constants[1] - 1);
            return true;
        }
};


class PrimitiveBinary1 : public XCSP3Core::PrimitivePattern
{
    // x <op> y
    public:
        PrimitiveBinary1(XCSP3Manager &m) : PrimitivePattern(m, "eq(x,y)")
        {
            pattern.root->type = OFAKEOP;
        }

        bool post() override
        {
            if(operators.size() != 1 || isRelationalOperator(operators[0]) == false)
                return false;
            manager.callback->buildConstraintPrimitive(id, expressionTypeToOrderType(operators[0]),
                                                        (XVariable *) manager.mapping[variables[0]], 0,
                                                        (XVariable *) manager.mapping[variables[1]]);
            
            return true;
        }
};


class PrimitiveBinary2 : public XCSP3Core::PrimitivePattern
{
    // x + 3 <op> y
    public:
        PrimitiveBinary2(XCSP3Manager &m) : PrimitivePattern(m, "eq(add(x,3),y)")
        {
            pattern.root->type = OFAKEOP;   // don't care between logical operator
        }

        bool post() override
        {
            if(operators.size() != 1 || isRelationalOperator(operators[0]) == false)
                manager.callback->buildConstraintPrimitive(id, expressionTypeToOrderType(operators[0]),
                                                            (XVariable *) manager.mapping[variables[0]], 
                                                            constants[0], (XVariable *) manager.mapping[variables[1]]);

            return true;                                                
        }
};


class PrimitiveBinary3 : public XCSP3Core::PrimitivePattern
{
    // x = y <op> 3
    public:
        PrimitiveBinary3(XCSP3Manager &m) : PrimitivePattern(m, "eq(y,add(x,3))")
        {
            pattern.root->type = OFAKEOP;   // don't care between logical operator
        }

        bool post() override
        {
            if(operators.size() != 1 || isRelationalOperator(operators[0]) == false)
                return false;
            constants[0] = -constants[0];
            manager.callback->buildConstraintPrimitive(id, expressionTypeToOrderType(operators[0]),
                                                        (XVariable *) manager.mapping[variables[0]],
                                                        constants[0], (XVariable *) manager.mapping[variables[1]]);

            return true;                                            
        }
};


class PrimitiveTernary1 : public XCSP3Core::PrimitivePattern
{
    // x = y <op> 3
    public:
        PrimitiveTernary1(XCSP3Manager &m) : PrimitivePattern(m, "eq(add(y,z),x)")
        {
            pattern.root->type = OFAKEOP;   // don't care between logical operator
        }

        bool post() override
        {
            if(operators.size() != 1 || isRelationalOperator(operators[0]) == false)
                return false;
            std::vector<XVariable *> list;
            for(string &s : variables)
                list.push_back((XVariable *) manager.mapping[s]);

            vector<int> coefs;
            coefs.push_back(1);
            coefs.push_back(1);
            coefs.push_back(-1);

            XCondition cond;
            cond.operandType = INTEGER;
            cond.op = expressionTypeToOrderType(operators[0]);
            cond.val = 0;

            manager.callback->buildConstraintSum(id, list, coefs, cond);
            return true;
        }
};


bool XCSP3Manager::recognizePrimitives(std::string id, Tree *tree)
{
    for(PrimitivePattern *p : patterns)
        if(p->seeTarget(id, tree)->match())
            return true;
        
        return false;
}


void XCSP3Manager::createPrimitivePatterns()
{
    patterns.push_back(new PrimitiveUnary1(*this));
    patterns.push_back(new PrimitiveUnary2(*this));
    patterns.push_back(new PrimitiveUnary3(*this));
    patterns.push_back(new PrimitiveUnary4(*this));
    patterns.push_back(new PrimitiveBinary1(*this));
    patterns.push_back(new PrimitiveBinary2(*this));
    patterns.push_back(new PrimitiveBinary3(*this));
    patterns.push_back(new PrimitiveTernary1(*this));
}


void XCSP3Manager::destroyPrimitivePatterns()
{
    for(PrimitivePattern *p : patterns)
        delete p;
}


void XCSP3Manager::buildVariable(XVariable *variable)
{
    if(discardedClasses(variable->classes))
        return;
    
    if(variable->domain->values.size() == 1)
    {
        callback->buildVariableInteger(variable->id, variable->domain->values[0]->minimum(),
                                       variable->domain->values[0]->maximum());
        
        return;
    }
    std::vector<int> values;

    for(unsigned int i = 0; i < variable->domain->values.size(); i++)
    {
        for(int j = variable->domain->values[i]->minimum();
            j <= variable->domain->values[i]->maximum(); j++)
        {
            values.push_back(j);
        }
    }
    callback->buildVariableInteger(variable->id, values);
}


void XCSP3Manager::buildVariableArray(XVariableArray *variable)
{
    if(discardedClasses(variable->classes))
        return;
    
    for(XVariable *v : variable->variables)
        if(v != nullptr)
            buildVariable(v);
}


// ##################################################################
// Basic constraints
// ##################################################################


void XCSP3Manager::newConstraintExtension(XConstraintExtension *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    
    if(constraint->list.size() == 1)
    {
        std::vector<int> tuples;
        for(vector<int> &tpl : constraint->tuples)
            tuples.push_back(tpl[0]);
        callback->buildConstraintExtension(constraint->id, constraint->list[0], tuples,
                                           constraint->isSupport, constraint->containStar);
    }
    else
        callback->buildConstraintExtension(constraint->id, constraint->list, constraint->tuples,
                                           constraint->isSupport, constraint->containStar);
}


void XCSP3Manager::newConstraintExtensionAsLastOne(XConstraintExtension *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    callback->buildConstraintExtensionAs(constraint->id, constraint->list,
                                         constraint->isSupport, constraint->containStar);
}


void XCSP3Manager::newConstraintIntension(XConstraintIntension *constraint)
{
    if(callback->intensionUsingString && callback->recognizeSpecialIntensionCases)
        throw std::runtime_error("You have to choose: using string or be able to recognize special intension constraints");
    if(discardedClasses(constraint->classes))
        return;
    if(callback->intensionUsingString)
    {
        callback->buildConstraintIntension(constraint->id, constraint->function);
        return;
    }

    // std::cout << "ROOT1= " << constraint->function << std::endl;
    Tree *tree = new Tree(constraint->function);
    tree->canonize();

    // std::cout << "ROOT2= " ; tree->prefixe(); std::cout << std::endl;
    if(callback->recognizeSpecialIntensionCases && recognizePrimitives(constraint->id, tree))
        return;
    
    callback->buildConstraintIntension(constraint->id, tree);
}


// ##################################################################
// Languages constraints
// ##################################################################


void XCSP3Manager::newConstraintRegular(XConstraintRegular *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    callback->buildConstraintRegular(constraint->id,
                                     constraint->list,
                                     constraint->start,
                                     constraint->final,
                                     constraint->transitions);
}


void XCSP3Manager::newConstraintMDD(XConstraintMDD *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    callback->buildConstraintMDD(constraint->id, constraint->list, constraint->transitions);
}


// ##################################################################
// Comparison constraints
// ##################################################################


void XCSP3Manager::newConstraintAllDiff(XConstraintAllDiff *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    if(constraint->except.size() == 0)
    {
        XTree *xt = dynamic_cast<XTree *>(constraint->list[0]);
        if(xt != nullptr)
        {
            // alldiff over tree
            vector<Tree *> trees;
            for(XVariable *x : constraint->list)
            {
                xt = dynamic_cast<XTree *>(x);
                Tree *t = new Tree(xt->id);
                t->canonize();
                trees.push_back(t);
            }
            callback->buildConstraintAlldifferent(constraint->id, trees);
            return;
        }
        callback->buildConstraintAlldifferent(constraint->id, constraint->list);
    }
    else
        callback->buildConstraintAlldifferentExcept(constraint->id, constraint->list, constraint->except);
}


void XCSP3Manager::newConstrictAllDiffMatrix(XConstraintAllDiffMatrix *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    callback->buildConstraintAlldifferentMatrix(constraint->id, constraint->matrix);
}


void XCSP3Manager::newConstraintAllDiffList(XConstraintAllDiffList *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    callback->buildConstraintAlldifferentList(constraint->id, constraint->matrix);
}


void XCSP3Manager::newConstraintAllEqual(XConstraintAllEqual *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    callback->buildConstraintAllEqual(constraint->id, constraint->list);
}


void XCSP3Manager::newConstraintOrdered(XConstraintOrdered *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    if(constraint->lengths.size() > 0)
    {
        vector<int> lengths;
        for(XVariable *x : constraint->lengths)
            lengths.push_back(((XInteger *) x)->value);
        callback->buildConstraintOrdered(constraint->id,
                                         constraint->list,
                                         constraint->list,
                                         constraint->op);
    }
    else
        callback->buildConstraintOrdered(constraint->id,
                                         constraint->list,
                                         constraint->op);
}


void XCSP3Manager::newConstraintLex(XConstraintLex *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    callback->buildConstraintLex(constraint->id, constraint->lists, constraint->op);
}


void XCSP3Manager::newConstraintLexMatrix(XConstraintLexMatrix *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    callback->buildConstraintLexMatrix(constraint->id, constraint->matrix, constraint->op);
}


// ##################################################################
// Summin and Counting constraints
// ##################################################################


void XCSP3Manager::normalizeSum(vector<XVariable *> &list, vector<int> &coefs)
{
    // merge
    for(unsigned int i = 0; i < list.size() - 1; i++)
    {
        if(coefs[i] == 0) continue;
        for(auto j = i + 1; j < list.size(); j++)
        {
            if(coefs[j] != 0 && list[i]->id == list[j]->id)
            {
                coefs[i] += coefs[j];
                coefs[j] = 0;
            }
        }
    }
    vector<int> tmpc;
    vector<XVariable *> tmpv;
    // remove coef = 0
    for(unsigned int i = 0; i < list.size(); i++)
        if(coefs[i] != 0)
        {
            tmpv.push_back(list[i]);
            tmpc.push_back(coefs[i]);
        }
    list.clear();
    list.assign(tmpv.begin(), tmpv.end());
    coefs.clear();
    coefs.assign(tmpc.begin(), tmpc.end());
}


void XCSP3Manager::newConstraintSum(XConstraintSum *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    XCondition xc;
    constraint->extractCondition(xc);

    XTree *xt = dynamic_cast<XTree *>(constraint->list[0]);
    if(xt != nullptr)
    {
        // Sum over tree
        vector<Tree *> trees;
        for(XVariable *x : constraint->list)
        {
            xt = dynamic_cast<XTree *>(x);
            Tree *t = new Tree(xt->id);
            t->canonize();
            trees.push_back(t);
        }
        if(constraint->values.size() == 0)
            callback->buildConstraintSum(constraint->id, trees, xc);
        else
        {
            vector<int> coefs;
            int v;
            for(XEntity *xe : constraint->values)
            {
                isInteger(xe, v);
                coefs.push_back(v);
            }
            callback->buildConstraintSum(constraint->id, trees, coefs, xc);
        }
        return;
    }

    if(constraint->values.size() == 0)
    {
        bool toModify = false;
        if(callback->normalizeSum)
        {
            // Check if a variable appears two times
            for(unsigned int i = 0; i < constraint->list.size() - 1; i++)
                for(auto j = i + 1; j < constraint->list.size(); j++)
                {
                    if(constraint->list[i]->id == constraint->list[j]->id)
                        toModify = true;
                }
            if(toModify)
                constraint ->values.assign(constraint->list.size(), new XInteger("", 1));
        }
        if(!toModify)
        {
            callback->buildConstraintSum(constraint->id, constraint->list, xc);
            return;
        }
    }

    int v;
    if(isInteger(constraint->values[0], v))
    {
        vector<int> coefs;
        vector<XVariable *> list;
        for(XEntity *xe : constraint->values)
        {
            isInteger(xe, v);
            coefs.push_back(v);
        }
        list.assign(constraint->list.begin(), constraint->list.end());

        if(callback->normalizeSum)
            normalizeSum(list, coefs);

        callback->buildConstraintSum(constraint->id, list, coefs, xc);
        return;
    }

    std::vector<XVariable *> xvalues;
    for(XEntity *xe : constraint->values)
    {
        xvalues.push_back((XVariable *) mapping[xe->id]);
    }
    callback->buildConstraintSum(constraint->id, constraint->list, xvalues, xc);
}


void XCSP3Manager::newConstraintCount(XConstraintCount *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    int value;
    XCondition xc;
    constraint->extractCondition(xc);
    std::vector<int> values;


    // One integer value
    // Special cases AtLeastK, AtMostK, ..
    if(callback->recognizeSpecialCountCases && constraint->values.size() == 1 && isInteger(constraint->values[0], value))
    {
        if(xc.operandType == INTEGER && xc.op == OrderType::LE)
        {
            callback->buildConstraintAtMost(constraint->id, constraint->list, value, xc.val);
            return;
        }
        if(xc.operandType == INTEGER && xc.op == OrderType::LT)
        {
            callback->buildConstraintAtMost(constraint->id, constraint->list, value, xc.val - 1);
            return;
        }
        if(xc.operandType == INTEGER && xc.op == OrderType::GE)
        {
            callback->buildConstraintAtLeast(constraint->id, constraint->list, value, xc.val);
            return;
        }
        if(xc.operandType == INTEGER && xc.op == OrderType::LT)
        {
            callback->buildConstraintAtLeast(constraint->id, constraint->list, value, xc.val + 1);
            return;
        }
        if(xc.operandType == INTEGER && xc.op == OrderType::EQ)
        {
            callback->buildConstraintExactlyK(constraint->id, constraint->list, value, xc.val);
            return;
        }
        if(xc.operandType == VARIABLE && xc.op == OrderType::EQ)
        {
            callback->buildConstraintExactlyVariable(constraint->id, constraint->list, value, (XVariable *) mapping[xc.var]);
            return;
        }
    }

    // Among
    if(callback->recognizeSpecialCountCases 
       && xc.op == OrderType::EQ && 
       xc.operandType == INTEGER &&
       isInteger(constraint->values[0], value))
    {
        for(XEntity *xe : constraint->values)
        {
            isInteger(xe, value);
            values.push_back(value);
        }
        callback->buildConstraintAmong(constraint->id, constraint->list, values, xc.val);
        return;
    }

    if(isInteger(constraint->values[0], value))
    {
        // Iterate and get integers
        for(XEntity *xe = constraint->values)
        {
            isInteger(xe, value);
            values.push_back(value);
        }
        callback->buildConstraintCount(constraint->id, constraint->list, values, xc);
    }
    else
    {
        std::vector<XVariable *> values;
        for(XEntity *xe : constraint->values)
        {
            values.push_back((XVariable *) mapping[xe->id]);
        }
        callback->buildConstraintCount(constraint->id, constraint->list, values, xc);
    }
}


void XCSP3Manager::newConstraintNValues(XConstraintNValues *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    XCondition xc;
    constraint->extractCondition(xc);

    // Special NotAllEqual case
    if(callback->recognizeNValuesCases && 
       xc.operandType == INTEGER && 
       constraint->except.size() &&
       ((xc.op == GE && xc.val == 2) || (xc.op == GT && xc.val == 1)))
    {
        callback->buildConstraintNotAllEqual(constraint->id, constraint->list);
        return;
    }

    // Special AllEqual case
    if(callback->recognizeNValuesCases &&
       xc.operandType == INTEGER &&
       constraint->except.size() == 0 &&
       (xc.op == OrderType::EQ && xc.val == 1))
    {
        callback->buildConstraintAllEqual(constraint->id, constraint->list);
        return;
    }

    // Special AllDiff case
    if(callback->recognizeNValuesCases &&
       xc.operandType == INTEGER &&
       constraint->except.size() == 0 &&
       (xc.op == OrderType::EQ && ((unsigned int) xc.val) == constraint->list.size()))
    {
        callback->buildConstraintAlldifferent(constraint->id, constraint->list);
        return;
    }

    if(constraint->except.size() == 0)
    {
        callback->buildConstraintNValues(constraint->id, constraint->list, xc);
        return;
    }
    callback->buildConstraintNValues(constraint->id, constraint->list, constraint->except, xc);
}


void XCSP3Manager::newConstraintCardinality(XConstraintCardinality *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    std::vector<int> intValues;
    std::vector<XVariable *> varValues;
    int v;
    for(XEntity *xe : constraint->values)
    {
        if(isInteger(xe, v))
            intValues.push_back(v);
        else
        {
            XVariable *xv = (XVariable *) xe;
            varValues.push_back(xv);
        }
    }

    std::vector<int> intOccurs;
    std::vector<XVariable *> varOccurs;
    std::vector<XInterval> intervalOccurs;

    for(XEntity *xe : constraint->occurs)
    {
        if(isInteger(xe, v))
            intOccurs.push_back(v);
        else
        {
            int min, max;
            if(isInterval(xe, min, max))
                intervalOccurs.push_back(XInterval(min, max));
            else
            {
                XVariable *xv = (XVariable *) xe;
                varOccurs.push_back(xv);
            }
        }
    }

    if(intValues.size() > 0 && intOccurs.size() > 0)
    {
        callback->buildConstraintCardinality(constraint->id, constraint->list, intValues,
                                             intOccurs, constraint->closed);
        return;
    }
    if(intValues.size() > 0 && varOccurs.size() > 0)
    {
        callback->buildConstraintCardinality(constraint->id, constraint->list, intValues,
                                             varOccurs, constraint->closed);
        return;
    }
    if(intValues.size() > 0 && intervalOccurs.size() > 0)
    {
        callback->buildConstraintCardinality(constraint->id, constraint->list, intValues,
                                             intervalOccurs, constraint->closed);
        return;
    }
    if(varValues.size() > 0 && intOccurs.size() > 0)
    {
        callback->buildConstraintCardinality(constraint->id, constraint->list, varValues,
                                             intOccurs, constraint->closed);
        return;
    }
    if(varValues.size() > 0 && varOccurs.size() > 0)
    {
        callback->buildConstraintCardinality(constraint->id, constraint->list, varValues,
                                             varOccurs, constraint->closed);
        return;
    }
    if(varValues.size() > 0 && intervalOccurs.size() > 0)
    {
        callback->buildConstraintCardinality(constraint->id, constraint->list, varValues,
                                             intervalOccurs, constraint->closed);
        return;
    }
}


// ##################################################################
// Connection constraints
// ##################################################################


void XCSP3Manager::newConstraintMinimum(XConstraintMinimum *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    XCondition xc;
    constraint->extractCondition(xc);

    if(constraint->index == NULL)
    {
        callback->buildConstraintMinimum(constraint->id, constraint->list, xc);
        return;
    }
    callback->buildConstraintMinimum(constraint->id, constraint->list, 
                                     constraint->index, constraint->rank, xc);
}


void XCSP3Manager::newConstraintMaximum(XConstraintMaximum *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    XCondition xc;
    constraint->extractCondition(xc);

    if(constraint->index == NULL)
    {
        callback->buildConstraintMaximum(constraint->id, constraint->list, xc);
        return;
    }
    callback->buildConstraintmaximum(constraint->id, constraint->list,
                                     constraint->startIndex, constraint->rank, xc);
}


void XCSP3Manager::newConstraintElement(XConstraintElement *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    int v;
    vector<int> listOfIntegers;
    if(isInteger(constraint->list[0], v))
    {
        for(XEntity *xe : constraint->list)
        {
            isInteger(xe, v);
            listOfIntegers.push_back(v);
        }
    }

    if(isInteger(constraint->value, v))
    {
        if(constraint->index == NULL)
        {
            if(listOfIntegers.size() > 0)
                throw runtime_error("Not yet supported");
            else
                callback->buildConstraintElement(constraint->id, constraint->list, v);
        }
        else
        {
            if(listOfIntegers.size() > 0)
                throw runtime_error("Not yet supported");
            else
                callback->buildConstraintElement(constraint->id, constraint->list,
                                                 constraint->startIndex, constraint->rank, v);
        }
        return;
    }
    
    // Is variable
    XVariable *xv = (XVariable *) constraint->value;
    if(constraint->index == NULL)
    {
        if(listOfIntegers.size() > 0)
            throw runtime_error("Not yet supported");
        else
            callback->buildConstraintElement(constraint->id, constraint->list, xv);
    }
    else
    {
        if(listOfIntegers.size() > 0)
            callback->buildConstraintElement(constraint->id, listOfIntegers, constraint->startIndex,
                                             constraint->index, constraint->rank, xv);
        else
            callback->buildConstraintElement(constraint->id, constraint->list, constraint->startIndex,
                                             constraint->index, constraint->rank, xv);
    }
}


void XCSP3Manager::newConstraintChannel(XConstraintChannel *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    if(constraint->secondList.size() == 0 && constraint->value == NULL)
    {
        callback->buildConstraintChannel(constraint->id, constraint->list, constraint->startIndex1);
        return;
    }

    if(constraint->secondList.size() > 0 && constraint->value == NULL)
    {
        callback->buildConstraintChannel(constraint->id, constraint->list, constraint->startIndex1,
                                         constraint->secondList, constraint->startIndex2);
        return;
    }
    callback->buildConstraintChannel(constraint->id, constraint->list, constraint->startIndex1, constraint->value);
}


// ##################################################################
// Packing and Scheduling constraints
// ##################################################################


void XCSP3Manager::newConstraintStretch(XConstraintStretch *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    if(constraint->patterns.size() == 0)
        callback->buildConstraintStretch(constraint->id, constraint->list, 
                                         constraint->values, constraint->widths);
    else
        callback->buildConstraintStretch(constraint->id, constraint->list, 
                                         constraint->values, constraint->widths, constraint->patterns);
}


void XCSP3Manager::newConstraintNoOverlap(XConstraintNoOverlap *constraint)
{
    if(discardedClasses(constraint->classes))
        return;
    
    if(constraint->origins[0] == NULL)
    {
        newConstraintNoOverlapKDim(constraint);
        return;
    }

    int v;
    vector<int> intLengths;
    vector<XVariable *> varLengths;

    for(XEntity *xe : constraint->lengths)
    {
        if(isInteger(xe, v))
            intLengths.push_back(v);
        else
        {
            XVariable *xv = (XVariable *) xe;
            varLengths.push_back(xv);
        }
    }

    if(intLengths.size() > 0)
        callback->buildConstraintNoOverlap(constraint->id, constraint->origins,
                                           intLengths, constraint->zeroIgnored);
    else
        callback->buildConstraintNoOverlap(constraint->id, constraint->origins,
                                           varLengths, constraint->zeroIgnored);
}


