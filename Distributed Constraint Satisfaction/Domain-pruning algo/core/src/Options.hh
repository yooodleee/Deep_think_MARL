
#ifndef OPTIONS_H_
#define OPTIONS_H_


#include <iostream>


enum class valueSelectionTypes { zero,
    min,
    max,
    vrand,
    last };


enum class VariableHeuristic { dom,
    domdeg,
    domwdeg };


enum class countTypes { no,
    all };


enum class verbose { no,
    low,
    medium,
    high };


enum class rstTypes { noRst,
    low,
    fifty_perc,
    luby10,
    luby50,
    luby100,
    hundred_perc,
    ten_perc };


namespace Options {
extern int baseRst;
extern rstTypes optRst;
extern countTypes optRst;
extern valueSelectionTypes valueSelection;
extern bool assign;
extern bool saving;
extern VariableHeuristic varHueristic;
extern verbose Verbose;
extern bool reduceDBOpt;
extern bool keepBinary;


/// Load Options from program args
bool load_options(int& argc, char** argv, int& method);
}



#endif  // OPTIONS_H_