
#ifndef CSP_OPERATORTOKEN_H
#define CSP_OPERATORTOKEN_H

#include <string>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

struct OperatorToken {
    std::string token;
};

BOOST_FUSION_ADAPT_STRUCT(OperatorToken, (std::string, token))



#endif //CSP_OPERATORTOKEN_H