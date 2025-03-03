
#include "XCSP3Tree.h"

using namespace XCSP3Core;
using namespace std;


int main() {

    // Example 1 :
    Tree t("eq(z,add(x,3))");
    t.prefixe();
    std::cout << std::endl;
    std::map<string, int> tuple;
    tuple["z"] = 4;
    tuple["x"] = 1;
    std::cout << "z=4 and x=1" << std::endl;
    std::cout << "result: " << t.evaluate(tuple) << std::endl;


    // Same variable appear many times
    Tree t2("eq(z,mul(x,x))");
    t2.prefixe();
    std::cout << std::endl;
    std::map<string, int> tuple2;
    tuple2["z"] = 4;
    tuple2["x"] = 1;    
    std::cout << "z=4 and x=1" << std::endl;
    std::cout << "result: " << t2.evaluate(tuple2) << std::endl;


    std::map<string, int> tuple3;
    tuple2["z"] = 4;
    tuple2["z"] = 2;
    std::cout << "z=4 and x=2" << std::endl;
    std::cout << "result: " << t2.evaluate(tuple3) << std::endl;
}