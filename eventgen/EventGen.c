#include <xdc/std.h>
#include <xdc/runtime/Log.h>

#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/LoggerBuf.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Types.h>

#include "package/internal/EventGen.xdc.h"

Void EventGen_genEvent(UArg arg)
{
    Log_write1(EventGen_LM_event, arg);

    /* For sanity-checking, dump the generated event. Only for LoggerBuf. */
    if (EventGen_dumpEvent)
        EventGen_printEvent();
}

Void EventGen_printEvent(Void)
{
    Text_RopeId rope;
    String  fmt;
    LoggerBuf_Handle logger;
    Log_EventRec er;

    /* This method is for use with LoggerBuf only! */
    logger = (LoggerBuf_Handle)EventGen_logger;

    LoggerBuf_getNextEntry(logger, &er);

    rope = Types_getEventId(er.evt);   /* the event id is the message rope */
    fmt = Text_ropeText(rope);
    System_printf("evt 0x%08x rope 0x%04x\n", er.evt, rope);

    /* Raw */
    System_printf("{evt: fmt=%p, args=[0x%x, 0x%x ...]}\n",
        fmt, er.arg[0], er.arg[1]);

    /* Strings resolved */
    if (Text_isLoaded) {
        System_printf(fmt, er.arg[0], er.arg[1], er.arg[2],
                er.arg[3], er.arg[4], er.arg[5], er.arg[6],
                er.arg[7]);
        System_printf("\n");
    }
}

/* See comment in .xdc */
Void EventGen_logEvent(UArg context, Log_EventRec *ev, Int nargs)
{
    Int i, b;
    Char *ptr;
    Types_RopeId id;

    /* marker */
    System_putch(0xf0);
    System_putch(0x0d);
    System_putch(0xca);
    System_putch(0xfe);

    id = Types_getEventId(ev->evt);   /* the event id is the message rope */
    for (b = 0; b < sizeof(id); ++b)
        System_putch(((UInt32)id >> (b * 8)) & 0xff);
    System_putch(nargs);
    for (i = 0; i < nargs; ++i) {
        for (b = 0; b < sizeof(ev->arg[i]); ++b)
            System_putch(((UInt32)ev->arg[i] >> (b * 8)) & 0xff);
    }
}
