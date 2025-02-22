
#ifndef RESTART_H_
#define RESTART_H_


#include "Options.hh"
#include "Stats.hh"


#include <math.h>


/**
 * Handle restart policy according to a parameter.
 */
class Restart {
private:
    int nbRst;
    int nbConflictBfRst;
    double nbConflictBfRstD;
    int Luby(int i);


public:
    /// Constructor
    Restart()
        : nbRst(0)
        , nbConflictBfRst(Options::baseRst)
        , nbConflictBfRstD(static_cast<double>(Options::baseRst)) {};
    

    /// Increase @nbRst
    void Inc();


    /// Return the num of allowed conflict before a restart occurs
    int GetNbConflictAllowed() { return nbConflictBfRst; };
};



#endif  // RESTART_H_