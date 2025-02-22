
#include "Restart.hh"
#include "Variable.hh"



void Restart::Inc()
{
    Stats::run++;
    /* Variable::gblOrder = 0; */
    nbRst++;
    
    switch (Options::optRst) {
    case rstTypes::hundred_perc:
        nbConflictBfRst += nbConflictBfRst;
        break;
    
    case rstTypes::luby10:
        nbConflictBfRst = (Luby(nbRst + 1) * 10);
        break;
    
    case rstTypes::luby100:
        nbConflictBfRst = (Luby(nbRst + 1) * 100):
        break;

    case rstTypes::luby50:
        nbConflictBfRst = (Luby(nbRst + 1) * 50);
        break;
    
    case rstTypes::low:
        nbConflictBfRstD *= 1.03;
        nbConflictBfRst = static_cast<int>(floor(nbConflictBfRstD));
        break;

    case rstTypes::ten_perc:
        nbConflictBfRstD *= 1.1;
        nbConflictBfRst = static_cast<int>(floor(nbConflictBfRstD));
        break;

    default:
        nbConflictBfRst += nbConflictBfRst >> 1;
        break;
    }
}



int Restart::Luby(int i)
{
    int k = 1;
    int power = 2;

    /// let 2^k be the least power of 2 >= (i+1)
    for (; power < (i + 1); k += 1, power *= 2)
        ;
    
    if (power == i + 1)
        return power / 2;
    
    return Luby(i - (power / 2) + 1);
}