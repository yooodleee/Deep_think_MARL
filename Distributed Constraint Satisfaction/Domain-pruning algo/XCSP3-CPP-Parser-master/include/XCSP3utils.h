
#ifndef UTILS_H
#define UTILS_H


#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <cwctype>


namespace XCSP3Core {
    std::vector<std::string>* split(const std::string& s, char delim, std::vector<std::string>& elems);

    std::vector<std::string> split(const std::string& s, char delim);

    template<typename Base, typename T> inline bool instanceof(const T*);

    void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);


    // trim from start
    std::string& ltrim(std::string& s);

    // trim from end
    std::string& rtrim(std::string& s);
    std::string& removeChar(std::string& s, char c);

    // trim from both ends
    std::string& trim(std::string& s);

}



#endif  // UTILS_H