import xdc.runtime.Diags;
import xdc.runtime.Log;
import xdc.runtime.ILogger;

/* A simple test app that logs an event.
 *
 * It can optionally dump the raw event info to console in human readable
 * form or output event bytes in binary to console.
 *
 * See eventgenapp (esp. it's config).
 */
module EventGen
{
    /* This is for debugging purposes. Wouldn't need it if could
     * get to common$.logger */
    config ILogger.Handle logger = null;
    
    /* For debugging: flag tells app to dump human-readable raw event data to
     * console (implemented in app) */
    config Bool dumpEvent = false;

    config Log.Event LM_event = {
        mask: Diags.USER1,
        msg: "LM_event: arg 0x%08x"
    };

    Void genEvent(UArg arg);

    /* For debugging: dumps one event from LoggerBuf buffer */
    Void printEvent();

    /* For debugging: this does a subset of what tivaapps/event_logger.c
     * does but works (1) outside of SYSBIOS and (2) on x86.
     * TODO: unify this with tivaapps/event_logger.c in a proper module */
    Void logEvent(UArg context, Log.EventRec *ev, Int nargs);
}
