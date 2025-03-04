
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


// Return Treu if START appears;
bool XMLParser::parseTuples(const UTF8String &txt, vector<vector<int>> &tuples)
{
    bool hasStar = false;
    UTF8String::Tokenizer tokenizer(txt);
    tokenizer.addSeparator(')');
    tokenizer.addSeparator(',');
    tokenizer.addSeparator('(');
    while(tokenizer.hasMoreTokens())
    {
        UTF8String token = tokenizer.nextToken();
        if(token == UTF8String(",")) continue;
        if(token == UTF8String("("))
        {
            currentTuple.clear();
            continue;
        }
        int val = -1;
        if(token == UTF8String(")"))
        {
            hasStar = true;
            val = STAR;
        }
        else
            token.to(val);
        currentTuple.push_back(val);
    }
    return hasStar;
}


void XMLParser::parseDomain(const UTF8String &txt, XDomainInteger &domain)
{
    UTF8String::Tokenizer tokenizer(txt);
    UTF8String dotdot("..");
    while(tokenizer.hasMoreTokens())
    {
        UTF8String token = tokenizer.nextToken();
        size_t pos = token.find(dotdot);

        if(pos == UTF8String::npos)
        {
            int val;
            if(false == token.to(val))
            {
                std::string ds;
                txt.to(ds);
                throw std::runtime_error("Integer expected: " + ds);
            }
            domain.addValue(val);
        }
        else
        {
            int first, last;
            if((false == token.substr(0, pos).to(first)) || (false == token.substr(pos + 2).to(last)))
            {
                std::string ds;
                txt.to(ds);
                throw std::runtime_error("Integer expected: " + ds);
            }
            domain.addInterval(first, last);
        }
    }
}


void XMLParser::parseListOfIntegerOrInterval(const UTF8String &txt, vector<XIntegerEntity *> &listToFill)
{
    UTF8String::Tokenizer tokenizer(txt);
    UTF8String dotdot = "..";
    while(tokenizer.hasMoreTokens())
    {
        UTF8String token = tokenizer.nextToken();
        size_t pos = token.find(dotdot);

        if(pos == UTF8String::npos)
        {
            int val;
            if(false == token.to(val))
            {
                std::string ds;
                txt.to(ds);
                throw std::runtime_error("Integer expected: " + ds);
            }
            XIntegerValue *xv = new XIntegerValue(val);
            listToFill.push_back(xv);
            toFreeEntity.push_back(xv);
        }
        else
        {
            int first, last;
            if((false == token.substr(0, pos).to(first)) || (false == token.substr(pos + 2).to(last)))
            {
                std::string ds;
                txt.to(ds);
                throw std::runtime_error("Integer expected: " + ds);
            }
            XIntegerInterval *xi = new XIntegerInterval(first, last);
            listToFill.push_back(xi);
            toFreeEntity.push_back(xi);
        }
    }
}


// ##################################################################
// Constructor and Descturctor
// ##################################################################


