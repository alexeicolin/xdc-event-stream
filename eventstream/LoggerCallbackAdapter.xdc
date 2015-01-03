package eventstream;

import xdc.runtime.Log;

module LoggerCallbackAdapter {
    /* Event processing callback to be given to LoggerCallback */
    Void writeEvent(UArg context, Log.EventRec *ev, Int nargs);
}
