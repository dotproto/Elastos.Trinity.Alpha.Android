#ifndef V8CHello_h
#define V8CHello_h

#include <v8.h>
#include "bindings/core/v8/IHello.h"

namespace ela {

class V8CHello {
public:
    static void installV8CHelloTemplate(v8::Isolate* isolate);
};

} // namespace ela
#endif // V8CHello_h
