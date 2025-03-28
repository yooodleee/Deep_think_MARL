
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
    public:
        int min, max;

        XIntegerInterval(int inf, int sup) : min(inf), max(sup) {}

        int width() override { return max - min + 1; }
        int minimum() override { return min; }
        int maximum() override { return max; }

        void print(std::ostream& O) const override { O << min << ".." << max << " "; }

        bool equals(XIntegerEntity* arg) override {
            XIntegerInterval* xii;
            if ((xii = dynamic_cast<XIntegerInterval*>(arg)) == NULL)
                return false;
            return min == xii->min && max == xii->max;
        }

        virtual ~XIntegerInterval() {}
    };

    class XDomain {

    };

    class XDomainInteger : public XDomain {
    // friend class XMLParser;
    protected:
        int size;
        int top {std::numeric_limits<int>::min()};

        void addEntity(XIntegerEntity* e) {
            values.push_back(e);
            size += e->width();
        }
    
    public:
        std::vector<XIntegerEntity*> values;

        XDomainInteger() : size(0) {}

        int nbValues() const {
            return size;
        }
        int minimum() {
            return values[0]->minimum();
        }
        int maximum() {
            return values[values.size() - 1]->maximum();
        }
        int isInterval() {
            return size == maximum() - minimum() + 1;
        }

        void addValue(int v) {
            if (v <= top)
                throw std::runtime_error("not sequence domain");
            addEntity(new XIntegerValue(top = v));
        }

        void addInterval(int min, int max) {
            if (min >= max || min <= top)
                throw std::runtime_error("not sequence domain");
            addEntity(new XIntegerInterval(min, top = max));
        }

        friend ostream& operator<<(ostream& f, const XDomainInteger& d);

        bool equals(XDomainInteger* arg) const {
            if (nbValues() != arg->nbValues())
                return false;
            if (values.size() != arg->values.size())
                return false;
            
            for (unsigned int i = 0; i < arg->values.size(); i++) {
                if (values[i]->equals(arg->values[i]) == false)
                    return false;
            }
            return true;
        }

        virtual ~XDomainInteger() {
            for (XIntegerEntity* xi : values)
                delete xi;
        }
    };
}


#endif  // XDOMAIN_H_