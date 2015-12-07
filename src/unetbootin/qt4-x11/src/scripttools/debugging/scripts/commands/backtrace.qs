name = "backtrace";

group = "stack";

shortDescription = "Print backtrace of stack frames";

longDescription = "";

aliases = [ "bt" ];

seeAlso = [ "frame", "info" ];

function execute() {
    scheduleGetBacktrace();
};

function handleResponse(resp) {
    var strings = resp.result;
    var msg = "";
    for (var i = 0; i < strings.length; ++i) {
        if (i > 0)
            msg += "\n";
        msg += "#" + i + "  " + strings[i];
    }
    message(msg);
}
