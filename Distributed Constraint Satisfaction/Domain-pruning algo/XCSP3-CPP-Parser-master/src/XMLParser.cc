
#include "XCSP3Domain.h"
#include "XCSP3Variable.h"
#include "XCSP3Constraint.h"
#include "XMLParser.h"


using namespace XCSP3Core;



// ##################################################################
// Callbacks from the XML parser
// ##################################################################


void XMLParser::startElement(UTF8String name, const AttributeList &attributes)
{
    // consume the last tokens before switch to the next elment
    if(!textLeft.empty())
    {
        handleAbridgeNotation(textLeft, true);
        textLeft.clear();
    }

    if(!stateStack.empty() && !stateStack.front().subtagAllowed)
        throw runtime_error("this element must not contain any element");
    
    TagActionList::iterator iAction = tagList.find(name);
    TagAction *action;

    if(iAction != tagList.end())
    {
        action = (*iAction).second;

        // ???
        // if (!action->isActivated())
        //          throw runtime_error("unexpected tag");
    }
    else
    {
        // add a handler to ignore the text and end element
        action = unknownTagHandler;
        cerr << "unknown tag " << name << endl;
    }

    stateStack.push_front(State());
    actionStack.push_front(action);
    action->beginTag(attributes);
}


void XMLParser::endElement(UTF8String name)
{
    // consume the last tokens
    if(!textLeft.empty())
    {
        handleAbridgeNotation(textLeft, true);
        textLeft.clear();
    }

    TagActionList::iterator iAction = tagList.find(name);

    if(iAction != tagList.end())
        (*iAction).second->endTag();
    
    actionStack.pop_front();
    stateStack.pop_front();
}


void XMLParser::characters(UTF8String chars)
{
    // cout << "chars= " << chars << "#" << endl;

    if(actionStack.empty())
    {
        if(chars.isWhiteSpace())
            return;
        else
            throw runtime_error("Text found outside any tag");
    }

    if(!textLeft.empty())
    {
        // break at first space, concatenate with textLeft and call
        // text()
        UTF8String::iterator it = chars.begin(), end = chars.end();

        while(it != end && !it.isWhiteSpace() && ((*it) != ')'))
        {
            textLeft.append(*it);
            ++it;
        }

        while(it != end && it.isWhiteSpace() && ((*it) == ')'))
        {
            textLeft.append(*it);
            ++it;
        }

        handleAbridgeNotation(textLeft, false);
        textLeft.clear();

        chars = chars.substr(it, chars.end());
    }

    // break after last space, call text() with the first part and
    // store the last part in textLeft

    UTF8String::iterator it, brk;

    brk = chars.end();
    while(brk != chars.begin())
    {
        --brk;
        char c = *brk;
        if(brk.isWhiteSpace() || c == ')')
        {
            ++brk;
            break;
        }
    }

    for(it = brk; it != chars.end(); ++it)
        textLeft.append(*it);
    
    chars = chars.substr(chars.begin(), brk);

    if(!chars.empty())
        handleAbridgeNotation(chars, false);
}


void XMLParser::handleAbridgeNotation(UTF8String chars, bool lastChunk)
{
    UTF8String::iterator it, beg, end;

    it = chars.begin();
    beg = chars.begin();
    end = chars.end();

    while(it != end)
    {
        // skip spaces
        while(it != end && it.isWhiteSpace())
            ++it;
        
        if(it == end)
            break;
        
        // no special token
        while(it != end && !it.isWhiteSpace())
            ++it;
    }

    if(beg != end)
        actionStack.front()->text(chars.substr(beg, end), lastChunk);

}


// ##################################################################
// Parse a sequence of tokens. Each token can represent 
// a compact list of array variables, or a basic entity, 
// or a template parameter
// ##################################################################


void XMLParser::parseSequence(const UTF8String &txt, vector<XVariable *> &list, vector<char> delimiters)
{
    UTF8String::Tokenizer tokenizer(txt);

    for(char c : delimiters)
        tokenizer.addSeparator(c);
    
    while(tokenizer.hasMoreTokens())
    {
        UTF8String token = tokenizer.nextToken();
        bool isSep = false;
        for(unsigned int i = 0; i < delimiters.size(); i++)
        {
            string tt;
            token.to(tt);
            if(tt.size() == 1 && tt[0] == delimiters[i])
            {
                if(i == 0)
                    list.push_back(NULL);
                isSep = true;
            }
        }
        if(isSep)
            continue;
        

        string current, compactForm;
        token.to(current);
        current = trim(current);
        size_t tree = current.find('(');
        if(tree != string::npos)
        {
            // Tree expressions
            list.push_back(new XTree(current));
            continue;
        }

        size_t percent = current.find('%');
        if(percent == string::npos)
        {
            // Normal variable
            size_t pos = current.find('[');
            if(pos == string::npos)
            {
                // Not an array
                size_t dotdot = current.find('.');
                if(dotdot == string::npos)
                {
                    int nb;
                    try
                    {
                        // An integer
                        nb = std::stoi(current);
                        XInteger *xi = new XInteger(current, nb);
                        list.push_back(xi);
                        toFree.push_back(xi);

                    }
                    catch(invalid_argument &e)
                    {
                        if(variablesList[current] != NULL)
                            list.push_back((XVariable *) variablesList[current]);
                        else
                            throw runtime_error("unknown variable: " + current);

                    }
                }
                else
                {
                    // A range
                    int first = std::stoi(current.substr(0, dotdot));
                    int last = std::stoi(current.substr(dotdot + 2));
                    if(keepIntervals)
                    {
                        list.push_back(new XEInterval(current, first, last));
                    }
                    else
                    {
                        for(int i = first; i <= last; i++)
                        {
                            XInteger *xi = new XInteger(to_string(i), i);
                            list.push_back(xi);
                            toFree.push_back(xi);
                        }
                    }
                }
            }
            else
            {
                string name;
                token.substr(0, pos).to(name);
                token.substr(pos).to(compactForm);

                if(variablesList[name] == NULL)
                    throw runtime_error("unknown variable: " + name);
                ((XVariableArray *) variablesList[name])->getVarFor(list, compactForm);
            }
        }
        else
        {
            // Parameter Variable form group template
            XParameterVariable *xpv = new XParameterVariable(current);
            if(xpv->number == -1) nbParameters = -1; else nbParameters++;
            list.push_back(xpv);
            toFree.push_back(xpv);
        }
    }

}


