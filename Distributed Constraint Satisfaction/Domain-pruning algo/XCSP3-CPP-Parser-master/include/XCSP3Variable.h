
#ifndef XVARIABLE_H_
#define XVARIABLE_H_


#include "XCSP3Domain.h"
#include <vector>
#include <sstream>


namespace XCSP3Core {

    class XInterval {
    public:
        int min, max;

        XInterval(int mn, int mx) : min(mn), max(mx) { }
        friend ostream &operator<<(ostream &f, const XInterval& ie);
    };

    class XEntity {
    public:
        std::string id;

        XEntity();
        XEntity(std::string lid);
        virtual ~XEntity();

        virtual void fake() { }  // Fake function to allow use of dynamic_cast!!
    };

    class XVariable : public XEntity {
    public:
        string classes;
        XDomainInteger* domain;

        XVariable(std::string idd, XDomainInteger* dom);
        XVariable(std::string idd, XDomainInteger* dom, std::vector<int> indexes);

        virtual ~XVariable();
        friend ostream& operator<<(ostream& f, const XVariable& ie);
    };


    /**
     * This is a fake variable used for representing an integer
     */

    class XInteger : public XVariable {
    public:
        int value;

        XInteger(std::string lid, int n) : XVariable(lid, NULL), value(n) { }
    };


    /**
     * This is a fake variable used for representing a Tree
     */
    class XTree : public XVariable {
    public:
        XTree(std::string lid) : XVariable(lid, NULL) { }
    };

    class XEInterval : public XVariable {
    public:
        int min, max;

        XEInterval(std::string lid, int mn, int mx) : XVariable(lid, NULL), min(mn), max(mx) { }
    };


    /**
     * Check if a XEntity is an integer
     * If true, the value is set to its integer
     */
    bool isInteger(XEntity* xe, int* value);


    /**
     * Check if a XEntity is an integer
     * If true, the value is set to its integer
     */
    bool isInterval(XEntity* xe, int& min, int& max);

    bool isVariable(XEntity* xe, XVariable* &v);


    /**
     * This is a fake variable used as parameter for group constraint
     */
    class XParameterVariable : public XVariable {
    public:
        int number; // -1 if %...
        XParameterVariable(std::string lid);
    };

    class XVariableArray : public XEntity {
    public:
        string classes;
        std::vector<XVariable*> variables;  // The flat (one-dimensioanl) array
        std::vector<int> sizes; // The size of the array, as defined in XCSP3.

        /**
         * Builds an array of variables with the specified id, type and size. 
         * All variables are directly defined with the specified domain. 
         */
        XVariableArray(std::string id, std::vector<int> szs);


        /**
         * Copy constructor
         * @param domain
         */
        XVariableArray(std::string idd, XVariableArray* as);

        virtual ~XVariableArray();


        /** 
         * Transforms a flat index
         * into a multi-dimensional index. 
         */
        void indexesFor(int flatIndex, std::Vector<int>& indexes);

        int flatIndexFor(vector<int> indexes);

        bool incrementIndexes(vector<int>& indexes, vector<XIntegerEntity*>& ranges);


        /**
         * Returns the list of variables that match the specified compact form. 
         * For example: 
         *      for x[1..3], the list will contain x[1] x[2] and x[3]. 
         */
        void getVarFor(vector<XVariable*>& list, string compactForm, vector<int>* flatIndexes = NULL, bool storeIndexes = false);


        /**
         * Builds a variable with the specified domain
         * for each unoccupied cell of the flat array. 
         */
        void buildVarsWith(XDomainInteger* domain);

    };
}



#endif  // XVARIABLE_H_