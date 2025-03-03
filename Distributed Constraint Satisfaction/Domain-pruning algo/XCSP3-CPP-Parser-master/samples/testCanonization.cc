
#include "XCSP3Manager.h"

using namespace XCSP3Core;

extern int equalNodes(Node* a, Node* b);


int main(int argc, char* argv) {
    int nbFailed = 0;
    int nbSuccess = 0;
    std::vector<std::pair<std::string, std::string>> allTests;


    allTests.push_back(std::make_pair("not(eq(y[3],y[2]))", "ne(y[2],y[3])"));
    allTests.push_back(std::make_pair("and(not(not(eq(y[0],3))),0)", "and(eq(y[0],3),0)"));
    allTests.push_back(std::make_pair("abs(sub(y[0],y[1]))", "dist(y[0],y[1])"));
    allTests.push_back(std::make_pair("eq(mul(3,y[3]),6)", "eq(y[3],2)"));
    allTests.push_back(std::make_pair("and(lt(10,y[3]),lt(y[3],20))", "and(le(y[3],19),le(11,y[3]))"));
    allTests.push_back(std::make_pair("or(lt(y[3],10),gt(y[3],20))", "or(le(y[3],9),le(21,y[3]))"));
    allTests.push_back(std::make_pair("le(sub(x[0],4),y[3])", "le(x[0],add(y[3],4))"));
    allTests.push_back(std::make_pair("lt(5,x[0])", "le(6,x[0])"));
    allTests.push_back(std::make_pair("lt(x[0],5)", "le(x[0],4)"));
    allTests.push_back(std::make_pair("lt(add(y[4],5),8)", "le(y[4],2)"));
    allTests.push_back(std::make_pair("lt(8,add(5,y[4]))", "le(4,y[4])"));
    allTests.push_back(std::make_pair("lt(sub(y[4],5),8)", "le(y[4],12)"));
    allTests.push_back(std::make_pair("lt(8,sub(5,y[4]))", "le(y[4],-4)"));
    allTests.push_back(std::make_pair("lt(8,sub(y[4],5))", "le(14,y[4])"));
    allTests.push_back(std::make_pair("lt(mod(4,y[0]),10)", "le(mod(4,y[0]),9)"));
    allTests.push_back(std::make_pair("lt(12,div(y[4],2))", "le(13,div(y[4],2))"));
    allTests.push_back(std::make_pair("lt(10,div(4,y[0]))", "le(11,div(4,y[0]))"));
    allTests.push_back(std::make_pair("lt(dist(5,y[3]),4)", "le(dist(y[3],5),3)"));
    allTests.push_back(std::make_pair("lt(mul(y[0],3),9)", "le(mul(y[0],3),8)"));
    allTests.push_back(std::make_pair("lt(9,mul(3,y[0]))", "le(10,mul(y[0],3))"));
    allTests.push_back(std::make_pair("lt(9,mul(y[0],3))", "le(10,mul(y[0],3))"));
    allTests.push_back(std::make_pair("lt(add(3,4,7),x[0])", "le(15,x[0])"));
    allTests.push_back(std::make_pair("lt(x[0],add(4,3,7))", "le(x[0],13)"));
    allTests.push_back(std::make_pair("lt(mul(3,4,7),x[0])", "le(85,x[0])"));
    allTests.push_back(std::make_pair("le(5,x[0])", "le(5,x[0])"));
    allTests.push_back(std::make_pair("le(x[0],5)", "le(x[0],5)"));
    allTests.push_back(std::make_pair("le(add(y[4],5),8)", "le(y[4],3)"));
    allTests.push_back(std::make_pair("le(8,add(5,y[4]))", "le(3,y[4])"));
    allTests.push_back(std::make_pair("le(sub(y[4],5),8)", "le(y[4],13)"));
    allTests.push_back(std::make_pair("le(8,sub(5,y[4]))", "le(y[4],-3)"));
    allTests.push_back(std::make_pair("le(8,sub(y[4],5))", "le(13,y[4])"));
    allTests.push_back(std::make_pair("le(mod(4,y[0]),10)", "le(mod(4,y[0]),10)"));
    allTests.push_back(std::make_pair("le(12,div(y[4],2))", "le(12,div(y[4],2))"));
    allTests.push_back(std::make_pair("le(10,div(4,y[0]))", "le(10,div(4,y[0]))"));
    allTests.push_back(std::make_pair("le(dist(5,y[3]),4)", "le(dist(y[3],5),4)"));
    allTests.push_back(std::make_pair("le(mul(y[0],3),9)", "le(mul(y[0],3),9)"));
    allTests.push_back(std::make_pair("le(9,mul(3,y[0]))", "le(9,mul(y[0],3))"));
    allTests.push_back(std::make_pair("le(9,mul(y[0],3))", "le(9,mul(y[0],3))"));
    allTests.push_back(std::make_pair("le(add(3,4,7),x[0])", "le(14,x[0])"));
    allTests.push_back(std::make_pair("le(x[0],add(4,3,7))", "le(x[0],14)"));
    allTests.push_back(std::make_pair("le(mul(3,4,7),x[0])", "le(84,x[0])"));
    allTests.push_back(std::make_pair("ge(5,x[0])", "le(x[0],5)"));
    allTests.push_back(std::make_pair("ge(x[0],5)", "le(5,x[0])"));
    allTests.push_back(std::make_pair("ge(add(y[4],5),8)", "le(3,y[4])"));
    allTests.push_back(std::make_pair("ge(8,add(5,y[4]))", "le(y[4],3)"));
    allTests.push_back(std::make_pair("ge(sub(y[4],5),8)", "le(13,y[4])"));
    allTests.push_back(std::make_pair("ge(8,sub(5,y[4]))", "le(-3,y[4])"));
    allTests.push_back(std::make_pair("ge(8,sub(y[4],5))", "le(y[4],13)"));
    allTests.push_back(std::make_pair("ge(mod(4,y[0]),10)", "le(10,mod(4,y[0]))"));
    allTests.push_back(std::make_pair("ge(12,div(y[4],2))", "le(div(y[4],2),12)"));
    allTests.push_back(std::make_pair("ge(10,div(4,y[0]))", "le(div(4,y[0]),10)"));
    allTests.push_back(std::make_pair("ge(dist(5,y[3]),4)", "le(4,dist(y[3],5))"));
    allTests.push_back(std::make_pair("ge(mul(y[0],3),9)", "le(9,mul(y[0],3))"));
    allTests.push_back(std::make_pair("ge(9,mul(3,y[0]))", "le(mul(y[0],3),9)"));
    allTests.push_back(std::make_pair("ge(9,mul(y[0],3))", "le(mul(y[0],3),9)"));
    allTests.push_back(std::make_pair("ge(add(3,4,7),x[0])", "le(x[0],14)"));
    allTests.push_back(std::make_pair("ge(x[0],add(4,3,7))", "le(14,x[0])"));
    allTests.push_back(std::make_pair("ge(mul(3,4,7),x[0])", "le(x[0],84)"));
    allTests.push_back(std::make_pair("gt(5,x[0])", "le(x[0],4)"));
    allTests.push_back(std::make_pair("gt(x[0],5)", "le(6,x[0])"));
    allTests.push_back(std::make_pair("gt(add(y[4],5),8)", "le(4,y[4])"));
    allTests.push_back(std::make_pair("gt(8,add(5,y[4]))", "le(y[4],2)"));
    allTests.push_back(std::make_pair("gt(sub(y[4],5),8)", "le(14,y[4])"));
    allTests.push_back(std::make_pair("gt(8,sub(5,y[4]))", "le(-2,y[4])"));
    allTests.push_back(std::make_pair("gt(8,sub(y[4],5))", "le(y[4],12)"));
    allTests.push_back(std::make_pair("gt(mod(4,y[0]),10)", "le(11,mod(4,y[0]))"));
    allTests.push_back(std::make_pair("gt(12,div(y[4],2))", "le(div(y[4],2),11)"));
    allTests.push_back(std::make_pair("gt(10,div(4,y[0]))", "le(div(4,y[0]),9)"));
    allTests.push_back(std::make_pair("gt(dist(5,y[3]),4)", "le(5,dist(y[3],5))"));
    allTests.push_back(std::make_pair("gt(mul(y[0],3),9)", "le(10,mul(y[0],3))"));
    allTests.push_back(std::make_pair("gt(9,mul(3,y[0]))", "le(mul(y[0],3),8)"));
    allTests.push_back(std::make_pair("gt(9,mul(y[0],3))", "le(mul(y[0],3),8)"));
    allTests.push_back(std::make_pair("gt(add(3,4,7),x[0])", "le(x[0],13)"));
    allTests.push_back(std::make_pair("gt(x[0],add(4,3,7))", "le(15,x[0])"));
    allTests.push_back(std::make_pair("gt(mul(3,4,7),x[0])", "le(x[0],83)"));
    allTests.push_back(std::make_pair("eq(5,x[0])", "eq(x[0],5)"));
    allTests.push_back(std::make_pair("eq(x[0],5)", "eq(x[0],5)"));
    allTests.push_back(std::make_pair("eq(add(y[4],5),8)", "eq(y[4],3)"));
    allTests.push_back(std::make_pair("eq(8,add(5,y[4]))", "eq(y[4],3)"));
    allTests.push_back(std::make_pair("eq(sub(y[4],5),8)", "eq(y[4],13)"));
    allTests.push_back(std::make_pair("eq(8,sub(5,y[4]))", "eq(y[4],-3)"));
    allTests.push_back(std::make_pair("eq(8,sub(y[4],5))", "eq(y[4],13)"));
    allTests.push_back(std::make_pair("eq(mod(4,y[0]),10)", "eq(mod(4,y[0]),10)"));
    allTests.push_back(std::make_pair("eq(12,div(y[4],2))", "eq(div(y[4],2),12)"));
    allTests.push_back(std::make_pair("eq(10,div(4,y[0]))", "eq(div(4,y[0]),10)"));
    allTests.push_back(std::make_pair("eq(dist(5,y[3]),4)", "eq(dist(y[3],5),4)"));
    allTests.push_back(std::make_pair("eq(mul(y[0],3),9)", "eq(y[0],3)"));
    allTests.push_back(std::make_pair("eq(9,mul(3,y[0]))", "eq(y[0],3)"));
    allTests.push_back(std::make_pair("eq(9,mul(y[0],3))", "eq(y[0],3)"));
    allTests.push_back(std::make_pair("eq(add(3,4,7),x[0])", "eq(x[0],14)"));
    allTests.push_back(std::make_pair("eq(x[0],add(4,3,7))", "eq(x[0],14)"));
    allTests.push_back(std::make_pair("eq(mul(3,4,7),x[0])", "eq(x[0],84)"));
    allTests.push_back(std::make_pair("ne(5,x[0])", "ne(x[0],5)"));
    allTests.push_back(std::make_pair("ne(x[0],5)", "ne(x[0],5)"));
    allTests.push_back(std::make_pair("ne(add(y[4],5),8)", "ne(y[4],3)"));
    allTests.push_back(std::make_pair("ne(8,add(5,y[4]))", "ne(y[4],3)"));
    allTests.push_back(std::make_pair("ne(sub(y[4],5),8)", "ne(y[4],13)"));
    allTests.push_back(std::make_pair("ne(8,sub(5,y[4]))", "ne(y[4],-3)"));
    allTests.push_back(std::make_pair("ne(8,sub(y[4],5))", "ne(y[4],13)"));
    allTests.push_back(std::make_pair("ne(mod(4,y[0]),10)", "ne(mod(4,y[0]),10)"));
    allTests.push_back(std::make_pair("ne(12,div(y[4],2))", "ne(div(y[4],2),12)"));
    allTests.push_back(std::make_pair("ne(10,div(4,y[0]))", "ne(div(4,y[0]),10)"));
    allTests.push_back(std::make_pair("ne(dist(5,y[3]),4)", "ne(dist(y[3],5),4)"));
    allTests.push_back(std::make_pair("ne(mul(y[0],3),9)", "ne(mul(y[0],3),9)"));
    allTests.push_back(std::make_pair("ne(9,mul(3,y[0]))", "ne(mul(y[0],3),9)"));
    allTests.push_back(std::make_pair("ne(9,mul(y[0],3))", "ne(mul(y[0],3),9)"));
    allTests.push_back(std::make_pair("ne(add(3,4,7),x[0])", "ne(x[0],14)"));
    allTests.push_back(std::make_pair("ne(x[0],add(4,3,7))", "ne(x[0],14)"));
    allTests.push_back(std::make_pair("ne(mul(3,4,7),x[0])", "ne(x[0],84)"));
    allTests.push_back(std::make_pair("in(y[3],set(2,3,4))", "in(y[3],set(2,3,4))"));
    allTests.push_back(std::make_pair("in(add(y[3],2),set(2,3,4))", "in(add(y[3],2),set(2,3,4))"));
    allTests.push_back(std::make_pair("in(add(2,y[3]),set(4,3,2))", "in(add(y[3],2),set(2,3,4))"));
    allTests.push_back(std::make_pair("in(mod(5,y[3]),set(3,2,4))", "in(mod(5,y[3]),set(2,3,4))"));
    allTests.push_back(std::make_pair("lt(y[3],y[4])", "lt(y[3],y[4])"));
    allTests.push_back(std::make_pair("lt(y[3],y[4])", "lt(y[3],y[4])"));
    allTests.push_back(std::make_pair("lt(y[3],y[2])", "lt(y[3],y[2])"));
    allTests.push_back(std::make_pair("lt(x[0],abs(x[1]))", "lt(x[0],abs(x[1]))"));
    allTests.push_back(std::make_pair("lt(abs(x[1]),x[0])", "lt(abs(x[1]),x[0])"));
    allTests.push_back(std::make_pair("lt(x[0],sub(x[1],4))", "lt(add(x[0],4),x[1])"));
    allTests.push_back(std::make_pair("lt(x[0],sub(4,x[1]))", "le(add(x[0],x[1]),3)"));
    allTests.push_back(std::make_pair("lt(4,sub(x[0],x[1]))", "le(add(x[1],5),x[0])"));
    allTests.push_back(std::make_pair("lt(sub(x[1],4),x[0])", "lt(x[1],add(x[0],4))"));
    allTests.push_back(std::make_pair("lt(sub(4,x[1]),x[0])", "le(5,add(x[0],x[1]))"));
    allTests.push_back(std::make_pair("lt(sub(4,y[2]),sub(y[1],3))", "le(8,add(y[1],y[2]))"));
    allTests.push_back(std::make_pair("lt(sub(y[1],3),sub(4,y[2]))", "le(add(y[1],y[2]),6)"));
    allTests.push_back(std::make_pair("lt(add(y[2],2),add(y[4],5))", "lt(add(y[2],-3),y[4])"));
    allTests.push_back(std::make_pair("lt(add(2,y[2]),add(5,y[4]))", "lt(add(y[2],-3),y[4])"));
    allTests.push_back(std::make_pair("lt(dist(y[2],2),y[4])", "lt(dist(y[2],2),y[4])"));
    allTests.push_back(std::make_pair("lt(y[4],dist(y[2],2))", "lt(y[4],dist(y[2],2))"));
    allTests.push_back(std::make_pair("lt(pow(y[3],y[4]),10)", "le(pow(y[3],y[4]),9)"));
    allTests.push_back(std::make_pair("lt(10,pow(y[3],y[4]))", "le(11,pow(y[3],y[4]))"));
    allTests.push_back(std::make_pair("lt(neg(y[2]),y[1])", "lt(neg(y[2]),y[1])"));
    allTests.push_back(std::make_pair("lt(sqr(y[2]),y[1])", "lt(sqr(y[2]),y[1])"));
    allTests.push_back(std::make_pair("lt(not(y[2]),y[1])", "lt(not(y[2]),y[1])"));
    allTests.push_back(std::make_pair("le(y[3],y[4])", "le(y[3],y[4])"));
    allTests.push_back(std::make_pair("le(y[3],y[4])", "le(y[3],y[4])"));
    allTests.push_back(std::make_pair("le(y[3],y[2])", "le(y[3],y[2])"));
    allTests.push_back(std::make_pair("le(x[0],abs(x[1]))", "le(x[0],abs(x[1]))"));
    allTests.push_back(std::make_pair("le(abs(x[1]),x[0])", "le(abs(x[1]),x[0])"));
    allTests.push_back(std::make_pair("le(x[0],sub(x[1],4))", "le(add(x[0],4),x[1])"));
    allTests.push_back(std::make_pair("le(x[0],sub(4,x[1]))", "le(add(x[0],x[1]),4)"));
    allTests.push_back(std::make_pair("le(4,sub(x[0],x[1]))", "le(add(x[1],4),x[0])"));
    allTests.push_back(std::make_pair("le(sub(x[1],4),x[0])", "le(x[1],add(x[0],4))"));
    allTests.push_back(std::make_pair("le(sub(4,x[1]),x[0])", "le(4,add(x[0],x[1]))"));
    allTests.push_back(std::make_pair("le(sub(4,y[2]),sub(y[1],3))", "le(7,add(y[1],y[2]))"));
    allTests.push_back(std::make_pair("le(sub(y[1],3),sub(4,y[2]))", "le(add(y[1],y[2]),7)"));
    allTests.push_back(std::make_pair("le(add(y[2],2),add(y[4],5))", "le(add(y[2],-3),y[4])"));
    allTests.push_back(std::make_pair("le(add(2,y[2]),add(5,y[4]))", "le(add(y[2],-3),y[4])"));
    allTests.push_back(std::make_pair("le(dist(y[2],2),y[4])", "le(dist(y[2],2),y[4])"));
    allTests.push_back(std::make_pair("le(y[4],dist(y[2],2))", "le(y[4],dist(y[2],2))"));
    allTests.push_back(std::make_pair("le(pow(y[3],y[4]),10)", "le(pow(y[3],y[4]),10)"));
    allTests.push_back(std::make_pair("le(10,pow(y[3],y[4]))", "le(10,pow(y[3],y[4]))"));
    allTests.push_back(std::make_pair("le(neg(y[2]),y[1])", "le(neg(y[2]),y[1])"));
    allTests.push_back(std::make_pair("le(sqr(y[2]),y[1])", "le(sqr(y[2]),y[1])"));
    allTests.push_back(std::make_pair("le(not(y[2]),y[1])", "le(not(y[2]),y[1])"));
    allTests.push_back(std::make_pair("ge(y[3],y[4])", "le(y[4],y[3])"));
    allTests.push_back(std::make_pair("ge(y[3],y[4])", "le(y[4],y[3])"));
    allTests.push_back(std::make_pair("ge(y[3],y[2])", "le(y[2],y[3])"));
    allTests.push_back(std::make_pair("ge(x[0],abs(x[1]))", "le(abs(x[1]),x[0])"));
    allTests.push_back(std::make_pair("ge(abs(x[1]),x[0])", "le(x[0],abs(x[1]))"));
    allTests.push_back(std::make_pair("ge(x[0],sub(x[1],4))", "le(x[1],add(x[0],4))"));
    allTests.push_back(std::make_pair("ge(x[0],sub(4,x[1]))", "le(4,add(x[0],x[1]))"));
    allTests.push_back(std::make_pair("ge(4,sub(x[0],x[1]))", "le(x[0],add(x[1],4))"));
    allTests.push_back(std::make_pair("ge(sub(x[1],4),x[0])", "le(add(x[0],4),x[1])"));
    allTests.push_back(std::make_pair("ge(sub(4,x[1]),x[0])", "le(add(x[0],x[1]),4)"));
    allTests.push_back(std::make_pair("ge(sub(4,y[2]),sub(y[1],3))", "le(add(y[1],y[2]),7)"));
    allTests.push_back(std::make_pair("ge(sub(y[1],3),sub(4,y[2]))", "le(7,add(y[1],y[2]))"));
    allTests.push_back(std::make_pair("ge(add(y[2],2),add(y[4],5))", "le(add(y[4],3),y[2])"));
    allTests.push_back(std::make_pair("ge(add(2,y[2]),add(5,y[4]))", "le(add(y[4],3),y[2])"));
    allTests.push_back(std::make_pair("ge(dist(y[2],2),y[4])", "le(y[4],dist(y[2],2))"));
    allTests.push_back(std::make_pair("ge(y[4],dist(y[2],2))", "le(dist(y[2],2),y[4])"));
    allTests.push_back(std::make_pair("ge(pow(y[3],y[4]),10)", "le(10,pow(y[3],y[4]))"));
    allTests.push_back(std::make_pair("ge(10,pow(y[3],y[4]))", "le(pow(y[3],y[4]),10)"));
    allTests.push_back(std::make_pair("ge(neg(y[2]),y[1])", "le(y[1],neg(y[2]))"));
    allTests.push_back(std::make_pair("ge(sqr(y[2]),y[1])", "le(y[1],sqr(y[2]))"));
    allTests.push_back(std::make_pair("ge(not(y[2]),y[1])", "le(y[1],not(y[2]))"));
    allTests.push_back(std::make_pair("gt(y[3],y[4])", "lt(y[4],y[3])"));
    allTests.push_back(std::make_pair("gt(y[3],y[4])", "lt(y[4],y[3])"));
    allTests.push_back(std::make_pair("gt(y[3],y[2])", "lt(y[2],y[3])"));
    allTests.push_back(std::make_pair("gt(x[0],abs(x[1]))", "lt(abs(x[1]),x[0])"));
    allTests.push_back(std::make_pair("gt(abs(x[1]),x[0])", "lt(x[0],abs(x[1]))"));
    allTests.push_back(std::make_pair("gt(x[0],sub(x[1],4))", "lt(x[1],add(x[0],4))"));
    allTests.push_back(std::make_pair("gt(x[0],sub(4,x[1]))", "le(5,add(x[0],x[1]))"));
    allTests.push_back(std::make_pair("gt(4,sub(x[0],x[1]))", "le(x[0],add(x[1],3))"));
    allTests.push_back(std::make_pair("gt(sub(x[1],4),x[0])", "lt(add(x[0],4),x[1])"));
    allTests.push_back(std::make_pair("gt(sub(4,x[1]),x[0])", "le(add(x[0],x[1]),3)"));
    allTests.push_back(std::make_pair("gt(sub(4,y[2]),sub(y[1],3))", "le(add(y[1],y[2]),6)"));
    allTests.push_back(std::make_pair("gt(sub(y[1],3),sub(4,y[2]))", "le(8,add(y[1],y[2]))"));
    allTests.push_back(std::make_pair("gt(add(y[2],2),add(y[4],5))", "lt(add(y[4],3),y[2])"));
    allTests.push_back(std::make_pair("gt(add(2,y[2]),add(5,y[4]))", "lt(add(y[4],3),y[2])"));
    allTests.push_back(std::make_pair("gt(dist(y[2],2),y[4])", "lt(y[4],dist(y[2],2))"));
    allTests.push_back(std::make_pair("gt(y[4],dist(y[2],2))", "lt(dist(y[2],2),y[4])"));
    allTests.push_back(std::make_pair("gt(pow(y[3],y[4]),10)", "le(11,pow(y[3],y[4]))"));
    allTests.push_back(std::make_pair("gt(10,pow(y[3],y[4]))", "le(pow(y[3],y[4]),9)"));
    allTests.push_back(std::make_pair("gt(neg(y[2]),y[1])", "lt(y[1],neg(y[2]))"));
    allTests.push_back(std::make_pair("gt(sqr(y[2]),y[1])", "lt(y[1],sqr(y[2]))"));
    allTests.push_back(std::make_pair("gt(not(y[2]),y[1])", "lt(y[1],not(y[2]))"));
    allTests.push_back(std::make_pair("eq(y[3],y[4])", "eq(y[3],y[4])"));
    allTests.push_back(std::make_pair("eq(y[3],y[4])", "eq(y[3],y[4])"));
    allTests.push_back(std::make_pair("eq(y[3],y[2])", "eq(y[2],y[3])"));
    allTests.push_back(std::make_pair("eq(x[0],abs(x[1]))", "eq(abs(x[1]),x[0])"));
    allTests.push_back(std::make_pair("eq(abs(x[1]),x[0])", "eq(abs(x[1]),x[0])"));
    allTests.push_back(std::make_pair("eq(x[0],sub(x[1],4))", "eq(add(x[0],4),x[1])"));
    allTests.push_back(std::make_pair("eq(x[0],sub(4,x[1]))", "eq(add(x[0],x[1]),4)"));
    allTests.push_back(std::make_pair("eq(4,sub(x[0],x[1]))", "eq(add(x[1],4),x[0])"));
    allTests.push_back(std::make_pair("eq(sub(x[1],4),x[0])", "eq(add(x[0],4),x[1])"));
    allTests.push_back(std::make_pair("eq(sub(4,x[1]),x[0])", "eq(add(x[0],x[1]),4)"));
    allTests.push_back(std::make_pair("eq(sub(4,y[2]),sub(y[1],3))", "eq(add(y[1],y[2]),7)"));
    allTests.push_back(std::make_pair("eq(sub(y[1],3),sub(4,y[2]))", "eq(add(y[1],y[2]),7)"));
    allTests.push_back(std::make_pair("eq(add(y[2],2),add(y[4],5))", "eq(add(y[2],-3),y[4])"));
    allTests.push_back(std::make_pair("eq(add(2,y[2]),add(5,y[4]))", "eq(add(y[2],-3),y[4])"));
    allTests.push_back(std::make_pair("eq(dist(y[2],2),y[4])", "eq(dist(y[2],2),y[4])"));
    allTests.push_back(std::make_pair("eq(y[4],dist(y[2],2))", "eq(dist(y[2],2),y[4])"));
    allTests.push_back(std::make_pair("eq(pow(y[3],y[4]),10)", "eq(pow(y[3],y[4]),10)"));
    allTests.push_back(std::make_pair("eq(10,pow(y[3],y[4]))", "eq(pow(y[3],y[4]),10)"));
    allTests.push_back(std::make_pair("eq(neg(y[2]),y[1])", "eq(neg(y[2]),y[1])"));
    allTests.push_back(std::make_pair("eq(sqr(y[2]),y[1])", "eq(sqr(y[2]),y[1])"));
    allTests.push_back(std::make_pair("eq(not(y[2]),y[1])", "ne(y[1],y[2])"));
    allTests.push_back(std::make_pair("ne(y[3],y[4])", "ne(y[3],y[4])"));
    allTests.push_back(std::make_pair("ne(y[3],y[4])", "ne(y[3],y[4])"));
    allTests.push_back(std::make_pair("ne(y[3],y[2])", "ne(y[2],y[3])"));
    allTests.push_back(std::make_pair("ne(x[0],abs(x[1]))", "ne(abs(x[1]),x[0])"));
    allTests.push_back(std::make_pair("ne(abs(x[1]),x[0])", "ne(abs(x[1]),x[0])"));
    allTests.push_back(std::make_pair("ne(x[0],sub(x[1],4))", "ne(add(x[0],4),x[1])"));
    allTests.push_back(std::make_pair("ne(x[0],sub(4,x[1]))", "ne(add(x[0],x[1]),4)"));
    allTests.push_back(std::make_pair("ne(4,sub(x[0],x[1]))", "ne(add(x[1],4),x[0])"));
    allTests.push_back(std::make_pair("ne(sub(x[1],4),x[0])", "ne(add(x[0],4),x[1])"));
    allTests.push_back(std::make_pair("ne(sub(4,x[1]),x[0])", "ne(add(x[0],x[1]),4)"));
    allTests.push_back(std::make_pair("ne(sub(4,y[2]),sub(y[1],3))", "ne(add(y[1],y[2]),7)"));
    allTests.push_back(std::make_pair("ne(sub(y[1],3),sub(4,y[2]))", "ne(add(y[1],y[2]),7)"));
    allTests.push_back(std::make_pair("ne(add(y[2],2),add(y[4],5))", "ne(add(y[2],-3),y[4])"));
    allTests.push_back(std::make_pair("ne(add(2,y[2]),add(5,y[4]))", "ne(add(y[2],-3),y[4])"));
    allTests.push_back(std::make_pair("ne(dist(y[2],2),y[4])", "ne(dist(y[2],2),y[4])"));
    allTests.push_back(std::make_pair("ne(y[4],dist(y[2],2))", "ne(dist(y[2],2),y[4])"));
    allTests.push_back(std::make_pair("ne(pow(y[3],y[4]),10)", "ne(pow(y[3],y[4]),10)"));
    allTests.push_back(std::make_pair("ne(10,pow(y[3],y[4]))", "ne(pow(y[3],y[4]),10)"));
    allTests.push_back(std::make_pair("ne(neg(y[2]),y[1])", "ne(neg(y[2]),y[1])"));
    allTests.push_back(std::make_pair("ne(sqr(y[2]),y[1])", "ne(sqr(y[2]),y[1])"));
    allTests.push_back(std::make_pair("ne(not(y[2]),y[1])", "eq(y[1],y[2])"));
    allTests.push_back(std::make_pair("lt(y[8],abs(sub(y[7],y[6])))", "lt(y[8],dist(y[6],y[7]))"));
    allTests.push_back(std::make_pair("lt(sub(x[1],4),sub(x[2],x[3]))", "lt(add(x[1],x[3]),add(x[2],4))"));
    allTests.push_back(std::make_pair("lt(add(y[0],y[1]),y[2])", "lt(add(y[0],y[1]),y[2])"));
    allTests.push_back(std::make_pair("lt(y[3],sub(y[4],y[5]))", "lt(add(y[3],y[5]),y[4])"));
    allTests.push_back(std::make_pair("lt(y[3],add(y[4],y[5]))", "lt(y[3],add(y[4],y[5]))"));
    allTests.push_back(std::make_pair("lt(add(y[3],10),add(10,y[4],y[5],6))", "lt(add(y[3],10),add(y[4],y[5],16))"));
    allTests.push_back(std::make_pair("lt(y[3],mul(y[4],y[5],3,5))", "lt(y[3],mul(y[4],y[5],15))"));
    allTests.push_back(std::make_pair("lt(y[1],pow(y[3],y[2]))", "lt(y[1],pow(y[3],y[2]))"));
    allTests.push_back(std::make_pair("le(y[8],abs(sub(y[7],y[6])))", "le(y[8],dist(y[6],y[7]))"));
    allTests.push_back(std::make_pair("le(sub(x[1],4),sub(x[2],x[3]))", "le(add(x[1],x[3]),add(x[2],4))"));
    allTests.push_back(std::make_pair("le(add(y[0],y[1]),y[2])", "le(add(y[0],y[1]),y[2])"));
    allTests.push_back(std::make_pair("le(y[3],sub(y[4],y[5]))", "le(add(y[3],y[5]),y[4])"));
    allTests.push_back(std::make_pair("le(y[3],add(y[4],y[5]))", "le(y[3],add(y[4],y[5]))"));
    allTests.push_back(std::make_pair("le(add(y[3],10),add(10,y[4],y[5],6))", "le(add(y[3],10),add(y[4],y[5],16))"));
    allTests.push_back(std::make_pair("le(y[3],mul(y[4],y[5],3,5))", "le(y[3],mul(y[4],y[5],15))"));
    allTests.push_back(std::make_pair("le(y[1],pow(y[3],y[2]))", "le(y[1],pow(y[3],y[2]))"));
    allTests.push_back(std::make_pair("ge(y[8],abs(sub(y[7],y[6])))", "le(dist(y[6],y[7]),y[8])"));
    allTests.push_back(std::make_pair("ge(sub(x[1],4),sub(x[2],x[3]))", "le(add(x[2],4),add(x[1],x[3]))"));
    allTests.push_back(std::make_pair("ge(add(y[0],y[1]),y[2])", "le(y[2],add(y[0],y[1]))"));
    allTests.push_back(std::make_pair("ge(y[3],sub(y[4],y[5]))", "le(y[4],add(y[3],y[5]))"));
    allTests.push_back(std::make_pair("ge(y[3],add(y[4],y[5]))", "le(add(y[4],y[5]),y[3])"));
    allTests.push_back(std::make_pair("ge(add(y[3],10),add(10,y[4],y[5],6))", "le(add(y[4],y[5],16),add(y[3],10))"));
    allTests.push_back(std::make_pair("ge(y[3],mul(y[4],y[5],3,5))", "le(mul(y[4],y[5],15),y[3])"));
    allTests.push_back(std::make_pair("ge(y[1],pow(y[3],y[2]))", "le(pow(y[3],y[2]),y[1])"));
    allTests.push_back(std::make_pair("gt(y[8],abs(sub(y[7],y[6])))", "lt(dist(y[6],y[7]),y[8])"));
    allTests.push_back(std::make_pair("gt(sub(x[1],4),sub(x[2],x[3]))", "lt(add(x[2],4),add(x[1],x[3]))"));
    allTests.push_back(std::make_pair("gt(add(y[0],y[1]),y[2])", "lt(y[2],add(y[0],y[1]))"));
    allTests.push_back(std::make_pair("gt(y[3],sub(y[4],y[5]))", "lt(y[4],add(y[3],y[5]))"));
    allTests.push_back(std::make_pair("gt(y[3],add(y[4],y[5]))", "lt(add(y[4],y[5]),y[3])"));
    allTests.push_back(std::make_pair("gt(add(y[3],10),add(10,y[4],y[5],6))", "lt(add(y[4],y[5],16),add(y[3],10))"));
    allTests.push_back(std::make_pair("gt(y[3],mul(y[4],y[5],3,5))", "lt(mul(y[4],y[5],15),y[3])"));
    allTests.push_back(std::make_pair("gt(y[1],pow(y[3],y[2]))", "lt(pow(y[3],y[2]),y[1])"));
    allTests.push_back(std::make_pair("eq(y[8],abs(sub(y[7],y[6])))", "eq(dist(y[6],y[7]),y[8])"));
    allTests.push_back(std::make_pair("eq(sub(x[1],4),sub(x[2],x[3]))", "eq(add(x[1],x[3]),add(x[2],4))"));
    allTests.push_back(std::make_pair("eq(add(y[0],y[1]),y[2])", "eq(add(y[0],y[1]),y[2])"));
    allTests.push_back(std::make_pair("eq(y[3],sub(y[4],y[5]))", "eq(add(y[3],y[5]),y[4])"));
    allTests.push_back(std::make_pair("eq(y[3],add(y[4],y[5]))", "eq(add(y[4],y[5]),y[3])"));
    allTests.push_back(std::make_pair("eq(add(y[3],10),add(10,y[4],y[5],6))", "eq(add(y[3],10),add(y[4],y[5],16))"));
    allTests.push_back(std::make_pair("eq(y[3],mul(y[4],y[5],3,5))", "eq(mul(y[4],y[5],15),y[3])"));
    allTests.push_back(std::make_pair("eq(y[1],pow(y[3],y[2]))", "eq(pow(y[3],y[2]),y[1])"));
    allTests.push_back(std::make_pair("ne(y[8],abs(sub(y[7],y[6])))", "ne(dist(y[6],y[7]),y[8])"));
    allTests.push_back(std::make_pair("ne(sub(x[1],4),sub(x[2],x[3]))", "ne(add(x[1],x[3]),add(x[2],4))"));
    allTests.push_back(std::make_pair("ne(add(y[0],y[1]),y[2])", "ne(add(y[0],y[1]),y[2])"));
    allTests.push_back(std::make_pair("ne(y[3],sub(y[4],y[5]))", "ne(add(y[3],y[5]),y[4])"));
    allTests.push_back(std::make_pair("ne(y[3],add(y[4],y[5]))", "ne(add(y[4],y[5]),y[3])"));
    allTests.push_back(std::make_pair("ne(add(y[3],10),add(10,y[4],y[5],6))", "ne(add(y[3],10),add(y[4],y[5],16))"));
    allTests.push_back(std::make_pair("ne(y[3],mul(y[4],y[5],3,5))", "ne(mul(y[4],y[5],15),y[3])"));
    allTests.push_back(std::make_pair("ne(y[1],pow(y[3],y[2]))", "ne(pow(y[3],y[2]),y[1])"));
    allTests.push_back(std::make_pair("and(x[0],x[1])", "and(x[0],x[1])"));
    allTests.push_back(std::make_pair("and(x[0],not(eq(x[1],x[2])))", "and(ne(x[1],x[2]),x[0])"));
    allTests.push_back(std::make_pair("and(gt(y[2],10),le(y[2],20))", "and(le(y[2],20),le(11,y[2]))"));
    allTests.push_back(std::make_pair("and(lt(y[2],100),ge(y[2],1))", "and(le(y[2],99),le(1,y[2]))"));
    allTests.push_back(std::make_pair("and(gt(y[2],10),lt(y[2],20))", "and(le(y[2],19),le(11,y[2]))"));
    allTests.push_back(std::make_pair("and(le(y[2],100),ge(y[2],1))", "and(le(y[2],100),le(1,y[2]))"));
    allTests.push_back(std::make_pair("and(x[0],not(in(x[1],set(4,3,8))))", "and(notin(x[1],set(3,4,8)),x[0])"));
    allTests.push_back(std::make_pair("or(x[3],x[4],x[1])", "or(x[1],x[3],x[4])"));
    allTests.push_back(std::make_pair("iff(x[2],x[1])", "iff(x[1],x[2])"));
    allTests.push_back(std::make_pair("not(not(eq(x[3],x[4])))", "eq(x[3],x[4])"));
    allTests.push_back(std::make_pair("eq(x[0],iff(x[1],x[2],x[3]))", "eq(iff(x[1],x[2],x[3]),x[0])"));
    allTests.push_back(std::make_pair("ne(x[3],imp(x[1],x[2]))", "ne(imp(x[1],x[2]),x[3])"));
    allTests.push_back(std::make_pair("eq(5,add(y[0],y[1],y[9]),sub(y[2],y[6]),y[8],mul(y[5],y[6]))", "eq(add(y[0],y[1],y[9]),sub(y[2],y[6]),mul(y[5],y[6]),y[8],5)"));
    allTests.push_back(std::make_pair("eq(x[0],min(x[1],min(x[2],x[3])))", "eq(min(x[1],x[2],x[3]),x[0])"));
    allTests.push_back(std::make_pair("eq(add(add(x[1],x[2],min(x[2],x[3]),add(x[3],x[4])),add(add(y[1],y[2]),y[3])),y[2])", "eq(add(min(x[2],x[3]),x[1],x[2],x[3],x[4],y[1],y[2],y[3]),y[2])"));


    int nb = 0;
    for (auto& p : allTests) {
        Tree t1(p.first);
        t1.canonize();
        Tree t2(p.second);
        nb++;

        if (equalNodes(t1.root, t2.root) != 0) {
            nbFailed++;
            std::cout << "Problem: number " << nb << std::endl;
            std::cout << "  Initial expression: " << p.first << std::endl;
            std::cout << "  Expected expression: " << p.second << std::endl;
            std::cout << "  Canonized expression: ";

            t1.prefixe();
            std::cout << std::endl;
            std::cout << "--" << std::endl;
        } else 
            nbSuccess++;
    }

    std::cout << allTests.size() << " tests: " << nbFailed << " failed " << nbSuccess << " success\n";
}