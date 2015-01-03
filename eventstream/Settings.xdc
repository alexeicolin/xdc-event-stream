package eventstream;

// TODO: should be able to rid the consumer from the burden of specifying
//       logger paramter-- is there a way to get list of modules used by
//       the program in getLibs()?

metaonly module Settings {
    config String eventLogger = null;
}
