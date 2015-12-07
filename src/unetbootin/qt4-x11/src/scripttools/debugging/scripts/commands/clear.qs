name = "clear";

group = "breakpoints";

shortDescription = "Clear breakpoint at specified location";

longDescription = "clear <file>:<line> : Clear breakpoints at the given location.";
longDescription += "\nclear <line> : Clear breakpoints at the given line of the current script.";

seeAlso = [ "delete" ];

argumentTypes = [ "script-filename" ];

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
        scriptId = sid;
    } else {
        fileName = arg.slice(0, colonIndex);
        lineNumber = parseInt(arg.slice(colonIndex+1));
    }
    scheduleGetBreakpoints();
    state = 1;
}

function handleResponse(resp) {
    if (state == 1) {
        var breakpoints = resp.result;
        if (breakpoints == undefined)
            return;
        for (var id in breakpoints) {
            var data = breakpoints[id];
            if ((data.lineNumber == lineNumber)
                && (data.fileName == fileName)
                    || ((data.scriptId != -1) && (data.scriptId = scriptId))) {
                scheduleDeleteBreakpoint(id);
                message("Deleted breakpoint " + id + ".");
            }
        }
        state = 2;
    } else if (state == 2) {

    }
}
