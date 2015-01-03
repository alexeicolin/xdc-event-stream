function getLibs(prog)
{
    var libs = [this.$name]; // all include the main archive

    // Logger adapters are in separate archives to avoid false dependencies

    // TODO: should be able to rid the consumer from the burden of specifying
    //       logger paramter-- is there a way to get list of modules used by
    //       the program?
    if (this.Settings.eventLogger == "LoggerCallback") {
        libs.push("logger_callback_adapter");
    } else if (this.Settings.eventLogger == "LoggerBuf") {
        libs.push("logger_buf_adapter");
    } else if (this.Settings.eventLogger == "LoggerIdle") {
        libs.push("logger_idle_adapter");
    }

    var suffix = prog.build.target.findSuffix(this);
    var lib_paths = [];
    for (var i = 0; i < libs.length;  ++i)
        lib_paths.push("lib/" + libs[i] + ".a" + suffix);

    return lib_paths.join(";");
}
