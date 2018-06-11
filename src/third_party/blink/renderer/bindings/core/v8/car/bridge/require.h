#ifndef __CAR_BRIDGE_REQUIRE_H
#define __CAR_BRIDGE_REQUIRE_H

#include <cstddef>
#include <cstdint>
#include <nan.h>
#include "macros.h"

CAR_BRIDGE_NAMESPACE_BEGIN

extern v8::Local<v8::Value> Require(char const *ecoPath, char const *const entryId);

CAR_BRIDGE_NAMESPACE_END

#endif
