#include <xdc/std.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Log.h>

#include <ti/uia/runtime/EventHdr.h>
#include <ti/uia/sysbios/LoggerIdle.h>

#include <EventEncoder.h>

#include "package/internal/LoggerIdleAdapter.xdc.h"

/* Parses the event from raw binary data in the UIA/LoggerIdle format into a
 * Log.EventRec, then encodes it back into a binary stream using EventEncoder.
 *
 * TODO: move the conversion work to the host-side decoder (decevents), which
 * means decevents should support parsing streams in two different format */
Int LoggerIdleAdapter_writeEvents(UChar *a, Int size)
{
    Log_EventRec evrec;
    EventHdr_HdrType hdrType;
    Int     i;
    Int     nBytes;
    Int     nWords;
    UInt32 *wordPtr;
    UInt32 *logRec;
    UInt32 *args;
    Int     nArgs;
    Int     bytesSent = 0;
    Int     remainder = size;
    UInt    encodedLen = 0;

    logRec = (UInt32 *)a;

    while (remainder > 0) {
        wordPtr = logRec;
        hdrType = EventHdr_getHdrType(*wordPtr);
        nBytes = EventHdr_getLength(*wordPtr);
        nWords = nBytes / 4;

        evrec.serial = EventHdr_getSeqCount(*wordPtr);

        if (hdrType == EventHdr_HdrType_EventWithTimestamp) {
            evrec.evt = *(wordPtr + 3);
            evrec.tstamp.hi = *(wordPtr + 2);
            evrec.tstamp.lo = *(wordPtr + 1);
            args = wordPtr + 4;
            nArgs = nWords - 4;
        }
        else {
            evrec.evt = *(wordPtr + 1);
            evrec.tstamp.hi = 0;
            evrec.tstamp.lo = 0;
            args = wordPtr + 2;
            nArgs = nWords - 2;
        }

        if (nArgs > Log_NUMARGS) {
            /* Bad event header! */
            nArgs = Log_NUMARGS;
        }

        for (i = 0; i < nArgs; i++) {
            evrec.arg[i] = *(args + i);
        }
        for (i = nArgs; i < Log_NUMARGS; i++) {
            evrec.arg[i] = 0;
        }

        EventEncoder_writeEvent(&evrec, nArgs);

        bytesSent += nBytes;
        remainder -= nBytes;
        logRec += nWords;
    }

    return (bytesSent);
}

Void LoggerIdleAdapter_onAppExit(Int state)
{
    LoggerIdle_flush();
}
