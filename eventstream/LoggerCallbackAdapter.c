#include <xdc/std.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Log.h>

#include <EventEncoder.h>

#include "package/internal/LoggerCallbackAdapter.xdc.h"

/* Callback for use with xdc.runtime.LoggerCallback */
Void LoggerCallbackAdapter_writeEvent(UArg context, Log_EventRec *ev, Int nargs)
{
    EventEncoder_writeEvent(ev, nargs);
}
