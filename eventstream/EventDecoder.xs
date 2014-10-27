function module$use()
{
    xdc.useModule('eventstream.EventRecord');
    xdc.useModule('eventstream.Text');
}

function module$validate()
{
    if (this.readFunc == null) {
        this.$logError("Read function must be defined", this, "readFunc");
    }
}
