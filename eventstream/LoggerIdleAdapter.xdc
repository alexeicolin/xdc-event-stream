package eventstream;

module LoggerIdleAdapter {
    /* Event processing callback to be given to LoggerIdle */
    Int writeEvents(UChar *a, Int size);

    /* Hook for flushing events from LoggerIdle buffer when app exits. */
    Void onAppExit(Int stat);
}
