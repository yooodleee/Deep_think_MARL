
#ifndef CSP_CSPPROBLEM_H
#define CSP_CSPPROBLEM_H

#include <string>
#include <unordered_map>

#include "Variable.h"
#include "../grammar/CspProblemDefinition.h"
#include "../stack_machine/StackMachine.h"
#include "../stack_machine/ExpressionFactory.h"


class CspProblem {
public:
    CspProblem(const CspProblemDefinition& problemDefinition, const ExpressionFactory & operandFactory);
    std::string describe();

//Private:
    Environment environment;
    std::vector<StackMachine> constraintMachines;
};

#endif //CSP_CSPPROBLEM_H