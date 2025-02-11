
#include "AbsValueOperator.h"
#include "../IntLiteral.h"
#include "../../utils/utils.h"


boost::optional<domain_element> AbsValueOperator::eval(StackIt stack, Environment &environment) {
    auto a = topAndProp(stack)->eval(stack, environment);
    if (!a) {
        return boost::none;
    }
    return boost::optional<domain_elements>(abs(boost::get<int>(a.get())));
}