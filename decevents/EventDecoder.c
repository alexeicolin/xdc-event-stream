#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>

#include <Text.h>

#include "package/internal/EventDecoder.xdc.h"

#include <stdio.h>
#include <string.h>
#include <err.h>

/* #define DEBUG */

static UChar marker[] = {0xf0, 0x0d, 0xca, 0xfe};

/* TODO: Does it make sense to reuse Log_EventRec? */
struct EventRec {
    UInt32 marker;
    Types_Timestamp64 timestamp;
    UInt16 modid;
    UInt16 id;
    UInt8 nargs;
    UInt32 arg[8];
};

static Char fmtBuf[256];

Void EventDecoder_run()
{
    Int rc, i;
    struct EventRec ev;
    Text_RopeId rope;
    String  fmt;
    Char *fmtbp, *sarg;
    FILE *fin = stdin;
    UChar markerByte;
    Int nextMatch = 0;

    while (1) {

        /* Read byte-by-byte until lock in on the marker */
        nextMatch = 0;
        do {
            if ((rc = fread(&markerByte, 1, 1, fin)) != 1) {
                if (rc)
                    err(1, "failed to read marker candidate: rc %d", rc);
                else /* eof */
                    return;
            }
            if (markerByte == marker[nextMatch])
                nextMatch++;
            else
                markerByte = 0;
        } while (nextMatch < sizeof(marker));

        if (EventDecoder_timestamp) {
            if ((rc = fread(&ev.timestamp, sizeof(ev.timestamp), 1, fin)) != 1)
                err(1, "failed to read timestamp: rc %d", rc);
        }
        if ((rc = fread(&ev.modid, sizeof(ev.modid), 1, fin)) != 1)
            err(1, "failed to read module id: rc %d", rc);
        if ((rc = fread(&ev.id, sizeof(ev.id), 1, fin)) != 1)
            err(1, "failed to read id: rc %d", rc);
        if ((rc = fread(&ev.nargs, sizeof(ev.nargs), 1, fin)) != 1)
            err(1, "failed to read nargs: rc %d", rc);
        if (ev.nargs > sizeof(ev.arg) / sizeof(ev.arg[0]))
            errx(1, "invalid narg value in trace: %d", ev.nargs);
        if ((rc = fread(&ev.arg, sizeof(ev.arg[0]), ev.nargs, fin)) != ev.nargs)
            err(1, "failed to read args: rc %d", rc);

        if (EventDecoder_resolveStrings) {

            fmt = Text_ropeText(ev.id);

            /* Replace '%s' with '0x%08p' because we ain't got the strings */
            if ((sarg = strstr(fmt, "%s")) != NULL) {
                memset(fmtBuf, 0, sizeof(fmtBuf));
                strncpy(fmtBuf, fmt, sizeof(fmtBuf) - 1);
                fmtBuf[sarg - fmt + 1] = 'p';
                fmt = fmtBuf;
            }

            System_printf("%010u:%010u ", ev.timestamp.hi, ev.timestamp.lo);
            System_printf(fmt, ev.arg[0], ev.arg[1], ev.arg[2],
                    ev.arg[3], ev.arg[4], ev.arg[5], ev.arg[6],
                    ev.arg[7]);
            System_printf("\n");
        } else {
            System_printf("evt: time %010u:%010u id 0x%02x nargs %d",
                    ev.timestamp.hi, ev.timestamp.lo, ev.id, ev.nargs);
            for (i = 0; i < ev.nargs; ++i)
                System_printf(" 0x%08x", ev.arg[i]);
            System_printf("\n");
        }

        System_flush();
    }
}
