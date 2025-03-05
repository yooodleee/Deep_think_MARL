
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


// UTF8String txt, bool last
void XMLParser::VarTagAction::text(const UTF8String txt, bool)
{
    if((variable != NULL || variableArray != NULL) && !txt.isWhiteSpace())
        throw runtime_error("<var> with attribute 'as' must not have domain declaration");
    this->parser->parseDomain(txt, *domain);
}


void XMLParser::VarTagAction::endTag()
{
    if(variableArray != NULL)
    {
        // Simuate an array
        this->parser->manager->beginVariableArray(variableArray->id);
        this->parser->variablesList[variableArray->id] = variableArray;
        for(XVariable *x : variableArray->variables)
            this->parser->variablesList[x->id] = x;
        this->parser->manager->buildVariableArray(variableArray);
        this->parser->manager->endVariableArray();
        return;
    }
    if(variable == NULL)
        variable = new XVariable(id, domain);
    variable->classes = classes;
    this->parser->variablesList[variable->id] = variable;
    this->parser->manager->buildVariable(variable);

}



/********************************************************************
 * Actions performed on ARRAY tag
 *******************************************************************/


void XMLParser::ArrayTagAction::beginTag(const AttributeList &attributes)
{
    string type, as, lid, size;

    this->checkParentTag("variables");
    varArray = NULL;
    domain = NULL;
    sizes.clear();

    if(!attributes["id"].to(lid))
        throw runtime_error("expected attribute id for tag <array>");
    id = lid;

    if(!attributes["class"].isNull())
        attributes["class"].to(classes);
    else
        classes = "";
    
    if(!attributes["type"].isNull())
    {
        attributes["type"].to(type);
        if(type != "integer")
            throw runtime_error("XCSP3Core expected type=\"integer\" for tag <var>");
    }


    if(!attributes["as"].isNull())
    {
        // Create a similar Variable
        attributes["as"].to(as);
        if(this->parser->variablesList[as] == nullptr)
            throw runtime_error("Matrix variable as \"" + "\" does not exist");
        XVariableArray *similar = (XVariableArray *)
            this->parser->variablesList[as];
        varArray = new XVariableArray(id, similar);
    }
    else
    {
        if(!attributes["size"].to(size))
            throw runtime_error("expected attribute id for tag <array>");
        vector<std::string> stringSizes = split(size, '[');
        for(unsigned int i = 0; i < stringSizes.size(); i++)
        {
            if(stringSizes[i].size() == 0)
                continue;
            sizes.push_back(std::stoi(stringSizes[i].substr(0, stringSizes[i].size() - 1)));
        }
        varArray = new XVariableArray(id, sizes);

        domain = new XDomainInteger();
        this->parser->allDomains.push_back(domain);
        this->parser->manager->beginVariableArray(id);
    }

    varArray->classes = classes;
}


void XMLParser::ArrayTagAction::endTag()
{
    // If domain is null -> as variable // Possible empty variables
    if(domain != nullptr && domain->nbValues() != 0)
        varArray->buildVarsWith(domain);
    this->parser->variablesList[varArray->id] = varArray;
    for(XVariable *x : varArray->variables)
    {
        if(x == nullptr)    // Undefined variable
            continue;
        this->parser->variablesList[x->id] = x;
    }
    this->parser->manager->buildVariableArray(varArray);
    this->parser->manager->endVariableArray();
}


void XMLParser::DomainTagAction::beginTag(const AttributeList &attributes)
{
    this->checkParentTag("array");
    attributes["for"].to(forAttr);
    if(forAttr == "others")
        d = ((XMLParser::ArrayTagAction *) this->parser->getParentTagAction())->domain;
    else
    {
        d = new XDomainInteger();
        this->parser->allDomains.push_back(d);
    }
}



// UTF8String txt, bool last
void XMLParser::DomainTagAction::text(const UTF8String txt, bool)
{
    this->parser->parseDomain(txt, *d);
}


