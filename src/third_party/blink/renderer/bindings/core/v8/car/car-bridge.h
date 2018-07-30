#ifndef __CARMANAGER_H__
#define __CARMANAGER_H__

#include "v8.h"

namespace ela {

class CarManager {
public:
   static bool initialize(v8::Isolate* isolate);

};

class Carrier {
public:
	static bool initialize(v8::Isolate* isolate);
};

} // namespace ela

#endif // __CARMANAGER_H__
