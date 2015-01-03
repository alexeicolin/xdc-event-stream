package eventgenapp;

interface IOutputChannel {
    Void outputStdOut(const Char *buf, Int size);
    Void outputEventLog(const Char *buf, Int size);
}
