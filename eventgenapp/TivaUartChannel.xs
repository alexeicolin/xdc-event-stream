function module$use()
{
    xdc.useModule("ti.drivers.UART");
    xdc.useModule("ti.sysbios.hal.Hwi");
}

function module$static$init(obj)
{
    obj.sysUartPort = null;
    obj.eventUartPort = null;
}

function module$validate()
{
    if (this.sysUartIndex == this.eventUartIndex &&
        this.sysUartBaudRate != this.eventUartBaudRate)
        this.$logError("System and event log UART ports are the same but " +
                       " baud rates are different.", this, "sysUartBaudRate");

    if (this.sysUartIndex < 0 || this.sysUartIndex > 1)
        this.$logError("Supported UART ports are at indices: [0, 1]", this,
                       "sysUartIndex");
    if (this.eventUartIndex < 0 || this.eventUartIndex > 1)
        this.$logError("Supported UART ports are at indices: [0, 1]", this,
                       "eventUartIndex");
}
