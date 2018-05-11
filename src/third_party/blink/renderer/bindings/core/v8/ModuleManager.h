#ifndef ModuleManager_h
#define ModuleManager_h

#include <v8.h>

namespace ela {
class ModuleManager {
public:
   static bool initialize(v8::Isolate* isolate);
};

} // namespace ela

#endif // ModuleManager_h
