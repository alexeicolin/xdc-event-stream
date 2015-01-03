#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <stdio.h>

#include "package/internal/ConsoleChannel.xdc.h"

Void ConsoleChannel_outputStdOut(const Char *buf, Int size) {
    // This is what SysMin does if no output func is defined
    fwrite(buf, 1, size, stdout);
}

Void ConsoleChannel_outputEventLog(const Char *buf, Int size) {
    Int i;
    for (i = 0; i < size; ++i)
        System_putch(buf[i]);
}
