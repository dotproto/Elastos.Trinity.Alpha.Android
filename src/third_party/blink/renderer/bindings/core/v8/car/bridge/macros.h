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

void Carbridge_throw(const char *func, const char *file, int line);
#define Throw_LOG(TYPE, MSG) Carbridge_throw(__FUNCTION__, __FILE__, __LINE__);
//#define Throw_LOG(TYPE, MSG) LOG(INFO) << __FUNCTION__ << " at " << __FILE__  << ":" << __LINE__<< " ";
#endif