void XMLParser::DomainTagAction::endTag()
{
    if(forAttr == "others")
        return;
    
    string name;
    vector<string> allCompactForms;
    vector<XVariable *> vars;
    XVariableArray *varArray = ((XMLParser::ArrayTagAction *) this->parser->getParentTagAction())->varArray;

    split(forAttr, ' ', allCompactForms);
    for(unsigned int i = 0; i < allCompactForms.size(); i++)
    {
        int pos = allCompactForms[i].find('[');
        name = allCompactForms[i].substr(0, pos);
        string compactForm = allCompactForms[i].substr(pos);
        
        vector<int> flatIndexes;
        vector<int> indexes;
        varArray->getVarFor(vars, compactForm, &flatIndexes, true);
        for(unsigned int j = 0; j < flatIndexes.size(); j++)
        {
            varArray->indexesFor(flatIndexes[j], indexes);
            varArray->variables[flatIndexes[j]] = new XVariable(varArray->id, d, indexes);
            this->parser->toFree.push_back(varArray->variables[flatIndexes[j]]);
        }
    }
}


void XMLParser::BasicConstraintTagAction::beginTag(const AttributeList &attributes)
{
    group = NULL;
    this->parser->star = false;

    // Classic group
    if(strcmp(this->parser->getParentTagAction()->getTagName(), "group") == 0)
        group = ((XMLParser::GroupTagAction *) this->parser->getParentTagAction())->group;
    
    // Group with meta constraint not or block
    if(strcmp(this->parser->getParentTagAction(2)->getTagName(), "group") == 0)
    {
        group = ((XMLParser::GroupTagAction *) this->parser->getParentTagAction(2))->group;
        exit(1);
    }

    // Group with not and block
    if(this->parser->getParentTagAction(3) != NULL &&
       strcmp(this->parser->getParentTagAction(3)->getTagName(), "group") == 0)

       group = ((XMLParser::GroupTagAction *) this->parser->getParentTagAction(3))->group;
    

    // Slide constraint (special cases of group without args)
    if(strcmp(this->parser->getParentTagAction()->getTagName(), "slide") == 0)
        group = ((XMLParser::SlideTagAction *) this->parser->getParentTagAction())->group;

    // Group with meta constraint not or block
    if(strcmp(this->parser->getParentTagAction(2)->getTagName(), "slide") == 0)
    {
        group = ((XMLParser::SlideTagAction *) this->parser->getParentTagAction(2))->group;
        exit(1);
    }

    // Group with not and block
    if(this->parser->getParentTagAction(3) != NULL &&
       strcmp(this->parser->getParentTagAction(3)->getTagName(), "slide") == 0)

       group = ((XMLParser::SlideTagAction *) this->parser->getParentTagAction(3))->group;
    

    attributes["id"].to(id);

    if(!attributes["class"].isNull())
        attributes["class"].to(this->parser->classes);
    else
        this->parser->classes = "";
    

    this->parser->listTag->nbCallsToList = 0;
    this->parser->lists.clear();
    this->parser->lists.push_back(vector<XVariable *>());
    this->parser->matrix.clear();
    this->parser->patterns.clear();


    this->parser->integers.clear();
    this->parser->values.clear();
    this->parser->widths.clear();
    this->parser->lengths.clear();
    this->parser->origins.clear();
    this->parser->transitions.clear();
    this->parser->nbParameters = 0;
    this->parser->occurs.clear();


    this->parser->star = false;
    this->parser->zeroIgnored = false;
    this->parser->condition = "";
    this->parser->rank = ANY;
    this->parser->index = NULL;
    this->parser->closed = true;

}



/********************************************************************
 * Actions performed on EXTENSION tag
 *******************************************************************/


void XMLParser::ExtensionTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintExtension(this->id, this->parser->classes);

    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = EXTENSION;
    }

}


void XMLParser::ExtensionTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->containStar = this->parser->star;

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintExtension(constraint);
        delete constraint;
    }
}


void XMLParser::IntensionTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintIntension(this->id, this->parser->classes);

    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = INTENSION;
    }
    fnc.clear();

}



// UTF8String txt, bool last
void XMLParser::IntensionTagAction::text(const UTF8String txt, bool)
{
    fnc.append(txt);
}


