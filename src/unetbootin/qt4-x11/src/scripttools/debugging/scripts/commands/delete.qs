name = "delete";

group = "breakpoints";

shortDescription = "Delete breakpoint(s)";

longDescription = "delete <breakpoint-id> : Deletes the breakpoint with the given id.";

seeAlso = [ "clear", "disable" ];

function execute() {
    if (arguments.length == 0) {
        // delete all breakpoints
        scheduleClearBreakpoints();
        state = 1;
    } else {
        var id = parseInt(arguments[0]);
        if (isNaN(id)) {
            message("Breakpoint id expected.");
            return;
        }
        scheduleDeleteBreakpoint(id);
        breakpointId = id;
        state = 2;
    }
}

function handleResponse(resp) {
    if (state == 1) {
    } else if (state == 2) {
        if (resp.error != 0) {
            message("No breakpoint number " + breakpointId + ".");
            return;
        }
    }
}
