#include <xdc/std.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Types.h>

#include "package/internal/EventGen.xdc.h"

Void EventGen_genEvent(UArg arg)
{
    Log_write1(EventGen_LM_event, arg);
}
