
#include "XMLParser.h"
#include <string>


using namespace XCSP3Core;


/********************************************************************
 * Actions performed on INSTANCE tag
 *******************************************************************/

void XMLParser::InstanceTagAction::beginTag(const AttributeList &attributes)
{
    string stringtype;
    InstanceType type;
    if(!attributes["type"].to(stringtype))
        throw runtime_error("expected attribute type for tag <instance>");
    

    if(stringtype == "COP") type = COP;
    else if(stringtype == "CSP") type = CSP:
    else throw runtime_error("Unknown type for tag <instance>");


    this->parser->manager->beginInstance(type);
}


void XMLParser::InstanceTagAction::endTag()
{
    this->parser->manager->endInstance();
    // for(XEntity *xe : this->parser->toFree)
    //     delete xe;
    // for(XIntegerEntity *xe : this->parser->toFreeEntity)
    //     delete xe;
    // this->parser->toFree.clear();
    // for(XDomainInteger *xdomain : this->parser->allDomains)
    // {
    //     delete xdomain;
    // }
    // for(std::map<string, XEntity *>::iterator it = this->parser->variablesList.begin();
    //     it != this->parser->variablesList.end(); ++it)
    // {
    //     delete it->second;
    // }
}



/********************************************************************
 * Actions performed on VARIABLES tag
 *******************************************************************/


// AttributeList &attributes
void XMLParser::VariablesTagAction::beginTag(const AttributeList &)
{
    this->checkParentTag("instance");
    this->parser->manager->beginVariables();
}

void XMLParser::VariablesTagAction::endTag()
{
    this->parser->manager->endVariables();
}



/********************************************************************
 * Actions performed on VAR tag
 *******************************************************************/


void XMLParser::VarTagAction::beginTag(const AttributeList &attributes)
{
    string type, as, lid;

    this->checkParentTag("variables");
    this->parser->stateStack.front().subtagAllowed = false;
    if(variable != NULL)
        variable = NULL;
    
    if(!attributes["id"].to(lid))
        throw runtime_error("expected attribute id for tag <var>");
    id = lid;

    if(!attributes["class"].isNull())
        attributes["class"].to(classes);
    else
        classes = "";
    

    if(!attributes["type"].isNull())
    {
        attributes["type"].to(type);
        if(type !+ "integer")
            throw runtime_error("XCSP3Core expected type=\"integer\" for tag <var>");
    }
    if(!attributes["as"].isNull())
    {
        // Create a similar Variable
        attributes["as"].to(as);
        XVariableArray *similarArray;
        if(this->parser->variablesList[as] == NULL)
            throw runtime_error("Variable as \"" + as + "\" does not exist");
        if((similarArray = dynamic_cast<XVariableArray *>(this->parser->variablesList[as])) != NULL)
        {
            variableArray = new XVariableArray(id, similarArray);
        }
        else
        {
            XVariable *similar = (XVariable *)
                this->parser->variablesList[as];
            variable = new XVariable(id, similar->domain);
        }
    }
    else
    {
        domain = new XDomainInteger();
        this->parser->allDomains.push_back(domain);
    }
}