XMLParser::XMLParser(XCSP3CoreCallbacks *cb)
{
    keepIntervals = false;
    this->manager = new XCSP3Manager(cb, variablesList);
    unknownTagHandler = new UnknownTagAction(this, "unknown");

    registerTagAction(tagList, new InstanceTagAction(this, "instance"));

    registerTagAction(tagList, new VariablesTagAction(this, "variables"));
    registerTagAction(tagList, varTagAction = new varTagAction(this, "var"));
    registerTagAction(tagList, new ArrayTagAction(this, "array"));
    registerTagAction(tagList, new DomainTagAction(this, "domain"));

    registerTagAction(tagList, new AnnotationsTagAction(this, "annotations"));
    registerTagAction(tagList, new DecisionTagAction(this, "decision"));

    registerTagAction(tagList, new ConstraintsTagAction(this, "constraints"));

    registerTagAction(tagList, new ExtensionTagAction(this, "extension"));
    registerTagAction(tagList, this->listTag = new ListTagAction(this, "list"));
    registerTagAction(tagList, new ConflictOrSupportTagAction(this, "supports"));
    registerTagAction(tagList, new ConflictOrSupportTagAction(this, "conflicts"));

    registerTagAction(tagList, new IntensionTagAction(this, "intension"));

    registerTagAction(tagList, new AllDiffEqualTagAction(this, "allDifferent"));
    registerTagAction(tagList, new AllDiffEqualTagAction(this, "allEqual"));

    registerTagAction(tagList, new SumTagAction(this, "sum"));
    registerTagAction(tagList, new ListOfVariablesOrIntegerTagAction(this, "coeffs", this->values));
    registerTagAction(tagList, new ConditionTagAction(this, "condition"));

    registerTagAction(tagList, new OrderedTagAction(this, "ordered"));

    registerTagAction(tagList, new ChannelTagAction(this, "channel"));
    
    registerTagAction(tagList, new LexTagAction(this, "lex"));

    registerTagAction(tagList, new CountTagAction(this, "count"));
    registerTagAction(tagList, new CardinalityTagAction(this, "cardinality"));


    // Value and Values are quite identical
    registerTagAction(tagList, new ListOfVariablesOrIntegerTagAction(this, "values", this->values));
    registerTagAction(tagList, new ListOfVariablesOrIntegerTagAction(this, "value", this->values));

    registerTagAction(tagList, new NValuesTagAction(this, "nValues"));
    registerTagAction(tagList, new InstantiationTagAction(this, "instantiation"));

    registerTagAction(tagList, new GroupTagAction(this, "group"));
    registerTagAction(tagList, new ArgsTagAction(this, "args"));

    registerTagAction(tagList, new MinMaxTagAction(this, "minimum"));
    registerTagAction(tagList, new MinMaxTagAction(this, "maximum"));

    registerTagAction(tagList, new IndexTagAction(this, "index"));

    registerTagAction(tagList, new ElementTagAction(this, "element"));

    registerTagAction(tagList, new NoOverlapTagAction(this, "noOverlap"));
    registerTagAction(tagList, new CumulativeTagAction(this, "cumulative"));
    registerTagAction(tagList, new OriginsTagAction(this, "origins", this->origins));
    registerTagAction(tagList, new OriginsTagAction(this, "lengths", this->lengths));
    registerTagAction(tagList, new ListOfVariablesOrIntegerTagAction(this, "ends", this->ends));
    registerTagAction(tagList, new ListOfVariablesOrIntegerTagAction(this, "heights", this->heights));

    registerTagAction(tagList, new ListOfVariablesOrIntegerOrIntervalTagAction(this, "occurs", this->occurs));
    registerTagAction(tagList, new StretchTagAction(this, "stretch"));
    registerTagAction(tagList, new ListOfIntegerOrIntervalTagAction(this, "widths", this->widths));

    registerTagAction(tagList, new OperatorTagAction(this, "operator"));

    registerTagAction(tagList, new RegularTagAction(this, "regular"));
    registerTagAction(tagList, new MDDTagAction(this, "mdd"));
    registerTagAction(tagList, new StringTagAction(this, "start"));
    registerTagAction(tagList, new StringTagAction(this, "final"));
    registerTagAction(tagList, new TransitionsTagAction(this, "transitions"));
    registerTagAction(tagList, new PatternsTagAction(this, "patterns"));

    registerTagAction(tagList, new ObjectivesTagAction(this, "objectives"));
    registerTagAction(tagList, new MinimizeOrMaximizeTagAction(this, "minimize"));
    registerTagAction(tagList, new MinimizeOrMaximizeTagAction(this, "maximize"));

    registerTagAction(tagList, new ListOfIntegerTagAction(this, "except"));
    registerTagAction(tagList, new MatrixTagAction(this, "matrix"));

    registerTagAction(tagList, new BlockTagAction(this, "block"));
    registerTagAction(tagList, new SlideTagAction(this, "slide"));

    registerTagAction(tagList, new CircuitTagAction(this, "circuit"));
    registerTagAction(tagList, new ListOfVariablesOrIntegerTagAction(this, "size", this->values));


}


XMLParser::~XMLParser()
{
    delete unknownTagHandler;
    for(TagActionList::iterator it = tagList.begin();
        it != tagList.end(); ++it)
        delete (*it).second;
}