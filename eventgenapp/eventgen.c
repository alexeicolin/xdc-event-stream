#include <xdc/std.h>
#include <eventgen/EventGen.h>

#include <xdc/cfg/global.h>

/* A simple app for playing around with log events */

Int main(Int argc, Char* argv[])
{
    EventGen_genEvent(42);
    return 0;
}
