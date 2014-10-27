package eventstream;

module NumFormat {

     /*  ======== NumFormat_format ========
     *  Format singed or unsigned 64-bit number in specified base,
     *  and return a pointer to the output string in a private buffer.
     *
     *  Note: base is negative if n is signed else n unsigned!
     *
     *  un   - The unsigned number to be formated
     *  base - The base to format the number into. TODO - signed?
     *
     * Adapted from System_fomatNum to handle 64-bit *
     */
    Char *format(UInt64 un, Int zpad, Int base);
};
