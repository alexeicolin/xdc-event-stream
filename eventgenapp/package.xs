function getLibs(prog)
{
    var libs = [this.$name]; // all include the main archive

    if (prog.build.target.isa == "i686")
        libs.push("console_channel");

    if (prog.build.target.name == "M4F")
        libs.push("tiva_uart_channel");

    var suffix = prog.build.target.findSuffix(this);
    var lib_paths = [];
    for (var i = 0; i < libs.length;  ++i)
        lib_paths.push("lib/" + libs[i] + ".a" + suffix);

    return lib_paths.join(";");
}
