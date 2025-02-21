
#include "LecoutreCompleteSolver.hh"
#include "InfoSystem.hh"
#include "LecoutreNoGoodsManager.hh"

using namespace std;



int LecoutreCompleteSolver::showFinal(int res)
{
    if (Options::Verbose != verbose::no && manager)
        cerr << *manager << endl;
    return CompleteSolver::showFinal(res);
}


