Synopsis
========

Stream `xdc.runtime.Log` events from RTSC modules on the target to a host in a
binary format delegating printf formatting to the host.

The log events differ from the system standard output (`System_printf`) in
their low overhead, flexible buffering via various `xdc.runtime.Logger*`
modules. This repository fills the gap of a *simple* and *composable* host-side
utility for retrieving the events from the target. The utility does not depend
on the existing host-side Java framework (too complicated) nor on any IDEs.

Consider some target code that generates events, including existing XDC
packages, such as SYSBIOS which generates system events such as task context
switches:

    module EventGen
    {
        config Log.Event LM_event = {
            mask: Diags.USER1,
            msg: "LM_event: arg 0x%08x"
        };
    }

    ...

    Log_write1(EventGen_LM_event, arg);

The tool makes the event log available on the host in a *composable* way:

    $ ./decevents.x86U /dev/ttyACM0
    0x801a LM_event: the answer is 0x0000002a

This repository provides three pieces:

1. event encoder RTSC *module*, for integration into the app on the target, for
  redirecting event log from the target via a channel to the host, 

2. event decoder host-side *app* for parsing the binary event stream, formatting
  the events using format strings pre-extracted from the target binary,

3. an event generator example application, runnable on both the target and the
   host (for most configurations), for exploring various logger modules and
   eventstream usage.

Build Prerequisites
===================

The `build-env.sh` script shows the environment variables that need to be set to the
path of each dependency. Customize it and source it by `. ./build-env.sh`.

XDCTools
--------

