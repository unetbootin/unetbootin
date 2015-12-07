name = "advance";

group = "running";

shortDescription = "Continue the program up to the given location";

longDescription = "This command has the same syntax as the \"break\" command.";

seeAlso = [ "break", "tbreak" ];

argumentTypes = [ "script-filename" ];

function execute() {
    if (arguments.length == 0) {
        message("Missing argument(s).");
        return;
    }
    var arg = arguments[0];
    var colonIndex = arg.lastIndexOf(':');
    if (colonIndex == -1) {
        lineNumber = parseInt(arg);
        if (isNaN(lineNumber)) {
            message("Location must be of the form <file>:<line> or <line>.");
            return;
        }
        var sid = getCurrentScriptId();
        if (sid == -1) {
            message("No script.");
            return;
        }
        scheduleRunToLocation(sid, lineNumber);
    } else {
        fileName = arg.slice(0, colonIndex);
        lineNumber = parseInt(arg.slice(colonIndex+1));
        // ### resolve the script to see if it's loaded or not? (e.g. so we can issue a warning)
        scheduleRunToLocation(fileName, lineNumber);
    }
}

function handleResponse(resp) {
}
