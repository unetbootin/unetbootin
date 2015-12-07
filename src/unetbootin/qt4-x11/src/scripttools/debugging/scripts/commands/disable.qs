name = "disable";

group = "breakpoints";

shortDescription = "Disable breakpoint(s)";

longDescription = "disable <breakpoint-id> : Disables the breakpoint with the given id.";

seeAlso = [ "enable", "delete", "ignore" ];

function execute() {
    if (arguments.length == 0) {
        // disable all breakpoints
        state = 1;
        scheduleGetBreakpoints();
    } else {
        var id = parseInt(arguments[0]);
        if (isNaN(id)) {
            message("Breakpoint id expected.");
            return;
        }
        scheduleGetBreakpointData(id);
        breakpointId = id;
        state = 3;
    }
};

function handleResponse(resp) {
    if (state == 1) {
        var breakpoints = resp.result;
        if (breakpoints == undefined)
            return;
        for (var id in breakpoints) {
            var data = breakpoints[id];
            if (data.enabled) {
                data.enabled = false;
                scheduleSetBreakpointData(id, data);
            }
        }
        state = 2;
    } else if (state == 2) {
        state = 0;
    } else if (state == 3) {
        var data = resp.result;
        if (data == undefined) {
            message("No breakpoint number " + breakpointId + ".");
            return;
        } else if (data.enabled) {
            data.enabled = false;
            scheduleSetBreakpointData(breakpointId, data);
            state = 4;
        }
    } else if (state == 4) {
        state = 0;
    }
}
