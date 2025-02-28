
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


    // ##############################################################
    // Node for Sub, Div, Mod, Pow, Dist
    // ##############################################################


    class NodeSub : public NodeBinary {
    public:
        NodeSub() : NodeBinary("sub", OSUB) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) - parameters[1]->evaluate(tuple);
        }
    };


    class NodeDiv : public NodeBinary {
    public:
        NodeDiv() : NodeBinary("div", ODIV) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) / parameters[1]->evaluate(tuple);
        }
    };


    class NodeMod : public NodeBinary {
    public:
        NodeMod() : NodeBinary("mod", OMOD) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) % parameters[1]->evaluate(tuple);
        }
    };


    class NodePow : public NodeBinary {
    public:
        NodePow() : NodeBinary("pow", OPOW) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return pow(parameters[0]->evaluate(tuple), parameters[1]->evaluate(tuple));
        }
    };


    class NodeDist : public NodeBinary {
    public:
        NodeDist() : NodeBinary("dist", ODIST) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int v = parameters[0]->evaluate(tuple) - parameters[1]->evaluate(tuple);
            return v > 0 ? v : -v;
        }
    };


    // ##############################################################
    // Node for LE, LT, GE, GT, NE, Imp
    // ##############################################################


    class NodeLE : public NodeBinary {
    public:
        NodeLE() : NodeBinary("le", OLE) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) <= parameters[1]->evaluate(tuple);
        }
    };


    class NodeLT : public NodeBinary {
    public:
        NodeLT() : NodeBinary("lt", OLT) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) < parameters[1]->evaluate(tuple);
        }
    };


    class NodeGE : public NodeBinary {
    public:
        NodeGE() : NodeBinary("ge", OGE) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) >= parameters[1]->evaluate(tuple);
        }
    };


    class NodeGT : public NodeBinary {
    public:
        NodeGT() : NodeBinary("gt", OGT) {}

        int evaluate(std::map<string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) > parameters[1]->evaluate(tuple);
        }
    };


    class NodeNE : public NodeBinary {
    public:
        NodeNE() : NodeBinary("ne", ONE) {}

        int evaluate(std::map<string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) != parameters[1]->evaluate(tuple);
        }
    };


    class NodeImp : public NodeBinary {
    public:
        NodeImp() : NodeBinary("impl", OIMP) {}

        int evaluate(std::map<string, int>& tuple) override {
            return parameters[0]->evaluate(tuple) == 0 || parameters[1]->evaluate(tuple);
        }
    };


    // ##############################################################
    // Node for Add, Mult, Min, Max
    // ##############################################################


    class NodeAdd : public NodeNAry {
    public:
        NodeAdd() : NodeNAry("add", OADD) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int nb = 0;
            for (unsigned int i = 0; i < parameters.size(); i++)
                nb += parameters[i]->evaluate(tuple);
            return nb;
        }
    };


    class NodeMult : public NodeNAry {
    public:
        NodeMult() : NodeNAry("mul", OMUL) {}

        int evaluate(std::map<string, int>& tuple) override {
            int nb = 1;
            for (unsigned int i = 0; i < parameters.size(); i++)
                nb *= parameters[i]->evaluate(tuple);
            return nb;
        }
    };


    class NodeMin : public NodeNAry {
    public:
        NodeMin() : NodeNAry("min", OMIN) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int nb = parameters[0]->evaluate(tuple);
            for (unsigned int i = 1; i < parameters.size(); i++) {
                int v = parameters[i]->evaluate(tuple);
                if (v < nb) nb = v;
            }
            return nb;
        }
    };


    class NodeMax : public NodeNAry {
    public:
        NodeMax() : NodeNAry("max", OMAX) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int nb = parameters[0]->evaluate(tuple);
            for (unsigned int i = 1; i < parameters.size(); i++) {
                int v = parameters[i]->evaluate(tuple);
                if (v > nb) nb = v;
            }
            return nb;
        }
    };


    // ##############################################################
    // Node for EQ, And, Or, Xor, Iff
    // ##############################################################


    class NodeEQ : public NodeNAry {
    public:
        NodeEQ() : NodeNAry("eq", OEQ) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int nb = parameters[0]->evaluate(tuple);
            for (unsigned int i = 1; i < parameters.size(); i++) {
                if (nb != parameters[i]->evaluate(tuple))
                    return 0;
            }
            return 1;
        }
    };


    class NodeAnd : public NodeNAry {
    public:
        NodeAnd() : NodeNAry("and", OAND) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            for (unsigned int i = 0; i < parameters.size(); i++) 
                if (!parameters[i]->evaluate(tuple))
                    return 0;
            return 1;
        }
    };


    class NodeOr : public NodeNAry {
    public:
        NodeOr() : NodeNAry("or", OOR) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            for (unsigned int i = 0; i < parameters.size(); i++)
                if (parameters[i]->evaluate(tuple)) {
                    return 1;
                }
            return 0;
        }
    };


    class NodeXor : public NodeNAry {
    public:
        NodeXor() : NodeNAry("xor", OXOR) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int nb = 0;
            for (unsigned int i = 0; i < parameters.size(); i++)
                nb = nb + parameters[i]->evaluate(tuple);
            return nb % 2 == 1;
        }
    };


    class NodeIf : public NodeNAry {
    public:
        NodeIf() : NodeNAry("if", OIF) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int nb = parameters[0]->evaluate(tuple);
            if (nb) return parameters[1]->evaluate(tuple);
            return parameters[2]->evaluate(tuple);
        }
    };


    class NodeIff : public NodeNAry {
    public:
        NodeIff() : NodeNAry("iff", OIFF) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            assert(parameters.size() == 2); // TODO if greater!!
            int nb = parameters[0]->evaluate(tuple);
            return (nb) ? parameters[1]->evaluate(tuple) : !parameters[1]->evaluate(tuple);
        }
    };


    class NodeSet : public NodeNAry {
    public:
        NodeSet() : NodeNAry("set", OSET) {}

        // std::map<std::string, int>& tuple
        int evaluate(std::map<std::string, int>& ) override {
            throw std::runtime_error("can't evaluate set");
        }
    };


    class NodeIn : public NodeBinary {
    protected:
        std::vector<int> set;

    public:
        NodeIn() : NodeBinary("in", OIN) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int nb = parameters[0]->evaluate(tuple);
            set.clear();
            NodeSet* nodeSet;

            if ((nodeSet = dynamic_cast<NodeSet*>(parameters[1])) == NULL)
                throw std::runtime_error("intension constraint : in requires a set as second parameter");
            for (unsigned int i = 0; i < nodeSet->parameters.size(); i++)
                set.push_back(nodeSet->parameters[i]->evaluate(tuple));
            return find(set.begin(), set.end(), nb) != set.end();
        }
    };


    class NodeNotIn : public NodeBinary {
    protected:
        std::vector<int> set;

    public:
        NodeNotIn() : NodeBinary("notin", ONOTIN) {}

        int evaluate(std::map<std::string, int>& tuple) override {
            int nb = parameters[0]->evaluate(tuple);
            set.clear();
            NodeSet* nodeSet;

            if ((nodeSet = dynamic_cast<NodeSet*>(parameters[1])) == NULL)
                throw std::runtime_error("intension constraint : in requires a set as second parameter");
            for (unsigned int i = 0; i < nodeSet->parameters.size(); i++)
                set.push_back(nodeSet->parameters[i]->evaluate(tuple));
            return find(set.begin(), set.end(), nb) == set.end();
        }
    };

}



#endif  // XCSP3PARSER_XCSP3TREENODE_H_