#ifndef CHELLO_H
#define CHELLO_H

#include <stdio.h>
#include "IHello.h"

class CHello : public IHello {
public:
    virtual const char* hello(const char* who);
};

#endif // CHELLO_H