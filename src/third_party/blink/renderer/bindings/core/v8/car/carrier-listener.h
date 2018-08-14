#ifndef RPCDEMO_CCARRIERLISTENER_H
#define RPCDEMO_CCARRIERLISTENER_H

#include <elastos/core/Object.h>
#include "ElastosCore.h"
#include "elatypes.h"
#include "v8.h"
#include "base/synchronization/lock.h"

#define MAX_EVENT_LEN 2048

class CCarrierListener
        : public Object
        , public ICarrierListener
{
public:

    CAR_INTERFACE_DECL()

    CCarrierListener(v8::Isolate* isolate, v8::Local<v8::Context> context)
//      : isolate_(isolate),
//        context_(isolate, context)
    {
    }

    ~CCarrierListener() override
    {

    }

    CARAPI OnIdle();

    CARAPI OnConnectionChanged(
        /* [in] */ _ELASTOS Boolean online);

    CARAPI OnReady();

    CARAPI OnFriendRequest(
        /* [in] */ const _ELASTOS String& uid,
        /* [in] */ const _ELASTOS String& hello);

    CARAPI OnFriendConnetionChanged(
        /* [in] */ const _ELASTOS String& uid,
        /* [in] */ _ELASTOS Boolean online);

    CARAPI OnPortForwardingRequest(
        /* [in] */ const _ELASTOS String& uid,
        /* [in] */ const _ELASTOS String& servicePort,
        /* [out] */ _ELASTOS Boolean * accept);

    CARAPI OnPortForwardingResult(
        /* [in] */ const _ELASTOS String& uid,
        /* [in] */ const _ELASTOS String& localPort,
        /* [in] */ const _ELASTOS String& remotePort,
        /* [in] */ _ELASTOS ECode code);

    CARAPI OnMessageReceived(
        /* [in] */ const _ELASTOS String& uid,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & message);

public:
    unsigned GetEvent(char** event);

private:
    //v8::Isolate* isolate_;
    //v8::Persistent<v8::Context> context_;
    base::Lock msg_buf_lock_;
    char event_[MAX_EVENT_LEN+1];

};

#endif //RPCDEMO_CCARRIERLISTENER_H
