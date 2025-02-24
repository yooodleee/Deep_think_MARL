
#ifndef COSOCO_ATTRIBUTELIST_H_
#define COSOCO_ATTRIBUTELIST_H_


#include <libxml/xmlstring.h>
namespace XCSP3Core {
    /**
     * Represents the attribute list of a XML tag
     */
    class AttributeList {
    public:
        typedef unsigned char Byte;


        /**
         * an empty list of attributes
         */
        AttributeList() {
            n = 0;
            list = NULL;
        }

        AttributeList(const Byte **attr) {
            list = attr;

            n = 0;
            if (list == NULL)
                return;

            while (list[n] != NULL)
                n += 2;
            
            n /= 2;
        }

        inline int size() const {
            return n;
        }

        UTF8String operator[](const char *name) const {
            for (int i = 0; i < n; ++i)
                if (xmlStrEqual(list[2 * i], reinterpret_cast<const Byte *> (name)))
                    return UTF8string(list[2 * i + 1]);
            
            return UTF8String();
        }

        inline UTF8String getName(int i) const {
            return UTF8String(list[2 * i]);
        }

        inline UTF8String getValue(int i) const {
            return UTF8String(list[2 * i + 1]);
        }


    private:
        int n;  // num of attributes
        const Byte **list;  // list[2 * i] is the name of the i-th attributes,
        // list [2 * i + 1] is its value
    };

}



#endif  // COSOCO_ATTRIBUTELIST_H_