#ifndef IHELLO_H
#define IHELLO_H

class IHello {

public:
    IHello(){}
    virtual ~IHello() {}
    virtual const char* hello(const char* who) = 0;
};

// the types of the class factories
typedef IHello* create_t();
typedef void destroy_t(IHello*);

extern "C" IHello* Create();
extern "C" void Destroy(IHello* p);

#endif // IHELLO_H