void XMLParser::IntensionTagAction::endTag()
{
    fnc.to(constraint->function);
    constraint->function = trim(constraint->function);
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    if(this->group == NULL)
    {
        this->parser->manager->newConstraintIntension(constraint);
        delete constraint;
    }

}



/********************************************************************
 ********************************************************************
 *                  CONSTRAINTS DEFINED ON LANGUAGES
 ********************************************************************
 *******************************************************************/

/********************************************************************
 * Actions performed on REGULAR tag
 *******************************************************************/


void XMLParser::RegularTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintRegular(this->id, this->parser->classes);

    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = REGULAR;
    }

}


void XMLParser::RegularTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin, this->parser->lists[0].end());
    constraint->start = this->parser->start;
    constraint->final.clear();
    split(this->parser->final, ' ', constraint->final);
    constraint->transitions.assign(this->parser->transitions.begin(), this->parser->transitions.end());

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintRegular(constraint);
        delete constraint;
    }

}



/********************************************************************
 * Actions performed on MDD tag
 *******************************************************************/


void XMLParser::MDDTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintMDD(this->id, this->parser->classes);

    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = MDD:
    }

}


void XMLParser::MDDTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->transitions.clear();
    for(unsigned int i = 0; i < this->parser->transitions.size(); i++)
    {
        XTransition &xt = this->parser->transitions[i];
        constraint->transitions.push_back(XTransition(xt.from, xt.val, xt.to));
    }

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintMDD(constraint);
        delete constraint;
    }

}



/********************************************************************
 ********************************************************************
 *                 CONSTRAINTS DEFINED ON COMPARISON
 ********************************************************************
 *******************************************************************/


/********************************************************************
 * Actions performed on ALLDIFF tag / ALLEQUAL
 *******************************************************************/


void XMLParser::AllDiffEqualTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    if(this->tagName == "allDifferent")
    {
        alldiff = new XConstraintAllDiff(this->id, this->parser->classes);
        ct = alldiff;
    }
    else
    {
        allequal = new XConstraintAllEqual(this->id, this->parser->classes);
        ct = allequal;
    }

    // Link constraint to group
    if(this->group != NULL)
    {
        if(this->tagName == "allDifferent")
            this->group->type = ALLDIFF;
        else
            this->group->type = ALLEQUAL;
        this->group->constraint = ct;
    }

}



// UTF8String txt, bool last
void XMLParser::AllDiffEqualTagAction::text(const UTF8String txt, bool)
{
    this->parser->parseSequence(txt, this->parser->lists[0]);
}


void XMLParser::AllDiffEqualTagAction::endTag()
{
    if((this->parser->lists() > 1 || this->parser->matrix.size() > 0) && this->group != NULL)
        throw runtime_error("AllDiff matrix and AllDiff lists are not implemented with groups");
    
    if(this->group == NULL)
    {
        if(this->tagName == "allDifferent")
        {
            if(this->parser->lists.size() > 1)
            {
                if(this->parser->integers.size() > 0)   // Except not implemented
                    throw runtime_error("except tag not allowed with alldiff on lists");
                
                XConstraintAllDiffList *ctl = new XConstraintAllDiffList(this->id, this->parser->classes);
                for(unsigned int i = 0; i < this->parser->lists.size(); i++)
                    ctl->matrix.push_back(vector<XVariable *>(this->parser->lists[i].begin(),
                                          this->parser->lists[i].end()));
                this->parser->manager->newConstraintAllDiffList(ct1);
                delete ctl;
                ct = ctl;
            }
            else
            {
                if(this->parser->matrix.size() > 0)
                {
                    // Matrix
                    XConstraintAllDiffMatrix *ctm = new XConstraintAllDiffMatrix(this->id, this->parser->classes, this->parser->matrix);
                    this->parser->manager->newConstraintAllDiffMatrix(ctm);
                    delete ct;
                    ct = ctm;
                }
                else
                {
                    // Alldiff classic
                    ct->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
                    alldiff->except.assign(this->parser->integers.begin(), this->parser->integers.end());
                    this->parser->manager->newConstraintAllDiff(alldiff);
                }
            }
        }
        else
        {
            ct->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
            this->parser->manager->newConstraintAllEqual(allequal);
        }
        delete ct;
    }
    else
    {
        ct->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    }

}



