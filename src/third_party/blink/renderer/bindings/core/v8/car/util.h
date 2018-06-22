#ifndef __CAR_BRIDGE_UTIL_H
#define __CAR_BRIDGE_UTIL_H
#include <nan.h>
#include <elastos.h>
#include "macros.h"
CAR_BRIDGE_NAMESPACE_BEGIN
extern IInterface *Probe(v8::Local<v8::Object> object, Elastos::EIID const &iid);
extern void AttachAspect(v8::Local<v8::Object> object, Elastos::ClassID const &aspectId);
extern void DetachAspect(v8::Local<v8::Object> object, Elastos::ClassID const &aspectId);
CAR_BRIDGE_NAMESPACE_END
#endif
