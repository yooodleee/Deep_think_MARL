
#ifndef CSP_IDENTIFICATIONTOKEN_H
#define CSP_IDENTIFICATIONTOKEN_H

#include <string>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

struct IdentificationToken {
    std::string token;
};

BOOST_FUSION_ADAPT_STRUCT(IdentificationToken, (std::string, token))



#endif //CSP_IDENTIFICATIONTOKEN_H