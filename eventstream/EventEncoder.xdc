package eventstream;

import xdc.runtime.Log;

module EventEncoder {
    typedef Void (*OutputFunc)(Char *, UInt);

    /* Whether to output events in human readable form as strings or
     * as binary data to be decoded by EventDecoder on the host.
     * Set equal to !Text.isLoaded by default. */
    config Bool binaryOutput;

    /* Function that forwards the buffer with event data (either human
     * readable string or binary, depending on `binaryOutput`) onwards
     * to the UART or elsewhere. */
    config OutputFunc outputFunc = null;

    /* Packages event binary data and passes to 'outputFunc'. */
    Void writeEvent(Log.EventRec *ev, Int nargs);
}
