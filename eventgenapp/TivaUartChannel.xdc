package eventgenapp;

@ModuleStartup
module TivaUartChannel inherits IOutputChannel
{
    config UInt sysUartIndex = 0;
    config UInt32 sysUartBaudRate = 115200;

    // Can be the same as sys
    config UInt eventUartIndex = 0;
    config UInt32 eventUartBaudRate = 115200;

  internal:
    
    struct Module_State {
        // Note: UART_Handle is not an instance handle but a private typedef
        Void *sysUartPort;
        Void *eventUartPort;
    };
}
