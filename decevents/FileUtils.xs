
function readToArray(file) {
    var fileReader = new java.io.FileReader(file);
    var bufReader = new java.io.BufferedReader(fileReader);
    var r = [];
    var ch;
    while ((ch = bufReader.read()) >= 0) {
        r.push(ch);
    }
    return r;
}

