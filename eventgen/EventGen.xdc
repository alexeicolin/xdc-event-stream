import xdc.runtime.Diags;
import xdc.runtime.Log;

/* A simple test app that logs an event.
 *
 * It can optionally dump the raw event info to console in human readable
 * form or output event bytes in binary to console.
 *
 * See eventgenapp (esp. it's config).
 */
module EventGen
{
    config Log.Event LM_event = {
        mask: Diags.USER1,
        msg: "LM_event: arg 0x%08x"
    };

    Void genEvent(UArg arg);
}
