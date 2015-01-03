package eventstream;

import xdc.runtime.Types;

module EventRecord {

    struct EventRec {
        UInt32 marker;
        Types.Timestamp64 timestamp;
        UInt32 serial;
        UInt16 modid;
        UInt16 id;
        UInt8 nargs;
        UInt32 arg[8];
    };

    enum TimestampMode {
        TimestampMode_NONE,
        TimestampMode_US, /* in microseconds: ticks are converted based on freq */
        TimestampMode_COUNTS, /* in timer counts */
        TimestampMode_RAW /* Clock_ticks:Timer_counter */
    };

    config UChar eventMarker[] = [0xf0, 0x0d, 0xca, 0xfe];
    config Int eventMarkerLen; /* set in module$meta$init */

    /* What kind of timestamp to include in the event data for each event */
    config TimestampMode hasTimestamp = TimestampMode_NONE;

    /* Whether to include a serial number into each event (if one is valid) */
    config Bool hasSerial = false;

    /* Whether to include the source module id number into each event */
    config Bool hasModuleId = true;
}
