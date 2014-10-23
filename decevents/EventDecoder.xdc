module EventDecoder {

    /* Whether to lookup event format strings using the Text module.
     * Setting this to 'false' displays raw parsed events, which is
     * mainly useful for debugging. */
    config Bool resolveStrings = true;

    /* Whether the event data contains a timestamp for each event */
    config Bool timestamp = true;

    /* Execute the decoding loop */
    Void run();
}
