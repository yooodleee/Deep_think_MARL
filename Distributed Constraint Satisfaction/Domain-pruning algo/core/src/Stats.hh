
#ifndef STATS_H_
#define STATS_H_


namespace Stats {

// CONSTRAINTS
extern unsigned long long nbConstraints;
extern unsigned long long saveNbConstraints;
extern unsigned long long minArity;
extern unsigned long long maxArity;


// NOGOODS - CLAUSES
extern unsigned long long nbSupp;
extern unsigned long long nbConflict;
extern unsigned long long nbNoGoods;
extern unsigned long long unit;
extern unsigned long long binary;
extern unsigned long long unitVar;


// AC
extern unsigned long long assignCall;
extern unsigned long long rmCall;
extern unsigned long long nbPropCalls;
extern unsigned long long nbDecisions;


// SOLVER
extern unsigned long long run;
extern unsigned long long conflict;
extern unsigned long long nbSols;
extern unsigned long long nbRst;


// VARS
extern unsigned long long nbVarProps;
extern unsigned long long saveNbVarProps;
extern unsigned long long nbVars;
extern unsigned long long uselessVars;


void print_after_init();


void print_all_stats();
}



#endif  // STATS_H_