/********************************************************************
 * Actions performed on ORDERED tag
 *******************************************************************/


void XMLParser::OrderedTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintOrdered(this->id, this->parser->classes);
    string cs;
    attributes["case"].to(cs);
    if(cs == "strictlyDecreasing") this->parser->op = GT;
    if(cs == "decreasing") this->parser->op = GE;
    if(cs == "strictlyIncreasing") this->parser->op = LT;
    if(cs == "increasing") this->parser->op = LE;


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = ORDERED;
    }

}


// const UTF8String txt, bool last
void XMLParser::OrderedTagAction::text(const UTF8String txt, bool)
{
    this->parser->parseSequence(txt, this->parser->lists[0]);
}


void XMLParser::OrderedTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->op = this->parser->op;
    if(this->group == NULL)
    {
        if(this->parser->lengths.size() > 0)
            constraint->lengths.assign(this->parser->lengths.begin(), this->parser->lengths.end());
        
        this->parser->manager->newConstraintOrdered(constraint);
        delete constraint;
    }

}



/********************************************************************
 * Actions performed on LEX tag
 *******************************************************************/


void XMLParser::LexTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintLex(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = LEX;
    }

}


void XMLParser::LexTagAction::endTag()
{
    if(this->parser->matrix.size() > 0)
    {
        XConstraintLexMatrix *lexM = new XConstraintLexMatrix(this->id, this->parser->classes);
        lexM->op = this->parser->op;
        for(unsigned int i = 0; i < this->parser->matrix.size(); i++)
            lexM->matrix.push_back(vector<XVariable *>(this->parser->matrix[i].begin(),
                                                       this->parser->matrix[i].end()));
        
        this->parser->manager->newConstraintLexMatrix(lexM);
        delete lexM;
        delete constraint;
    }
    else
    {
        if(this->parser->lists.size() == 0)
            throw runtime_error("<lex> tag should have many lists");
        
        for(unsigned int i = 0; i < this->parser->lists.size(); i++)
            constraint->lists.push_back(vector<XVariable *>(this->parser->lists[i].begin(),
                                                            this->parser->lists[i].end()));
        constraint->op = this->parser->op;
        if(this->group == NULL)
        {
            this->parser->manager->newConstraintLex(constraint);
            delete constraint;
        }
    }

}



/********************************************************************
 ********************************************************************
 *                 COUNTING AND SUMMING CONSTRAINTS
 ********************************************************************
 *******************************************************************/


/********************************************************************
 * Actions performed on SUM tag
 *******************************************************************/


void XMLParser::SumTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintSum(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = SUM;
    }

}


void XMLParser::SumTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    if(this->parser->values.size() == 0)
        constraint->values.clear();
    else
        constraint->values.assign(this->parser->values.begin(), this->parser->values.end());
    
    constraint->condition = this->parser->condition;

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintSum(constraint);
        delete constraint;
    }

}


/********************************************************************
 * Actions performed on NVALUES tag
 *******************************************************************/


void XMLParser::NValuesTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintNValues(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = NVALUES;
    }

}


void XMLParser::NValuesTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->condition = this->parser->condition;
    constraint->except.assign(this->parser->integers.begin(), this->parser->integers.end());
    if(this->group == NULL)
    {
        this->parser->manager->newConstraintNValues(constraint);
        delete constraint;
    }

}


/********************************************************************
 * Actions performed on COUNT tag
 *******************************************************************/


void XMLParser::CountTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintCount(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = COUNT;
    }

}


void XMLParser::CountTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->values.assign(this->parser->values.begin(), this->parser->values.end());
    constraint->condition = this->parser->condition;
    if(this->group == NULL)
    {
        this->parser->manager->newConstraintCount(constraint);
        delete constraint;
    }

}


