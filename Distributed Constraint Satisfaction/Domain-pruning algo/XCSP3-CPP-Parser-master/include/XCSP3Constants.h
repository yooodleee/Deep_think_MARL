
#ifndef XCSP3CONSTANTS_H_
#define XCSP3CONSTANTS_H_


#include <climits>


namespace XCSP3Core {
    typedef enum {
        CSP, 
        COP
    } InstanceType;


    typedef enum CONSTRAINTS {
        UNKNOWN, 
        EXTENSION, 
        INTENSION, 
        ALLDIFF, 
        ALLEQUAL, 
        SUM, 
        ORDERED, 
        COUNT, 
        NVALUES, 
        CARDINALITY, 
        MAXIMUM, 
        MINIMUM,
        ELEMENT, 
        NOOVERLAP, 
        STRETCH, 
        LEX, 
        CHANNEL, 
        REGULAR, 
        MDD, 
        CUMULATIVE, 
        INSTANTIATION, 
        CIRCUIT
    } ConstraintType;


    typedef enum order {
        LE,
        LT,
        GE,
        GT,
        IN,
        EQ,
        NE
    } OrderType;


    typedef enum operandtype {
        INTEGER,
        INTERVAL,
        VARIABLE
    } OperandType;


    typedef enum tag {
        UNKNOWNTAG, 
        LISTTAG, 
        FUNCTIONALTAG, 
        VALUESTAG, 
        VALUETAG, 
        CONDITIONTAG, 
        INDEXTAG, 
        LENGTHSTAG
    } Tag;


    typedef enum ranktype {
        ANY,
        FIRST,
        LAST
    } RankType;


    typedef enum objective {
        MINIMIZE,
        MAXIMIZE
    } ObjectiveGoal;


    typedef enum expressionObjective {
        EXPRESSION_O,
        SUM_O,
        PRODUCT_O,
        MINIMUM_O,
        MAXIMUM_O,
        NVALUES_O,
        LEX_O
    } ExpressionObjective;

#define STAR INT_MAX
}



#endif  // XCSP3CONSTANTS_H_