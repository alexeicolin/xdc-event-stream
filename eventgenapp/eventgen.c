#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <eventgen/EventGen.h>

/* A simple app for playing around with log events */

Int main(Int argc, Char* argv[])
{
    System_printf("Hello world! Generating a log event...\n");

    EventGen_genEvent(42);

    return 0;
}
