name = "break";

group = "breakpoints";

shortDescription = "Set a breakpoint at specified location";

longDescription = "break <file>:<line> : Sets a breakpoint at the given location.";
longDescription += "\nbreak <line> : Sets a breakpoint at the given line of the current file.";

argumentTypes = [ "script-filename" ];

aliases = [ "b" ];

seeAlso = [ "condition", "delete", "disable", "tbreak" ];

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
        scheduleSetBreakpoint({ fileName: fileName, lineNumber: lineNumber});
        state = 2;
    }
}

function handleResponse(resp) {
    if (state == 1) {
        fileName = resp.result.fileName;
        if (fileName.length == 0)
            fileName = "<anonymous script, id=" + scriptId + ">";
        scheduleSetBreakpoint({ scriptId: scriptId, lineNumber: lineNumber});
        state = 2;
    } else if (state == 2) {
        if (resp.error == 0) {
            var id = resp.result;
            message("Breakpoint " + id + ": " + fileName + ", line " + lineNumber + ".");
        }
    }
}