/********************************************************************
 * Actions performed on CARDINALITY tag
 *******************************************************************/


void XMLParser::CardinalityTagAction::beginTag(const AttributeList &attributes)
{
    // Must be inside a constraint
    BasicConstraintTagAction::beginTag(attributes);
    this->parser->closed = false;
    constraint = new XConstraintCardinality(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = CARDINALITY;
    }

}


void XMLParser::CardinalityTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->values.assign(this->parser->values.begin(), this->parser->values.end());
    constraint->occurs.assign(this->parser->occurs.begin(), this->parser->occurs.end());
    constraint->closed = this->parser->closed;
    if(this->group == NULL)
    {
        this->parser->manager->newConstraintCardinality(constraint);
        delete constraint;
    }

}



/********************************************************************
 ********************************************************************
 *                     CONNECTION CONSTRAINTS
 ********************************************************************
 *******************************************************************/


/********************************************************************
 * Actions performed on CHANNEL tag
 *******************************************************************/


void XMLParser::ChannelTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintChannel(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = CHANNEL;
    }

}


// const UTF8String txt, bool last
void XMLParser::ChannelTagAction::text(const UTF8String txt, bool)
{
    // if (this->parser->list.size() == 0)
    this->parser->parseSequence(txt, this->parser->lists[0]);
}


void XMLParser::ChannelTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->startIndex1 = this->parser->startIndex;

    if(this->parser->values.size() == 1)
        constraint->value = this->parser->values[0];
    else if(this->parser->values.size() > 1)
        throw runtime_error("<value> tag accepts only one value");
    
    if(this->parser->lists.size() == 2)
    {
        constraint->secondList.assign(this->parser->lists[1].begin(), this->parser->lists[1].end());
        constraint->startIndex2 = this->parser->startIndex2;
    }

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintChannel(constraint);
        delete constraint;
    }
}


/********************************************************************
 * Actions performed on ELEMENT tag
 *******************************************************************/


void XMLParser::ElementTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintElement(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = ELEMENT;
    }
}


void XMLParser::ElementTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->startIndex = this->parser->startIndex;
    constraint->index = this->parser->index;
    constraint->rank = this->parser->rank;

    if(this->parser->values.size() != 1)
        throw runtime_error("<element> tag should have one value");
    constraint->value = this->parser->values[0];

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintElement(constraint);
        delete constraint;
    }
}


/********************************************************************
 * Actions performed on MAXIMUM or MINIMUM tag
 *******************************************************************/


void XMLParser::MinMaxTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraints = new XConstraintMaximum(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        if(this->tagName == "maximum")
            this->group->type = MAXIMUM;
        else
            this->group->type = MINIMUM;
    }
}


void XMLParser::MinMaxTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->startIndex = this->parser->startIndex;
    constraint->condition = this->parser->condition;
    constraint->index = this->parser->index;
    constraint->rank = this->parser->rank;

    if(this->group == NULL)
    {
        if(this->tagName == "maximum")
            this->parser->manager->newConstraintMaximum(constraint);
        else
            this->parser->manager->newConstraintMinimum(constraint);
        delete constraint;
    }
}



/********************************************************************
 ********************************************************************
 *                PACKING and SCHEDULING CONSTRAINTS
 ********************************************************************
 *******************************************************************/


/********************************************************************
 * Actions performed on STRETCH tag
 *******************************************************************/


void XMLParser::StretchTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintStretch(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = STRETCH;
    }
}


void XMLParser::StretchTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    for(XEntity *xi : this->parser->values)
    {
        int v;
        isInteger(xi, v);
        constraint->values.push_back(v);
    }

    for(unsigned int i = 0; i < this->parser->widths.size(); i++)
    {
        XIntegerInterval *xrange = dynamic_cast<XIntegerInterval *>(this->parser->widths[i]);
        constraint->widths.push_back(XInterval(xrange->min, xrange->max));
    }


    if(this->parser->patterns.size() > 0)
    {
        constraint->patterns.reserve(this->parser->patterns.size());
        for(unsigned int i = 0; i < this->parser->patterns.size(); i++)
            constraint->patterns[i].assign(this->parser->patterns[i].begin(), this->parser->patterns[i].end());
    }

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintStretch(constraint);
        delete constraint;
    }
}


