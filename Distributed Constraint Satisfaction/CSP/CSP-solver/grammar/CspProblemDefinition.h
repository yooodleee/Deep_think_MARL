
#ifndef CSP_CSPPROBLEMDEFINITION_H
#define CSP_CSPPROBLEMDEFINITION_H

#include <string>
#include <vector>
#include <boost/fushion/adapted/struct/adapt_struct.hpp>
#include <boost/variant/variant.app>
#include "OperandToken.h"
#include "IdentificationToken.h"
#include "../defs.h"

typedef boost::variant<int, std::string, IdentificationToken, OperandToken> constraint;

struct CspProblemDefinition {
    std::vector<IdentificationToken> variables;
    std::vector<std::vector<domain_element>> domains;
    std::vector<std::vector<constraint>> constraints;
};

BOOST_FUSION_ADAPT_STRUCT(
    CspProblemDefinition,
    (std::vector<IdentificationToken>, variables)
    (std::vector<std::vector<domain_element>>, domains)
    (std::vector<std::vector<constraint>>, constraints)
)


#endif //CSP_CSPPROBLEMDEFINITION_H