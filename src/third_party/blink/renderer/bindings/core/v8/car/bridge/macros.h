#ifndef __CAR_BRIDGE_MACROS_H
#define __CAR_BRIDGE_MACROS_H

#define CAR_BRIDGE_NAMESPACE_BEGIN \
namespace Elastos { \
namespace CARBridge {

#define CAR_BRIDGE_NAMESPACE_END \
} \
}

#define CAR_BRIDGE ::Elastos::CARBridge

#define CAR_BRIDGE_NAMESPACE_USING \
using namespace ::Elastos::CARBridge;

#define LOG(TYPE, MSG) printf("%d %d", TYPE, MSG)
//#define LOG(format, ...) fprintf(stdout, format, __VA_ARGS__)

#endif
