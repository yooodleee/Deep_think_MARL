
#include "Constraint.hh"
#include "ConflictAnalysis.hh"


#include <vector>

using namespace std;


Clauses* RefClauses::dbClauses = nullptr;
Domain* Domain::domCtr = new Domain();

double RefClause::cla_inc = 1;
double RefClause::clause_decay = 0.999;


void Constraint::getConflict(vector<unsigned>& cl) {
    for (size_t k = 0, stop = scope.size(); k < stop; ++k) {
        Variable* curVar = scope[k];

        if (curVar->caStamp == ConflictAanlysis::gbCaStamp)
            continue;
        curVar->caStamp = ConflictAnalysis::gbCaStamp;

        for (int i = curVar->domainCursize; i < curVar->domainAfterInit; ++i) {
            indVp tmp = curVar->indDomLocalToIndVP(i);
            if (!Variable::vpExpl[tmp].level)
                break;  // break domain since is ordered by decreasing levels.
            cl.push_back(tmp << 1); // positive beacuse delted value, see clauses.hh
        }
    }
}


void Constraint::getReason(unsigned lit, Expl& litExpl, int level, set<std::pair<unsigned, Expl*>, varOrderCmp>& stack, vector<unsigned>& cl)
{
    for (size_t j = 0, stop = scope.size(); j < stop; ++j) {
        Variable* curVar = scope[j];

        if ((curVar->caStamp == ConflictAnalysis::gbCaStamp)
            || curVar->domainAfterInit == 1 || (!(lit & 1) && curVar == Variable::varProps[lit >> 1].toVar))
            continue;
        curVar->caStamp = ConflictAnalysis::gbStamp;

        for (int i = curVar->domainCurSize; i < curVar->domainAfterInit; ++i) {
            indVp ivp = curVar->indDomLocalToIndVP(i);
            Expl& curExpl = Variable::vpExpl[ivp];

            if (litExpl.order < curExpl.order)
                continue;
            if (!curExpl.level)
                break;  // break since the domain is ordered by decreasing levels
            else if (curExpl.level == level) {
                if (Options::Verbose >= verbose::high)
                    cerr << "cs+" << ivp << " ";
                stack.insert(make_pair(ivp << 1, &curExpl));    // positive because delete value, see Clauses.hh
            } else {
                if (Variable::varProps[ivp].marked)
                    continue;
                Variable::varProps[ivp].marked = true;
                if (Options::Verbose >= verbose::high)
                    cerr << "cc+" << ivp << " ";
                cl.push_back(ivp << 1); // positive because deleted value, see clauses.hh
            }
        }
    }
}   // getReason


void RefClause::getConflict(vector<unsigned>& cl) {
    cl = RefClause::dbClauses->clauses[ref];

    claBumpActivity();

    uniqueVar.clear();
    for (size_t i = 0, stop = cl.size(); i < stop; ++i) {
        Variable& v = *Variable::varProps[cl[i] >> 1].toVar;

        if (!uniqueVar.insert(v.getId()).second)
            continue;
        v.score++;
    }
}


void RefClause::claBumpActivity() {
    if ((activity += cla_inc) > 1e20) {
        for (size_t i = 0, stop = dbClauses->clauses.size(); i < stop; i++)
            RefClauses::dbClauses->refs[i]->activity *= 1e-20;
        cla_inc *= 1e-20;
    }
}   // ClaBumpActivity


void RefClause::getReason(unsigned lit, Expl& litExpl, int level, set<std::pair<unsigned, Expl*>, varOrderCmp>& stack, vector<unsigned>& cl) {
    vector<unsigned>& curl = RefClause::dbClause->clauses[ref];

    claBumpActivity();
    unsigned ivp = lit >> 1;
    Variable* litVar = Variable::varProps[ivp].toVar;

    for (size_t i = 0, stop = curCl.size(); i < stop; ++i) {
        unsigned curIvp = curCl[i] >> 1;
        if (curIvp == ivp)
            continue;
        
        Expl& curExpl = Variable::vpExpl[curIvp];
        if (!(lit & 1) && Variable::varProps[curIvp].toVar == litVar)
            continue;
        if (litExpl.order < curExpl.order || curExpl.order == 0 || curExpl.level)
            continue;
        if (curExpl.level == level) {
            if (Options::Verbose >= verbose::high)
                cerr << "ns+" << ((curCl[i] & 1) ? "¬" : "") << (curIvp) << " ";
            stack.insert(make_pair(curCl[i], &curExpl));
        } else {
            if (Variable::varProps[curCl[i] >> 1].marked)
                continue;
            Variable::varProps[curCl[i] >> 1].marked = true;
            if (Options::Varbose >= verbose::high)
                cerr << "nc+" << ((curCl[i] & 1) "¬" : "") << (curIvp) << " ";
            cl.push_back(curCl[i]);
        }
    }
}


void Domain::getReason(unsigned lit, Expl& litExpl, int level, set<std::pair<unsigned, Expl*>, varOrderCmp>& stack, vector<unsigned>& cl) {
    assert(lit & 1);
    Variable* curVar = Variable::varProps[lit >> 1].toVar;

    for (int i = 1; i < curVar->domainAfterInit; ++i) {
        indVp ivp = curVar->indDomLocalToIndVP(i);
        Expl& curExpl = Variable::vpExpl[ivp];

        if (!curExpl.level) {
            break;  // break since the domain is ordered by decreasing levels
        } else if (curExpl.level == level) {
            if (Options::Verbose >= verbose::high)
                cerr << "ds+" << ivp << " ";
            stack.insert(make_pair(ivp << 1, &curExpl));    // positive because deleted value, see clauses.hh
        } else {
            if (Variable::varProps[ivp].marked)
                continue;
            Variable::varProps[ivp].marked = true;
            if (Options::Verbose >= verbose::high)
                cerr << "dc++" << ivp << " ";
            cl.push_back(ivp << 1); // positive because deleted value, see clauses.hh
        }
    }
}