/********************************************************************
 * Actions performed on NOOVERLAP tag
 *******************************************************************/


void XMLParser::NoOverlapTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);
    diffn = false;
    constraint = new XConstraintNoOverlap(this->id, this->parser->classes);
    if(!attributes["zeroIgnored"].isNull())
    {
        string tmp;
        attributes["zeroIgnored"].to(tmp);
        this->parser->zeroIgnored = (tmp == "true");
    }
    else
        this->parser->zeroIgnored = true;

    
    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = NOOVERLAP;
    }
}


void XMLParser::NoOverlapTagAction::endTag()
{
    constraint->list.assign(this->parser->origins.begin(), this->parser->origins.end());
    constraint->lengths.assign(this->parser->lengths.begin(), this->parser->lengths.end());
    constraint->zeroIgnored = this->parser->zeroIgnored();
    if(this->group == NULL)
    {
        this->parser->manager->newConstraintNoOverlap(constraint);
        delete constraint;
    }
}


/********************************************************************
 * Actions performed on CUMULATIVE tag
 *******************************************************************/


void XMLParser::CumulativeTagAction::beginTag(const AttributeList &attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintCumulative(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constrainta = constraint;
        this->group->type = CUMULATIVE;
    }
}


void XMLParser::CumulativeTagAction::endTag()
{
    constraint->list.assign(this->parser->origins.begin(), this->parser->origins.end());
    constraint->lengths.assign(this->parser->lengths.begin(), this->parser->lengths.end());
    constraint->ends.assign(this->parser->ends.begin(), this->parser->ends.end());
    constraint->heights.assign(this->parser->heights.begin(), this->parser->heights.end());
    constraint->condition = this->parser->condition;

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintCumulative(constraint);
        delete constraint;
    }
}



/********************************************************************
 ********************************************************************
 *                  CONSTRAINTS DEFINED ON GRAPHS
 ********************************************************************
 *******************************************************************/


void XMLParser::CircuitTagAction::beginTag(const AttributeList &attributes)
{
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintCircuit(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = CIRCUIT;
    }
    this->parser->values.clear();
}


// UTF8String txt, bool last
void XMLParser::CircuitTagAction::text(const UTF8String txt, bool)
{
    this->parser->parseSequence(txt, this->parser->lists[0]);
}


void XMLParser::CircuitTagAction::endTag()
{
    constraint->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->startIndex = this->parser->startIndex;
    if(this->parser->values.size() == 1)
        constraint->value = this->parser->values[0];
    else
    {
        if(this->parser->values.size() == 0)
            constraint->value = nullptr;
        else
            throw runtime_error("<size> tag accepts only one value");
    }

    if(this->group == NULL)
    {
        this->parser->manager->newConstraintCircuit(constraint);
        delete constraint;
    }
}



/********************************************************************
 ********************************************************************
 *                          OBJECTIVES
 ********************************************************************
 *******************************************************************/


// AttributeList &attribues
void XMLParser::ObjectivesTagAction::beginTag(const AttributeList &)
{
    objective = new XObjective();
    this->parser->expr = "";
    this->checkParentTag("instance");
    this->parser->lists.clear();
    this->parser->listTag->nbCallsToList = 0;
    this->parser->integers.clear();
    this->parser->values.clear();
    this->parser->lists.push_back(vector<XVariable *>());
    this->parser->manager->beginObjectives();
}


void XMLParser::ObjectivesTagAction::endTag()
{
    if(this->parser->expr != "")
        objective->expression = this->parser->expr;
    
    if(this->parser->lists[0].size() > 0)
        objective->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    if(this->parser->values.size() > 0)
    {
        int value;
        for(XEntity *xe : this->parser->values)
        {
            isInteger(xe, value);
            objective->coeffs.push_back(value);
        }
    }
    else if(objective->type != EXPRESSION_O)
    {
        objective->coeffs.assign(objective->list.size(), 1);
    }

    this->parser->manager->addObjective(objective);
    delete objective;
    this->parser->manager->endObjectives();

}


