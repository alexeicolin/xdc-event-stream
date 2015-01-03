function module$use()
{
    var System = xdc.useModule('xdc.runtime.System');
    System.atexitMeta(this.onAppExit);
}
