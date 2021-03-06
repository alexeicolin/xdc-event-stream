#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Timestamp.h>

#include <EventRecord.h>
#include <NumFormat.h>

#include <string.h>

#include "package/internal/EventEncoder.xdc.h"

#if (xdc_target__bitsPerChar * xdc_target__sizeof_Int) == 32
#define SERIAL "#%010u "
#define HI "%0.8x:"
#define LO "%0.8x] "
#elif (xdc_target__bitsPerChar * xdc_target__sizeof_Long) == 32 
#define SERIAL "#%010lu "
#define HI "%0.8lx:"
#define LO "%0.8lx] "
#else
#error xdc.runtime.Log does not support this target. 
#endif

#define MICRO_MULTIPLIER 1000000

static char outbuf[1024];

/* Local memcpy: the real one doesn't exist when building for M4F (?) */
static Void lmemcpy(Char *dest, UChar *src, Int n)
{
    if (!n)
        return;
    do {
        --n;
        dest[n] = src[n];
    } while (n);
}

static UInt encodeEvent(Char *bufPtr, Log_EventRec *er, Int nargs)
{
    Text_RopeId rope;
    String  fmt;
    UInt64 timestamp;
    Types_FreqHz freq;
    UInt64 freqVal;

    if (EventEncoder_binaryOutput) {
        lmemcpy(bufPtr, EventRecord_eventMarker, EventRecord_eventMarkerLen);
        bufPtr += EventRecord_eventMarkerLen;
    }

    /* print serial number if there is one; 0 isn't a valid serial number */
    if (EventRecord_hasSerial && er->serial) {
        if (!EventEncoder_binaryOutput) {
            System_sprintf(bufPtr, SERIAL, er->serial);
            bufPtr = outbuf + strlen(outbuf);
        } else {
            lmemcpy(bufPtr, (UChar *)&er->serial, sizeof(er->serial));
            bufPtr += sizeof(er->serial);
        }
    }

    if (EventRecord_hasTimestamp != EventRecord_TimestampMode_NONE) {
        /* print timestamp if there is one; ~0 isn't a valid timestamp value */
        if (er->tstamp.hi != ~0 && er->tstamp.lo != ~0) {
            if (EventRecord_hasTimestamp == EventRecord_TimestampMode_COUNTS ||
                EventRecord_hasTimestamp == EventRecord_TimestampMode_US) {

                timestamp = (((UInt64)er->tstamp.hi) << 32) | ((UInt64)er->tstamp.lo);

                if (EventRecord_hasTimestamp == EventRecord_TimestampMode_US) {
                    Timestamp_getFreq(&freq);
                    freqVal = (((UInt64)freq.hi) << 32) | ((UInt64)freq.lo);
                    /* timestamp in us = timestamp in counts / freqVal * 10^6 */
                    /* To perfrom that calculation in integer math,
                     * we have some cases */
                    if (freqVal > MICRO_MULTIPLIER) {
                        timestamp /= freqVal / MICRO_MULTIPLIER;
                    }
                    else {
                        timestamp *= MICRO_MULTIPLIER;
                        timestamp /= freqVal;
                    }
                }

                System_sprintf(bufPtr, "%s:", NumFormat_format(timestamp, 0, 10));

            }
            else { /* EventRecord_TimestampMode_RAW */
                if (!EventEncoder_binaryOutput) {
#if 0
                    System_sprintf(bufPtr, "%010u:%010u ",
                                   er->tstamp.hi, er->tstamp.lo);
#else
                    /* TODO: this logic is duplicated in decevents app */
                    System_sprintf(bufPtr, "%05u:%05u:%010u ",
                        /* Up-to-date ticks in Clock module state */
                        er->tstamp.hi >> 16,
                        /* Timestamp ticks: together with current timer value
                         * makes up the elapsed time. */
                        er->tstamp.hi & 0xffff,
                        er->tstamp.lo);
#endif
                    bufPtr = outbuf + strlen(outbuf);
                } else {
                    lmemcpy(bufPtr, (UChar *)&er->tstamp, sizeof(er->tstamp));
                    bufPtr += sizeof(er->tstamp);
                }
            }
        }
    }

    /* print module name */
    if (EventRecord_hasModuleId) {
        rope = Types_getModuleId(er->evt);
        if (!EventEncoder_binaryOutput) {
            Text_putMod(rope, &bufPtr, -1);
            System_sprintf(bufPtr, ": ");
            bufPtr = outbuf + strlen(outbuf);
        } else {
            lmemcpy(bufPtr, (UChar *)&rope, sizeof(rope));
            bufPtr += sizeof(rope);
        }
    }
    
    /* print event */
    rope = Types_getEventId(er->evt);   /* the event id is the message rope */
    if (rope == 0) {
        /* Log_print() event */
        if (!EventEncoder_binaryOutput) {
            System_asprintf(bufPtr, (String)iargToPtr(er->arg[0]),
                er->arg[1], er->arg[2], er->arg[3], er->arg[4],
                er->arg[5], er->arg[6], 0, 0);
            bufPtr = outbuf + strlen(outbuf);
            *bufPtr++ = '\n';
        } else {
            nargs--; /* don't count arg[0], the format string */
            lmemcpy(bufPtr, (UChar *)&rope, sizeof(rope));
            bufPtr += sizeof(rope);
            lmemcpy(bufPtr, (UChar *)&nargs, sizeof(Char));
            bufPtr += sizeof(Char);
            lmemcpy(bufPtr, (UChar *)&er->arg[1], sizeof(er->arg[0]) * nargs);
            bufPtr += sizeof(er->arg[0]) * nargs;
            strcpy(bufPtr, (Char *)iargToPtr(er->arg[0]));
            bufPtr += strlen((Char *)iargToPtr(er->arg[0])) + 1; /* for null byte */
        }
    }
    else {
        /* Log_write() event */
        if (!EventEncoder_binaryOutput) {
            fmt = Text_ropeText(rope);

            System_asprintf(bufPtr, fmt, er->arg[0], er->arg[1], er->arg[2],
                    er->arg[3], er->arg[4], er->arg[5], er->arg[6],
                    er->arg[7]);
            bufPtr = outbuf + strlen(outbuf);
            *bufPtr++ = '\n';
        }
        else {
            lmemcpy(bufPtr, (UChar *)&rope, sizeof(rope));
            bufPtr += sizeof(rope);
            lmemcpy(bufPtr, (UChar *)&nargs, sizeof(Char));
            bufPtr += sizeof(Char);
            lmemcpy(bufPtr, (UChar *)er->arg, sizeof(er->arg[0]) * nargs);
            bufPtr += sizeof(er->arg[0]) * nargs;
        }
    }

    return bufPtr - outbuf;
}

Void EventEncoder_writeEvent(Log_EventRec *ev, Int nargs)
{
    UInt encodedLen = 0;
    encodedLen = encodeEvent(outbuf, ev, nargs);
    EventEncoder_outputFunc(outbuf, encodedLen);
}

