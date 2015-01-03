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
    if (this.outputFunc == null)
        this.$logError("Output function must be defined", this, "outputFunc");

    /* We support extracting the strings from the target binary for use by the
     * host-side decoder only when the linker places the strings into the
     * designated 'noload' section, which happens when !Text.isLoaded. */
    var Text = xdc.module('xdc.runtime.Text');
    if (this.binaryOutput && Text.isLoaded)
        this.$logError("Binary output only works if text is excluded from " +
            "linked binary, i.e. !Text.isLoaded", this, "binaryOutput");
}
