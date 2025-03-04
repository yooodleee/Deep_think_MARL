
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


