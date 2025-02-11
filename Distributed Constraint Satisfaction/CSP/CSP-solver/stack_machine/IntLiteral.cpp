

#include "IntLiteral.h"


boost::optional<domain_element> IntLiteral::eval(StackIt& it, Environment &environment) {
    return boost::optional<domain_element>(value);
}

IntLiteral::IntLiteral(int val) {
    this->value = val;
}