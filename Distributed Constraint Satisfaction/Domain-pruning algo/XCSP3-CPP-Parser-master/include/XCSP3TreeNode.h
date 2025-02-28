
#ifndef XCSP3PARSER_XCSP3TREENODE_H_
#define XCSP3PARSER_XCSP3TREENODE_H_


#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>


namespace XCSP3Core {

    enum ExpressionType {
        OUNDEF,
        ONEG,
        OABS,
        OSQR,
        OADD,
        OSUB,
        OMUL,
        ODIV,
        OMOD,
        OPOW,
        ODIST,
        OMIN,
        OMAX,
        OLT,
        OLE,
        OGE,
        OGT,
        ONE,
        OEQ,
        OSET,
        OIN,
        ONOTIN,
        ONOT,
        OAND,
        OOR,
        OXOR,
        OIFF,
        OIMP,
        OIF,
        OCARD,
        OUNION,
        OINTER,
        ODIFF,
        OSDIFF,
        OHULL,
        ODJOINT,
        OSUBSET,
        OSUBSEQ,
        OSUPSEQ,
        OSUPSET,
        OCONVEX,
        OFDIV,
        OFMOD,
        OSQRT,
        ONROOT,
        OEXP,
        OLN,
        OLOG,
        OSIN,
        OCOS,
        OTAN,
        OASIN,
        OACOS,
        OATAN,
        OSINH,
        OCOSH,
        OTANH,
        OVAR,
        OPAR,
        OLONG,
        ORATIONAL,
        ODECIMAL,
        OSYMBOL,
        OFAKEOP   // Used only to match primitives
    };

    bool isSymmetricOperator(ExpressionType type);
    bool isNonSymmetricRelationalOperator(ExpressionType type);

    ExpressionType arithmeticInversion(ExpressionType type);

    std::string operatorToString(ExpressionType op);

    ExpressionType logicalInversion(ExpressionType type);

    bool isRelationalOperator(ExpressionType type);


    // ##############################################################
    // Node and ExpressionType
    // ##############################################################


    class Node {
        friend class Intension;

    public:
        ExpressionType type;

        std::vector<Node*> parameters;  // Useless for constant and variables, but avoid many casts!

        Node(ExpressionType o) : type(o) {}

        virtual int evaluate(std::map<std::string, int>& tuple) = 0;
        virtual Node* canonize() = 0;
        virtual void prefixe() = 0;

        static bool areSimilar(Node* canonize, Node* pattern, std::vector<ExpressionType>& operators, std::vector<int>& constants, std::vector<std::string>& variables);
    };


    // ##############################################################
    // NodeConstant
    // ##############################################################


    class NodeConstant : public Node {
    
    public:
        int val;

        NodeConstant(int v) : Node(ODECIMAL), val(v) {}

        // std::map<std::string, int>& tuple
        int evaluate(std::map<std::string, int>&) override {
            return val;
        }

        Node* canonize() override {
            return this;
        }

        void prefixe() override {
            std::cout << val;
        }
    };


    // ##############################################################
    // NodeVariable
    // ##############################################################


    class NodeVariable : public Node {
    
    public:
        std::string var;

        NodeVariable(std::string v) : Node(OVAR), var(v) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return tuple[var];
        }

        Node* canonize() override {
            return this;
        }

        void prefixe() override {
            std::cout << var;
        }
    };


    // ##############################################################
    // NodeOperator and NodeUnary
    // ##############################################################


    class NodeOperator : public Node {
    public:
        std::string op;

        NodeOperator(std::string o, ExpressionType _operator) : Node(_operator), op(o) {}

        NodeOperator* addParameter(Node* p) {
            parameters.push_back(p);
            return this;
        }

        NodeOperator* addParameters(std::vector<Node*> params) {
            parameters.insert(parameters.end(), params.begin(), params.end());
            return this;
        }

        void prefixe() override {
            std::cout << op << "(";
            for (unsigned int i = 0; i < parameters.size(); i++) {
                if (i != 0) std::cout << ",";
                parameters[i]->prefixe();
            }
            std::cout << ")";
        }

        Node* canonize() override;
    };

    class NodeUnary : public NodeOperator {
    public:
        NodeUnary(std::string o, ExpressionType _operator) : NodeOperator(o, _operator) {}

    };


    // ##############################################################
    // NodeBinary
    // ##############################################################


    class NodeBinary : public NodeOperator {
    public:

        NodeBinary(std::string o, ExpressionType _operator) : NodeOperator(o, _operator) {}
    };


    // ##############################################################
    // NodeNAry
    // ##############################################################


    class NodeNAry : public NodeOperator {
        friend class NodeIn;
        friend class NodeNotIn;

    public:
        NodeNAry(std::string o, ExpressionType _operator) : NodeOperator(o, _operator) {}
    };


    // ##############################################################
    // NodeNeg
    // ##############################################################


    class NodeNeg : public NodeUnary {
    public:
        NodeNeg() : NodeUnary("neg", ONEG) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return -parameters[0]->evaluate(tuple);
        }
    };


    // ##############################################################
    // NodeAbs and NodeSquare and NodeNot
    // ##############################################################


    class NodeAbs : public NodeUnary {
    public:
        NodeAbs() : NodeUnary("abs", OABS) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int v = parameters[0]->evaluate(tuple);
            return v > 0 ? v : -v;
        }
    };


    class NodeSquare : public NodeUnary {
    public:
        NodeSquare() : NodeUnary("sqr", OSQR) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int v = parameters[0]->evaluate(tuple);
            return v * v;
        }
    };


    class NodeNot : public NodeUnary {
    public:
        NodeNot() : NodeUnary("not", ONOT) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int v = parameters[0]->evaluate(tuple);
            return !v;  // v ? 0 : v;
        }
    };


    
}