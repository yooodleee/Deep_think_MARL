
#include "XCSP3CoreParser.h"
#include "XCSP3PrintCallbacks.h"


using namespace XCSP3Core;


int main(int argc, char **argv) {
    XCSP3PrintCallbacks cb; // interface between the parser and the solver

    if (argc != 2)
        throw std::runtime_error("usage: ./test xcsp3instance.xml");

    try
    {
        XCSP3CoreParser parser(&cb);
        parser.parse(argv[1]);  // fileName is a string
    }
    catch (exception &e)
    {
        cout.flush();
        cerr << "\n\tUnexpected exception :\n";
        cerr << "\t" << e.what() << endl;
        exit(1);
    }

    return 0;
}