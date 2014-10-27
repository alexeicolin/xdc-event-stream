package eventstream;

import xdc.runtime.Error;

module EventDecoder {

    /* Function that reads from the binary stream with the event data.  Takes
     * destination buffer and bytes to read, returns number bytes read, zero
     * on eof, or negative code on error. */
    typedef Int (*ReadFunc)(UChar *, Int);

    config Error.Id E_readMarkerCandidate = {
        msg: "failed to read marker candidate byte: rc %d"
    };
    config Error.Id E_readTimestamp = {
        msg: "failed to read timestamp (len %d): rc %d"
    };
    config Error.Id E_readSerial = {
        msg: "failed to read serial (len %d): rc %d"
    };
    config Error.Id E_readModuleId = {
        msg: "failed to read module ID (len %d): rc %d"
    };
    config Error.Id E_readEventId = {
        msg: "failed to read event ID (len %d): rc %d"
    };
    config Error.Id E_readNumArgs = {
        msg: "failed to read num args (len %d): rc %d"
    };
    config Error.Id E_readArgs = {
        msg: "failed to read args (len %d): rc %d"
    };
    config Error.Id E_invalidNumArgs = {
        msg: "invalid num args: %d (max %d)"
    };
    config Error.Id E_fmtBufTooSmall = {
        msg: "format string buffer too small: have %d need %d bytes"
    };

    /* Whether to lookup event format strings using the Text module.
     * Setting this to 'false' displays raw parsed events, which is
     * mainly useful for debugging. */
    config Bool resolveStrings = true;

    /* Function that reads from the binary stream with the event data */
    config ReadFunc readFunc = null;

    /* Read stream until an event is found and parsed. Return number of
     * events read (1 on success), or zero if eof, or negative if error.  */
    Int readEvent(Char *fmt, Int fmtLen, EventRecord.EventRec *ev, Error.Block *eb);

}