void XMLParser::MinimizeOrMaximizeTagAction::beginTag(const AttributeList &attributes)
{
    obj = ((XMLParser::ObjectivesTagAction *) this->parser->getParentTagAction())->objective;
    obj->goal = (this->tagName == "minimize") ? MINIMIZE : MAXIMIZE;
    string tmp;
    this->checkParentTag("objectives");

    attributes["type"].to(tmp);
    obj->type = EXPRESSION_O;
    if(tmp == "sum") obj->type = SUM_O;
    if(tmp == "product") obj->type = PRODUCT_O;
    if(tmp == "minimum") obj->type = MINIMUM_O;
    if(tmp == "maximum") obj->type = MAXIMUM_O;
    if(tmp == "nValues") obj->type = NVALUES_O;
    if(tmp == "lex") obj->type = LEX_O;
    
}


// UTF8String txt, bool last
void XMLParser::MinimizeOrMaximizeTagAction::text(const UTF8String txt, bool)
{
    string ip;
    txt.to(op);
    if(trim(op) == "") return;  // skip white space.

    if(obj->type == EXPRESSION_O)
        this->parser->expr = op;
    else
        this->parser->parseSequence(txt, this->parser->lists[0]);
}



/********************************************************************
 * Actions performed on list of variables integers... tag
 *******************************************************************/


// UTF8String txt, bool last
void XMLParser::ListOfIntegerTagAction::text(const UTF8String txt, bool)
{
    UTF8String::Tokenizer tokenizer(txt);
    while(tokenizer.hasMoreTokens())
    {
        UTF8String token = tokenizer.nextToken();
        int c = -1;
        token.to(c);
        this->parser->integers.push_back(c);
    }
}


void XMLParser::ListOfVariablesOrIntegerTagAction::beginTag(const AttributeList &attributes)
{
    listToFill.clear();
    if(!attributes["closed"].isNull())
    {
        string tmp;
        attributes["closed"].to(tmp);
        this->parser->closed = (tmp == "true");
    }
}


// UTF8String txt, bool last
void XMLParser::ListOfVariablesOrIntegerTagAction::text(const UTF8String txt, bool)
{
    this->parser->parseSequence(txt, listToFill);
}


// AttributeList &attributes
void XMLParser::ListOfVariablesOrIntegerOrIntervalTagAction::beginTag(const AttributeList &)
{
    listToFill.clear();
}


// UTF8String txt, bool last
void XMLParser::ListOfVariablesOrIntegerOrIntervalTagAction::text(const UTF8String txt, bool)
{
    this->parser->keepIntervals = true;
    this->parser->parseSequence(txt, listToFill);
    this->parser->keepIntervals = false;
}


// AttributeList &attributes) {
void XMLParser::ListOfIntegerOrIntervalTagAction::beginTag(const AttributeList &)
{
    listToFill.clear();
}


// UTF8String txt, bool last
void XMLParser::ListOfIntegerOrIntervalTagAction::text(const UTF8String txt, bool)
{
    this->parser->parseListOfIntegerOrInterval(txt, listToFill);
}



/********************************************************************
 * Actions performed on ORIGINS tag
 *******************************************************************/


// AttributeList &attributes
void XMLParser::OriginsTagAction::beginTag(const AttributeList &)
{
    listToFill.clear();
}


// UTF8String txt, bool last
void XMLParser::OriginsTagAction::text(const UTF8String txt, bool)
{
    vector<char> delims;
    delims.push_back('(');
    delims.push_back(')');
    delims.push_back(',');
    this->parser->parseSequence(txt, listToFill, delims);
}



/********************************************************************
 * Actions performed on ARGS tag
 *******************************************************************/


// AttributeList &attributes
void XMLParser::ArgsTagAction::beginTag(const AttributeList &)
{
    this->parser->args.clear();
}


// UTF8String txt, bool last
void XMLParser::ArgsTagAction::text(const UTF8String txt, bool)
{
    this->parser->parseSequence(txt, this->parser->args);
}


