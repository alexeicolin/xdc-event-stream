#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Error.h>

#include <EventRecord.h>
#include <Text.h>

#include <string.h>

#include "package/internal/EventDecoder.xdc.h"

Int EventDecoder_readEvent(Char *fmt, Int fmtLen,
                           EventRecord_EventRec *ev, Error_Block *eb)
{
    Int rc, i, len, actualFmtLen;
    String  textFmt;
    Char *sarg, *unresolvedFmt, *argFmt, *fmtp;
    UChar markerByte;
    Int nextMatch = 0;
    const Int maxArgs = sizeof(ev->arg) / sizeof(ev->arg[0]);

    memset(ev, 0, sizeof(*ev));

    /* Read byte-by-byte until lock in on the marker */
    nextMatch = 0;
    do {
        if ((rc = EventDecoder_readFunc((UChar *)&markerByte, 1)) != 1) {
            if (rc) {
                Error_raise(eb, EventDecoder_E_readMarkerCandidate, rc, 0);
                return rc;
            } else { /* eof */
                return 0;
            }
        }
        if (markerByte == EventRecord_eventMarker[nextMatch])
            nextMatch++;
        else
            nextMatch = 0;
    } while (nextMatch < EventRecord_eventMarkerLen);

    if (EventRecord_hasSerial) {
        len = sizeof(ev->serial);
        if ((rc = EventDecoder_readFunc((UChar *)&ev->serial, len)) != len) {
            Error_raise(eb, EventDecoder_E_readSerial, len, rc);
            return rc;
        }
    }
    if (EventRecord_hasTimestamp) {
        len = sizeof(ev->timestamp);
        if ((rc = EventDecoder_readFunc((UChar *)&ev->timestamp, len)) != len) {
            Error_raise(eb, EventDecoder_E_readTimestamp, len, rc);
            return rc;
        }
    }
    if (EventRecord_hasModuleId) {
        len = sizeof(ev->modid);
        if ((rc = EventDecoder_readFunc((UChar *)&ev->modid, len)) != len) {
            Error_raise(eb, EventDecoder_E_readModuleId, len, rc);
            return rc;
        }
    }
    len = sizeof(ev->id);
    if ((rc = EventDecoder_readFunc((UChar *)&ev->id, len)) != len) {
        Error_raise(eb, EventDecoder_E_readEventId, len, rc);
        return rc;
    }
    len = sizeof(ev->nargs);
    if ((rc = EventDecoder_readFunc((UChar *)&ev->nargs, len)) != len) {
        Error_raise(eb, EventDecoder_E_readNumArgs, len, rc);
        return rc;
    }
    if (ev->nargs > maxArgs) {
        Error_raise(eb, EventDecoder_E_invalidNumArgs, ev->nargs, maxArgs);
        return rc;
    }
    len = sizeof(ev->arg[0]) * ev->nargs;
    if ((rc = EventDecoder_readFunc((UChar *)&ev->arg, len)) != len) {
        Error_raise(eb, EventDecoder_E_readArgs, len, rc);
        return rc;
    }

    if (ev->id) { /* Log_write */
        if (EventDecoder_resolveStrings) {
            textFmt = Text_ropeText(ev->id);
            strncpy(fmt, textFmt, fmtLen - 1);
        } else {
            unresolvedFmt = "id 0x%04x nargs %d:";
            argFmt = " 0x%%08x";
            actualFmtLen = strlen(unresolvedFmt) + 8 + 10 + ev->nargs * strlen(argFmt);
            if (fmtLen <= actualFmtLen) {
                Error_raise(eb, EventDecoder_E_fmtBufTooSmall, fmtLen, actualFmtLen);
                return -1;
            }
            System_asprintf(fmt, unresolvedFmt, ev->id, ev->nargs);
            for (i = 0; i < ev->nargs; ++i)
                strcat(fmt, argFmt);
        }
    } else { /* Log_print */
        /* Format string is in the event stream data: read it byte by byte */
        fmtp = fmt;
        do {
            if ((rc = EventDecoder_readFunc((UChar *)fmtp, 1)) != 1) {
                Error_raise(eb, EventDecoder_E_readFmtString, 1, rc);
                return rc;
            }
        } while (*fmtp++);
    }

    /* Replace '%s' with '%p' because we ain't got the static strings */
    sarg = fmt;
    while ((sarg = strstr(sarg, "%s")) != NULL)
        fmt[sarg - fmt + 1] = 'p';

    /* Assert failures generate a event with fmt string '%$F%$S', which
     * probably has to do with XDC-specific printf functionality, but
     * in any case it crashes the decoder, so sanitize it. */
    sarg = fmt;
    while ((sarg = strstr(sarg, "%$")) != NULL)
        fmt[sarg - fmt + 1] = '%';

    return 1; /* read one event */
}
