#include <xdc/std.h>

#include <string.h>

#include "package/internal/NumFormat.xdc.h"

/* Large enough to hold string repr of a 64-bit number in base 2 */
#define MAX_NUM_LENGTH 64
static Char numberBuf[MAX_NUM_LENGTH + 1];  /* one for null byte */
static Char *numberBufEnd = numberBuf + MAX_NUM_LENGTH;
static const Char digtohex[] = "0123456789abcdef";

Char* NumFormat_format(UInt64 un, Int zpad, Int base)
{
    Int i = 0;
    Char sign = 0;

    UInt64 n;
    n = un;

    Char *ptr = numberBufEnd;
    *ptr = '\0';

    if (base < 0) {
        /* handle signed long case */
        base = -base;
        if ((Int64)n < 0) {
            n = -(Int64)n;

            /* account for sign '-': ok since zpad is signed */
            --zpad;
            sign = '-';
        }
    }

    /* compute digits in number from right to left */
    do {
        *(--ptr) = digtohex[(Int) (n % base)];
        n = n / base;
        ++i;
    } while (n);

    /* pad with leading 0s on left */
    while (i < zpad) {
        *(--ptr) = '0';
        ++i;
    }

    /* add sign indicator */
    if (sign) {
        *(--ptr) = sign;
    }

    return (ptr);
}