void XMLParser::ArgsTagAction::endTag()
{
    XConstraintGroup *group = ((GroupTagAction *) this->parser->getParentTagAction())->group;
    group->arguments.push_back(vector<XVariable *>(this->parser->args.begin(),
                                                   this->parser->args.end()));
}



/********************************************************************
 * Actions performed on Operator tag
 *******************************************************************/


// UTF8String txt, bool last
void XMLParser::OperatorTagAction::text(const UTF8String txt, bool)
{
    string op;
    txt.to(op);
    if(trim(op) == "") return;  // skip white space.
    if(op == "lt") this->parser->op = LT;
    if(op == "le") this->parser->op = LE;
    if(op == "gt") this->parser->op = GT;
    if(op == "ge") this->parser->op = GE;
}


// UTF8String txt, bool last
void XMLParser::StringTagAction::text(const UTF8String txt, bool)
{
    string tmp;
    txt.to(tmp);
    tmp = trim(tmp);
    if(tmp == "")
        return;
    if(this->tagName == "final")
    {
        this->parser->final = tmp;
    }
    if(this->tagName == "start")
        this->parser->start = tmp;
}


/********************************************************************
 * Actions performed on instantiation tag
 *******************************************************************/


void XMLParser::InstantiationTagAction::beginTag(const AttributeList *attributes)
{
    // Must be called inside a constraint
    BasicConstraintTagAction::beginTag(attributes);

    constraint = new XConstraintInstantiation(this->id, this->parser->classes);


    // Link constraint to group
    if(this->group != NULL)
    {
        this->group->constraint = constraint;
        this->group->type = INSTANTIATION;
    }
}


void XMLParser::InstantiationTagAction::endTag()
{
    constraints->list.assign(this->parser->lists[0].begin(), this->parser->lists[0].end());
    constraint->values.clear();
    for(XEntity *xi : this->parser->values)
    {
        int v;
        isInteger(xi, v);
        constraint->values.push_back(v);
    }
    if(this->group == NULL)
    {
        this->parser->manager->newConstraintInstantiation(constraint);
        delete constraint;
    }
}


void XMLParser::ListTagAction::beginTag(const AttributeList &attributes)
{
    nbCallsToList++;
    if(nbCallsToList > 1)
    {
        this->parser->lists.push_back(vector<XVariable *>());
        this->parser->startIndex2 = 0;
        if(!attributes["startIndex"].isNull())
            attributes["startIndex"].to(this->parser->startIndex2);
    }
    else
    {
        this->parser->startIndex = 0;
        if(!attributes["startIndex"].isNull())
            attributes["startIndex"].to(this->parser->startIndex);
    }
    if(!attributes["offset"].isNull())
    {
        SlideTagAction *slide = ((XMLParser::SlideTagAction *) this->parser->getParentTagAction());
        attributes["offset"].to(slide->offset);
    }
}


// UTF8String txt, bool last
void XMLParser::ListTagAction::text(const UTF8String txt, bool)
{
    this->parser->parseSequence(txt, this->parser->lists.back());
}


void XMLParser::ListTagAction::endTag()
{
    if(this->parser->getParentTagAction() != NULL &&
       strcmp(this->parser->getParentTagAction()->getTagName(), "slide") == 0)
    {
        assert(this->parser->lists.size() == 1);
        SlideTagAction *sl = ((XMLParser::SlideTagAction *) this->parser->getParentTagAction());
        sl->list.insert(sl->list.begin(), this->parser->lists[0].begin(), this->parser->lists[0].end());
        nbCallsToList = 0;
        this->parser->lists[0].clear(); // for sure.
    }

}


// AttributeList &attributes
void XMLParser::ConflictOrSupportTagAction::beginTag(const AttributeList &)
{
    bool support = true;
    this->checkParentTag("extension");

    this->parser->star = false;
    if(this->tagName == "conflicts")
        support = false;
    
    ((XMLParser::ExtensionTagAction *) this->parser->getParentTagAction())->constraint->isSupport = support;
    
}