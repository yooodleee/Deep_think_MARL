
#include <iostream>
#include "CspProblem.h"
#include "../utils/utils.h"
#include <list>
#include <sstream>

using namespace std;


CspProblem::CspProblem(const CspProblemDefinition& problemDefinition, const ExpressionFactory & operandFactory) {
    for (int i = 0;i < problemDefinition.variables.size(); i++) {
        string name = problemDefinition.variables[i].token;

        Variable variable(
            name,
            list<domain_element>(
                problemDefinition.domains[i].begin(),
                problemDefinition.domains[i].end()
            )
        );
        this->environment.emplace(name, move(variable));
    }

    for (auto& constraints : problemDefinition.constraints) {
        vector<OperandPtr> operands;
        operands.resize(constraints.size());

        std::transform(constraints.begin(), constraints.end(), operands.begin(), [&](constraint c) {
            return boost::apply_visitor(operandFactory, c);
        });

        this->constraintMachines.puch_back(stackMachine (move(operands)));
    }
}


string CspProblem::describe() {
    stringstream description;
    description << "Variables : " << environment.size() << "\n";
    description << "Constraints : " << environment.size() << "\n";

    for (auto& constraintMachine : this->constraintMachines) {
        description << constraintMachine.description();

    }
    return description.str();
}