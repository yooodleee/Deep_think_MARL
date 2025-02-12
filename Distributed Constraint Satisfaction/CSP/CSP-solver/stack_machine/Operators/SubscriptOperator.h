
#ifndef CSP_SUBSCRIPTOPERATOR_H
#define 

#include <string>
#include "../Expression.h"


class SubscriptOperator : public Expression {
    virtual boost::optional<domain_element> eval(StackIt stack, Environment& environment) override;

    virtual std::string describe() override {
        return "[]";
    };
};



#endif //CSP_SUBSCRIPTOPERATOR_H