Install [XDCTools Core
Distribution](http://downloads.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/rtsc/index.html)
and add the installation directory to your `PATH` to access `xs` and `xdc`
toosl it provides. This is done by `build-env.sh` script.

Target Packages
---------------

The target-side event stream tools are runnable both on the target and on the
host for quick testing and development. The host-side tools (event decoder
`decevents`) builds only for the host. The code was developed and tested on a
Linux x86 host and TI Tiva C LaunchPad target. The respective target packages
are `gnu.targets.Linux86` and `gnu.targets.arm.M4F` plus platform
`ti.platforms.tiva:TM4C123GH6PM:1` (the 1 means to use a default linker
script).

The target packages are shipped separately from XDCTools since v3.30. They are
present in [SYSBIOS](http://www.ti.com/tool/sysbios), which is available as a
standalone distribution or as part of
[TI-RTOS](http://www.ti.com/tool/ti-rtos).  Download and install the SYSBIOS
distribution. The destination directory is assumed to be `bios` in what
follows.

`gnu.targets.rts86U`
--------------------

The runtime support package (contains the library `xdc.runtime` among other
things) usually "comes with" the target, but `gnu.targets.rts86U` is an
exception. It is not included in either SYSBIOS or TI-RTOS v2 for Tiva C
distributions ([forum post about
this](https://www.eclipse.org/forums/index.php/t/918197/)). It can be found in
the *legacy* distribution of XDCTools v3.25. Download the legacy distribution
and install (to `xdctools_v3.25`). The commands below use `repoman` to copy the
needed package from the XDCTools v3.25 install repository into a separate
repository, i.e. a new directory, `rts/packages`. This is required, because
having v3.25 package path in the `XDCPATH` together with v3.30 package path
breaks the build. Furthermore, it is required to rebuild `rts86U` package
because there are incompatibilities between XDCTools v3.30 and v3.25, the v3.25
binary of `rts86U` fails to link into the application that is built using v3.30
(the failure is due to multiply defined symbols).

    xs xdc.tools.repoman -c -p xdctools_v3.25/packages -r rts/packages \
        gnu.targets.rts86U
    cd rts/packages/gnu/rts/rts86U
    xdc clean
    xdc XDCPATH=bios/packages all

Universal Instrumentation Architecture (UIA)
--------------------------------------------

Only for `ti.uia.sysbios.LoggerIdle` logger the UIA framework is required. UIA
is available as a [standalone
distribution](http://downloads.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/uia/index.html)
or as part of TI-RTOS. Download and install
[UIA](http://www.ti.com/tool/ti-rtos). The destination directory is assumed to
be `uia` in what follows.

SYSBIOS
-------

Only for `ti.uia.sysbios.LoggerIdle` logger the SYSBIOS kernel is required.
[SYSBIOS](http://www.ti.com/tool/sysbios) is available as a standalone
distribution or as part of [TI-RTOS](http://www.ti.com/tool/ti-rtos). The
destination directory is assumed to be `uia` in what follows.

TivaWare
--------

For building for the TI Tiva C platform (as opposed only for x86 host), the
TivaWare library is required. This library does not provide an RTSC package but
a native C library archive, called `driverlib`.
[TivaWare](http://www.ti.com/tool/sw-tm4c) is available as a standalone
distribution or as part of [TI-RTOS](http://www.ti.com/tool/ti-rtos). Install
and set `TIVAWARE_INSTALLATION_DIR` environment variable to the installation
destination path.

The `eventgenapp` package build (`package.bld`) script (along with a workaround
in the global `config.bld` script) contains the necessary compiler and linker
arguments necessary to integrate the library. These came from `Board.xs` in
TI-RTOS examples source tree.

OpenOCD
-------

OpenOCD is used for programming the example application onto TI Tiva C
Launchpad board over the ICDI interface. The config files for the board are
provided in `openocd/` directory. See Flash section below.

Build
=====

To build all packages in this event stream tools distribution, run this in the
root directory of this repository:

    export XDCPATH="bios/packages;rts/packages;uia/packages"
    xdc -Pr .

By default the `eventstream` package will build one main library archive and
one archive per logger adapter module. It is also possible to build the adapter
libraries only for specified logger types, thereby eliminating the need to
install some of the above dependencies, for example the following will not
build LoggerIdle adapter, for which neither SYSBIOS nor UIA installation is
necessary:

    cd eventstream
    xdc XDCARGS="LoggerSys LoggerBuf" all

Similarly, by default, the `eventgenapp` example app builds one executable per
target, per logger, per event destination, for example:
`eventgen-LoggerBuf-stream.xm4fd`.  The package supports building only specific
configs by specifying the logger names with an optional suffix indicating the
event log destination `-stdout` or `-stream`:

    cd eventgenapp
    xdc XDCARGS="LoggerSys LoggerBuf-stream" all

Flash
=====

To write the example event generator application onto the flash of TI Tiva C
Launchpad board:

    cd eventgenapp
    openocd -f ../openocd/ek-tm4c123gxl.cfg -c \
        'program eventgen-LoggerCallback-stream.xm4fg 0x00000000'

Usage
=====

EventEncoder (target)
---------------------
To "grab" the stream stream on the target and forward it to the host, the
`eventstream.EventEncoder` and  `eventstream.EventRecord` modules must be
configured in the application `.cfg` config file. Below is an example of
hooking up the event encoder to the LoggerCallback module. See
`eventgenapp/eventgen.cfg` for an example for each of the other loggers.

    // Exclude event message format strings from final linked binary
    Text.isLoaded = false;

    // Configure what is transmitted with each event
    var EventRecord = xdc.useModule('eventstream.EventRecord');
    EventRecord.hasModuleId = true;

    var EventEncoder = xdc.useModule('eventstream.EventEncoder');
    EventEncoder.outputFunc = '&sendBytesToHost';
    EventEncoder.binaryOutput = !Text.isLoaded;

    var Logger = xdc.useModule('xdc.runtime.LoggerCallback');
    var LoggerCallbackAdapter = xdc.useModule('eventstream.LoggerCallbackAdapter');
    Logger.outputFxn = '&eventstream_LoggerCallbackAdapter_writeEvent';
    loggerInstance = Logger.create();

The application must then implement `sendBytesToHost(const Char *buf, Int
size)` method. In the example `eventgen` app, this is accomplished through
and `IOutputChannel` abstraction (see Example App section).

EventDecoder in `decevents` (host)
----------------------------------

A *binary* event stream needs to be decoded and formatted into text form. The
`decevents` application built for the host to does this. It is not necessary
when `EventEncoder.binaryOutput` is `false`, since the stream is in text form
and can be displayed by `cat /dev/tty*`.

Decoding the event stream includes formating the event log messages. The event
log message format strings specified in the event declarations (in module's
`*.xdc`) can be excluded from the application binary by setting `Text.isLoaded`
to `false`. In this case the transmitted event stream includes only event IDs,
which needs to be mapped back to the corresponding format strings on the host.
The current implementation supports `binaryOutput` set to `true` only when
`isLoaded` is set to `false`, because the format string extraction for the
decoder relies on them being placed in a designated `xdc.noload` section which
happens when `isLoaded` is set to `false`.

1. Extract log message format strings from the linked application binary
   and save them as two binary files (`chartab.bin` and `nodetab.bin`). For
   example, for the app binary:

        ./extract-bin.sh ./eventgenapp/eventgen-LoggerCallback-stream.xm5fg

2. (Re-)build the decoder, which looks for `*.bin` files in the root directory.

        cd decevents
        xdc clean && xdc all

3. Configure TTY device(s) to not interfere with data bytes:

        stty -F /dev/ttyACM0 raw

   Make this permanent by a `udev` rule in `/etc/udev/rules.d/tiva-uart.rules`:

        KERNEL=="ttyACM0", RUN+="/bin/stty -F /dev/ttyACM0 115200 raw"

4. Receive and decode the event log stream:

        ./decevents/decevents.x86U /dev/ttyACM0

TODO: This is inconvenient, would be nice to have a better string extraction
method.

Example App
-----------

The `eventstream` package is demonstrated on an example application. The app
generates one event and can be configured to use various loggers and to forward
events via the EventEncoder. A normal application is expected to have only one
of the config clauses present in `eventgen.cfg`. The app can be built for the
x86 host or for the TI Tiva C (M4F) target.

The `EventEncoder` needs one callback for forwarding a byte buffer to the host.
The `eventgenapp` defines and uses an `IOutputChannel` abstraction which
supports two concurrent byte streams: system stream and event log stream. The
`ConsoleChannel` forwards to `stderr` via standard C IO (appropriate for the
x86 host), and `TivaUartChannel` forwards via one or two separate UART ports on
Tiva C. The configuration is in `eventgen.cfg`. The `eventstream` modules do
not depend on this abstraction, the `*Channel` modules are outside of the
`eventstream` package.  Nevertheless, these modules might be generic enough to
be re-used outside of this example application.

On Tiva C Launchpad, the UART port that is forwarded via the ICDI chip (the
`/dev/ttyACM0` one) is fixed at 115200 baud. For higher baud rate, a UART on
the target microcontroller needs to be used directly through a suitable
USB->serial (FTDI) device. Got events at 2000000 baud with a USB FTDI 3.3v
cable.
