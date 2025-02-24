
#include <csignal>


using namespace std;


#include "InfoSystem.hh"
#include "XCSP3CoreParser.hh"
#include "main.hh"


using namespace XCSP3Core;


Solver* x;


void handle_interrupt(int sig)
{
    s->showFinal(2);
    exit(0);
}   // handle_interrupt


int main(int argc, char** argv)
{
    XCSP3Callbacks cb;  // interface between the parser and the solver

    if (argc < 3) {
        if (argc == 1) {
            show_help();
            exit(0);
        }
        std::string arg = argv[1];
        if (!arg.compare("-moreHelp"))
            show_advanced_help();
        else
            show_help();
        exit(0);
    }

    try {
        XCSP3CoreParser parser(&cb);
        parser.parse(argv[1]);
        argv++;
        argc--;

        signal(SIGINT, handle_interrupt);
        signal(SIGTERM, handle_interrupt);
        signal(SIGALRM, handle_interrupt);

        s = create_solver(0, argc, argv);

        exit(s->showFinal(s->solve()));

    } catch (exception& e) {
        cout.flush();
        cerr << "\n\tUnexpected exception :\n";
        cerr << "\t" << e.what() << endl;
        exit(1);
    }

    return 0;
}