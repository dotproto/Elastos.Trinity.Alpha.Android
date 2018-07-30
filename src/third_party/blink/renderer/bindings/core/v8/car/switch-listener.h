//
// Created by 林楠 on 2018/4/11.
//

#ifndef RPCDEMO_CMESSAGELISTENER_H
#define RPCDEMO_CMESSAGELISTENER_H

#include <elastos/core/Object.h>
#include "v8.h"
#include "ElastosCore.h"
#include "elatypes.h"
//#include "Switch.h"

class CSwitchListener
    : public Object
    , public ISwitchListener
{
public:
    CAR_INTERFACE_DECL()

    CSwitchListener(v8::Isolate* isolate)
    {
        mIsolate = isolate;
    }

    virtual ~CSwitchListener()
    {
    }

    CARAPI OnSwitch1Changed(
        /* [in] */ Boolean on);

    CARAPI OnSwitch2Changed(
        /* [in] */ Boolean on);

private:
    v8::Isolate* mIsolate;

};
#endif //RPCDEMO_CMESSAGELISTENER_H
