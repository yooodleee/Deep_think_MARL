
#include <map>
#include <sstream>
#include <vector>
#include <limits>
#include <algorithm>


#include "XCSP3Tree.h"
#include "XCSP3TreeNode.h"


using namespace XCSP3Core;
using namespace std;


// trim from start

static inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}


// trim from end

static inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}


// trim from both ends

static inline std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

template <typename T>
static int min(T v1, T v2, T v3)
{
    if (v1 == -1) v1 = std::numeric_limits<T>::max();
    if (v2 == -1) v2 = std::numeric_limits<T>::max();
    if (v3 == -1) v3 = std::numeric_limits<T>::max();

    return std::min( { v1, v2, v3 } );
}

Node *Tree::fromStringToTree(std::string current)
{
    current = trim(current);
    std::vector<NodeOperator*> stack;
    std::vector<Node*> params;

    while (true)
    {
        int posOpenParenthesis = current.find('(');
        int posCloseParenthesis = current.find(')');
        int posComma = current.find(',');
        int nb = min(posCloseParenthesis, posComma, posOpenParenthesis);

        string currentElement = current.substr(0, nb);
        if(currentElement != "" && nb != posOpenParenthesis)
            createBasicParameter(currentElement, stack, params);

        if(nb == posCloseParenthesis)
            closeOperator(stack, params);
        
        if(nb == posOpenParenthesis)
            createOperator(currentElement, stack, params);
        
        current = current.substr(nb + 1);
        if(current == "") break;
    }
    assert(params.size() == 1);
    assert(stack.size() == 0);

    return params.back();
}


extern NodeOperator* createNodeOperator(std::string currentElement);
void Tree::createOperator(string currentElement, std::vector<NodeOperator*> &stack, std::vector<Node*> &params)
{
    NodeOperator *tmp = createNodeOperator(currentElement);
    if(tmp == nullptr)
        throw runtime_error("Intension constraint. Unknown operator: " + currentElement);
    stack.push_back(tmp);
    params.push_back(nullptr);  // delemitor
}


void Tree::closeOperator(std::vector<NodeOperator*> &stack, std::vector<Node*> &params)
{
    NodeOperator *tmp = stack.back();

    int startParams = params.size() - 1;
    while(params[startParams] != nullptr)
        startParams--;
    startParams++;
    int nbP = 0;
    for(unsigned int i = startParams; i < params.size(); i++, nbP++)
        tmp->addParameter(params[i]);
    stack.pop_back();
    // params.shrink(nbP);
    params.erase(params.end() - nbP, params.end());
    assert(params.back() == nullptr);
    params.pop_back();
    params.push_back(tmp);
}


// string currentElement, std::Vector<NodeOperator*> &stack, std::vector<Node*> &params
void Tree::createBasicParameter(string currentElement, std::vector<NodeOperator*> &, std::vector<Node*> &params)
{
    try 
    {
        int nb = stoi(currentElement);
        params.push_back(new NodeConstant(nb));
    }
    catch(invalid_argument e)
    {
        int position = -1;
        for(unsigned int i = 0; i < listOfVariables.size(); i++)
            if(listOfVariables[i] == currentElement)
            {
                position = i;
                break;
            }
        if(position == -1)
        {
            listOfVariables.push_back(currentElement);
            position = listOfVariables.size() - 1;
        }
        params.push_back(new NodeVariable(currentElement));
    }
}