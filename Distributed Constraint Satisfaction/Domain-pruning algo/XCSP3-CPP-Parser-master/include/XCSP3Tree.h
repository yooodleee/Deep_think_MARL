
#ifndef TREE_H_
#define TREE_H_


#include <cmath>
#include <vector>
#include <map>
#include <iostream>
#include <assert.h>


#include "XCSP3TreeNode.h"


namespace XCSP3Core {

    class Tree {
    protected:
        void createOperator(std::string currentElement, std::Vector<NodeOperator*>* stack, std::vector<Node*>& params);
        void closeOperator(std::vector<NodeOperator*>& stack, std::vector<Node*>& params);
        voide createBasicParameter(std::string currentElement, std::vector<NodeOperator*>& stack, std::vector<Node*>& params);

    public:
        Node* root;
        std::vector<std::string> listOfVariables;

        Tree(std::string e) : expr(e) {
            root = fromStringToTree(expr);
        }

        Tree(Node* r) : root(r) { }

        Node* fromStringToTree(std::string);

        int arity() {
            return listOfVariables.size();
        }
        int evaluate(std::map<std::string, int>& tuple) {
            return root->evaluate(tuple);
        }

        void prefixe() {
            root->prefixe();
        }
        void canonize() {
            root = root->canonize();
        }
    };
}



#endif  // TREE_H_