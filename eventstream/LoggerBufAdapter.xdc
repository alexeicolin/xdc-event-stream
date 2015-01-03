package eventstream;

import xdc.runtime.LoggerBuf;

module LoggerBufAdapter {

    /* Application must set 'loggerInstance' reference to a LoggerBuf instance
     * at configuration time. The reference is needed because we have not
     * choice but to pull events out of LoggerBuf, since LoggerBuf does not
     * expose any callbacks upon flush. */
    config LoggerBuf.Handle loggerInstance;

    /* Pull events from LoggerBuf and ship them to the eventstream.EventEncoder.
     * This is automatically called upon application exit and can also be
     * called at any convenient time from the application. */
    Void writeEvents();

    /* Hook for flushing events from LoggerBuf when app exits. */
    Void onAppExit(Int stat);
}
