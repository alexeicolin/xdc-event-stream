#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <eventstream/EventDecoder.h>
#include <eventstream/EventRecord.h>

#include <stdio.h>
#include <string.h>
#include <err.h>

static FILE *fin = NULL;
static Char fmt[256];

Int readEventData(UChar *buf, Int len)
{
    Int rc = fread(buf, len, 1, fin);
    return rc <= 0 ? rc : len;
}

Int main(Int argc, Char* argv[])
{
    Char *finPath = NULL;
    Int numEvents;
    Error_Block eb;
    EventRecord_EventRec ev;

    if (!(argc == 1 || argc == 2)) {
        errx(1, "Usage: %s [<intput file>]", argv[0]);
    }

    if (argc == 2)
        finPath = argv[1];

    if (finPath) {
        if (!(fin = fopen(finPath, "r")))
            err(1, "failed to open input file: '%s'", finPath);
    } else {
        fin = stdin;
    }

    Error_init(&eb);

    while ((numEvents = EventDecoder_readEvent(fmt, sizeof(fmt), &ev, &eb)) > 0) {
        System_printf("%010u:%010u ", ev.timestamp.hi, ev.timestamp.lo);
        System_printf(fmt, ev.arg[0], ev.arg[1], ev.arg[2],
                ev.arg[3], ev.arg[4], ev.arg[5], ev.arg[6],
                ev.arg[7]);
        System_printf("\n");

        System_flush();
    }

    if (Error_check(&eb)) {
        Error_print(&eb);
        err(1, "event decoder error (errno)");
    }

    if (finPath)
        fclose(fin);

    return 0;
}
