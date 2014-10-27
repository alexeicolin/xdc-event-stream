function module$use()
{
    xdc.useModule('xdc.runtime.Text');
    xdc.useModule('eventstream.EventRecord');
    xdc.useModule('eventstream.NumFormat');
}

function module$meta$init()
{
    var Text = xdc.useModule('xdc.runtime.Text');
    this.binaryOutput = !Text.isLoaded;
}

function module$validate()
{
     if (this.outputFunc == null) {
        this.$logError("Output function must be defined", this, "outputFunc");
    }
}
