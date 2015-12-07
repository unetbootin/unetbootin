name = "tbreak";

group = "breakpoints";

shortDescription = "Set a temporary breakpoint";

longDescription = "The same as the \"break\" command, except that the breakpoint is automatically deleted as soon as it is triggered.";

seeAlso = [ "break", "ignore" ];

argumentTypes = [ "script-filename" ];

// ### merge with break.qs: only difference is the "singleShot: true" in call to scheduleSetBreakpoint()
// ### maybe an include() function so commands can share code?

function execute() {
    if (arguments.length == 0) {
        message("Missing argument.");
        return;
    }
    var arg = arguments[0];
    var colonIndex = arg.lastIndexOf(':');
    if (colonIndex == -1) {
        lineNumber = parseInt(arg);
        if (isNaN(lineNumber)) {
            message("Breakpoint location must be of the form <file>:<line> or <line>.");
            return;
        }
        var sid = getCurrentScriptId();
        if (sid == -1) {
            message("No script.");
            return;
        }
        scheduleGetScriptData(sid);
        scriptId = sid;
        state = 1;
    } else {
        fileName = arg.slice(0, colonIndex);
        lineNumber = parseInt(arg.slice(colonIndex+1));
        // ### resolve the script to see if it's loaded or not? (e.g. so we can issue a warning)
        scheduleSetBreakpoint({ fileName: fileName, lineNumber: lineNumber, singleShot: true });
        state = 2;
    }
}

function handleResponse(resp) {
    if (state == 1) {
        fileName = resp.result.fileName;
        if (fileName.length == 0)
            fileName = "<anonymous script, id=" + scriptId + ">";
        scheduleSetBreakpoint({ scriptId: scriptId, lineNumber: lineNumber, singleShot: true });
        state = 2;
    } else if (state == 2) {
        if (resp.error == 0) {
            var id = resp.result;
            message("Breakpoint " + id + ": " + fileName + ", line " + lineNumber + ".");
        }
    }
}
