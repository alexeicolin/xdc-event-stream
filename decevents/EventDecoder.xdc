module EventDecoder {

    /* Whether to lookup event format strings using the Text module.
     * Setting this to 'false' displays raw parsed events, which is
     * mainly useful for debugging. */
    config Bool resolveStrings = true;

    /* Execute the decoding loop */
    Void run();
}
