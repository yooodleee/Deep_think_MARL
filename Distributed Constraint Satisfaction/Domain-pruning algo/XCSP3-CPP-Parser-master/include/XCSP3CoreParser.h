
#ifndef _XMLParser_libxml2_h_
#define _XMLParser_libxml2_h_


#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <climits>
#include <libxml/parser.h>


#include "XMLParser.h"
#include "XCSP3CoreCallbacks.h"
#include "XCSP3Constants.h"
#include "UTF8String.h"
// #defind debug


namespace XCSP3Core {
    using namespace std;


    /**
     * @brief the parser using the libxml2 library
     */
    class XCSP3CoreParser {
    
    protected:
        XMLParser cspParser;

    public:
        XCSP3CoreParser(XCSP3CoreCallbacks *cb) : cspParser(cb) {
            LIBXML_TEST_VERSION
        }

        int parse(istream &in);
        int parse(const char *filename);

    protected:
        // ##########################################################
        // SAX Handler
        // ##########################################################

        static void comment(void *parser, const xmlChar *value);
        static void startDocument(void *parser);
        static void endDocument(void *parser);
        static void characters(void *parser, const xmlChar *ch, int len);
        static void startElement(void *parser, const xmlChar *name, const xmlChar **attr);
        static void endElement(void *parser, const xmlChar *name);
    };
}



#endif  // _XMLParser_libxml2_h_