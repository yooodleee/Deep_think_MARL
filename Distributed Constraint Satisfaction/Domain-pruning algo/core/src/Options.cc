
#include "Options.hh"


#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>


namespace Options {
int baseRst = 10;
rstTypes optRst = rstTypes::low;
valueSelectionTypes valueSelection = valueSelectionTypes::min;
bool assign = true;
bool saving = false;
countTypes countSols = countTypes::no;
VariableHeuristic varHueristic = VariableHeuristic::domwdeg;
verbose Verbose = verbose::low;
bool reduceDBOpt = true;
bool keepBinary = true;
}


bool Options::load_options(int& argc, char** argv, int& method)
{
    std::string strMethod = argv[1];
    if (!strMethod.compare("-moreHelp")) {
        method = 0;
    } else if (!strMethod.compare("-complete")) {
        method = 1;
    } else if (!strMethod.compare("-incng"))
        method = 2;
    else if (!strMethod.compare("-ngen"))
        method = 3;
    else if (!strMethod.compare("-nld"))
        method = 4;
    else if (!strMethod.compare("-ca"))
        method = 5;
    else if (!strMethod.compare("-infoCSP"))
        method = 10;
    else
        return true;
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];


        // Count solutions
        if (!arg.compare("-sols")) {
            Options::countSols = countTypes::all;
            if (method == 1) {
                Options::optRst = rstTypes::noRst;
                baseRst = std::numeric_limits<int>::max();
            }
        }


        // Restarts
        else if (!arg.compare("noRst")) {
            Options::optRst = rstTypes::noRst;
            baseRst = std::numeric_limits<int>::max();
        } else if (!arg.compare("-double")) {
            Options::optRst = rstTypes::hundred_perc;
        } else if (!arg.compare("-50perc")) {
            Options::optRst = rstTypes::fifty_perc;
        } else if (!arg.compare("-10perc")) {
            Options::optRst = rstTypes::ten_perc;
        } else if (!arg.compare("-luby10")) {
            Options::optRst = rstTypes::luby10;
            baseRst = 10;
        } else if (!arg.compare("-luby100") || !arg.compare("-l100")) {
            Options::optRst = rstTypes::luby100;
            baseRst = 100;
        } else if (!arg.compare("-luby50")) {
            Options::optRst = rstTypes::luby50;
            baseRst = 50;
        }


        // Heuristics -- Values
        else if (!arg.compare("-valMin")) {
            Options::valueSelection = valueSelectionTypes::min;
        } else if (!arg.compare("-valMax")) {
            Options::valueSelection = valueSelectionTypes::max;
        } else if (!arg.compare("-valRand")) {
            Options::valueSelection = valueSelectionTypes::vrand;
        } else if (!arg.compare("-valFirst")) {
            Options::valueSelection = valueSelectionTypes::zero;
        } else if (!arg.compare("-valLast")) {
            Options::valueSelection = valueSelectionTypes::last;
        }


        // Heuristics -- Variables
        else if (!arg.compare("-dom")) {
            Options::varHueristic = VariableHeuristic::dom;
        } else if (!arg.compare("-domdeg")) {
            Options::varHueristic = VariableHeuristic::domdeg;
        } else if (!arg.compare("-domwdeg") || !arg.compare("-dwd")) {
            Options::varHueristic = VariableHeuristic::domwdeg;
        }


        // Heuristics -- Miscellaneous
        else if (!arg.compare("-refutation")) {
            Options::assign = false;
        } else if (!arg.compare("-saving")) {
            Options::saving = true;
        }


        // Conflict analysis options
        else if (!arg.compare("-noReduceDB")) {
            Options::reduceDBOpt = false;
        } else if (!arg.compare("-noBinaryCL")) {
            Options::keepBinary = false;
        }


        // Verbosity
        else if (!arg.compare("-verb=0") || !arg.compare("-cm")) {
            Options::Verbose = verbose::no;
        } else if (!arg.compare("-verb=1")) {
            Options::Verbose = verbose::low;
        } else if (!arg.compare("-verb=2")) {
            Options::Verbose = verbose::medium;
        } else if (!arg.compare("-verb=3")) {
            Options::Verbose = verbose::high;
        }

        else {
            std::cerr << "Unknow option \"" << arg << "\"." << std::endl;
            return true;
        }
    }

    return false;
}