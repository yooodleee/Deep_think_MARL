
#ifndef XDOMAIN_H_
#define XDOMAIN_H_


#include <limits>
#include <vector>
#include <iostream>
#include <stdexcept>


using namespace std;
namespace XCSP3Core {

    class XIntegerEntity {
    public:
        virtual int width() = 0;
        virtual int minimum() = 0;
        virtual int maximum() = 0;
        virtual void print(std::ostream &O) const = 0;

        friend ostream &operator<<(ostream &f, const XIntegerEntity &ie);
        virtual bool equals(XIntegerEntity *arg) = 0;
        virtual ~XIntegerEntity() {}
        
    };

    class XIntegerValue : public XIntegerEntity {
    public:
        int value;

        XIntegerValue(int v) : value(v) {}

        int width() override { return 1; }
        int minimum() override { return value; }
        int maximum() override { return value; }

        void print(std::ostream &O) const override { O << value << " "; }

        bool equals(XIntegerEntity *arg) override {
            XIntegerValue *xiv;
            if ((xiv = dynamic_cast<XIntegerValue*>(arg)) == NULL)
                return false;
            return value == xiv->value;
        }

        virtual ~XIntegerValue() {}
    };

    class XIntegerInterval : public XIntegerEntity {
        
    }
}