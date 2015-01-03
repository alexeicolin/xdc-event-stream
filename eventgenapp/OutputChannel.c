#include <xdc/std.h>

#include "package/internal/OutputChannel.xdc.h"

Void OutputChannel_outputStdOut(const Char *buf, Int size)
{
    OutputChannel_ChannelProxy_outputStdOut(buf, size);
}

Void OutputChannel_outputEventLog(const Char *buf, Int size)
{
    OutputChannel_ChannelProxy_outputEventLog(buf, size);
}
