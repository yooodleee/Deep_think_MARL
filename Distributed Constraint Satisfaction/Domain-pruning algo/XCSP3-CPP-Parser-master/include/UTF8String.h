
#ifndef COSOCO_UTF8STRING_H_
#define COSOCO_UTF8STRING_H_


#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <climits>
#include <vector>


/**
 * A class to represent a UTF8 encoded string passed by the XML
 * parser. This string is normally read-only and the memory is not
 * owned by this class. The end of the string is represented
 * either by the first NULL character, or by and end pointer
 * (whichever comes first). The end pointer is used to represent
 * substring without having to copy the substring. 
 * 
 * The append() method provides a limited interface to a writeable
 * string. When this method is used, memory is allocated (and owned
 * by this class) and is automatically resized when necessary. This
 * is a kind of copy-on-write mechanism. 
 * 
 * This class can also represent a null string (in the SQL sense). 
 */

using namespace std;

namespace XCSP3Core {
    class UTF8String {
    public:
        typedef unsigned char Byte;

        static const int npos = INT_MAX;

        UTF8String();

        UTF8String(const UTF8String &s);
        UTF8String(const Byte *b, const Byte *e);
        UTF8String(const Byte *s);
        UTF8String(const char *s);

        ~UTF8String();

        bool isNull() const;

        bool empty() const;

        int byteLength() const;

        void clear();


        /**
         * an iterator on characters
         */
        class iterator {
        private:
            const Byte *p;
        
        public:
            iterator();

            explicit iterator(const Byte *s);

            int operator*();

            iterator &operator++();
            iterator operator++(int);
            iterator &operator--();
            iterator operator--(int);
            iterator &operator=(iterator it);

            bool operator!=(iterator it);
            bool operator==(iterator it);

            const Byte *getPointer() const;

            Byte firstByte() const;

            inline bool isWhiteSpace() const {
                return *p < 128 && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'
                                    || *p == '\v' || *p == '\f');
            }

        protected:

            /**
             * return the num of bytes of the current code point
             */
            inline int codeLength(int ch) {
                if (ch < 0x80)
                    return 1;   // only one byte
                else if (ch < 0xC2)
                    throw runtime_error("invalid UTF8 character");
                else if (ch < 0xD0)
                    return 2;   // 2 bytes
                else if (ch < 0xF0)
                    return 3;   // 3 bytes
                else if (ch < 0xF5)
                    return 4;   // 4 bytes
                else
                    throw runtime_error("invalid UTF8 character");
            }

            inline void addNextByte(int &ch) {
                ch <<= 6;
                ++p;
                if (*p < 0x80 || *p >= 0xC0)
                    throw runtime_error("invalid UTF8 character");
                ch |= *p & 0x3F;
            }

        };

        iterator begin() const;
        iterator end() const;

        void append(int ch);
        void append(UTF8String s);


        /**
         * returns true iff the string contains only white space
         */
        bool isWhiteSpace() const;

        int firstChar() const;

        int find(UTF8String sub) const;

        UTF8String substr(int pos, int count = npos);
        UTF8String substr(iterator beg, iterator end);

        bool operator==(const UTF8String s) const;
        bool operator!=(const UTF8String s) const;
        bool operator<(const UTF8String s) const;

        bool to(string &v) const;
        bool to(int &v) const;

        void appendTo(string &v) const;

        friend ostream &operator<<(ostream &f, const UTF8String s);

        class Tokenizer {
        private:
            iterator it, end;
            vector<int> separators;

        public:
            Tokenizer(const UTF8String s);
            void addSeparator(int ch);
            bool hasMoreTokens();
            UTF8String nextToken();



        protected:
            inline bool isSeparator(int ch) {
                for (vector<int>::const_iterator it = separators.begin();
                    it != separators.end(); ++it)
                    if (*it == ch)
                        return true;
                
                return false;
            }

            inline void skipWhiteSpace() {
                while (it != end && *it && it.isWhiteSpace())
                    ++it;
            }
        };

    protected:
        void resize();
        void write(Byte *&p, int ch);


    protected:
        const Byte *_beg;
        mutable const Byte *_end;
        int allocated;  // size of allocated array (or 0 if don't own the memory)
    };


}



#endif  // COSOCO_UTF8STRING_H_