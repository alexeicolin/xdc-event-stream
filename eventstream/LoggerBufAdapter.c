#include <xdc/std.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/LoggerBuf.h>

#include <EventEncoder.h>

#include "package/internal/LoggerBufAdapter.xdc.h"

Void LoggerBufAdapter_writeEvents()
{
    Assert_isTrue(LoggerBufAdapter_loggerInstance != NULL, NULL);
    Log_EventRec er;
    while (LoggerBuf_getNextEntry(LoggerBufAdapter_loggerInstance, &er))
        EventEncoder_writeEvent(&er, Log_NUMARGS); /* don't have actual num */
}

Void LoggerBufAdapter_onAppExit(Int stat)
{
    LoggerBufAdapter_writeEvents();
}
