name = "ignore";

group = "breakpoints";

shortDescription = "Set ignore-count of a breakpoint";

longDescription = "ignore <breakpoint-id> <count> : Ignores the breakpoint with the given id the next count times it is hit.";

seeAlso = [ "condition" ];

function execute() {
    if (arguments.length < 1) {
        message("Missing arguments (breakpoing number and ignore-count).");
        return;
    }
    if (arguments.length < 2) {
        message("Missing argument (ignore-count).");
        return;
    }
    var id = parseInt(arguments[0]);
    if (isNaN(id)) {
        message("First argument (breakpoint id) must be a number.");
        return;
    }
    var count = parseInt(arguments[1]);
    if (isNaN(count)) {
        message("Second argument (ignore-count) must be a number.");
        return;
    }
    scheduleGetBreakpointData(id);
    breakpointId = id;
    if (count < 0)
        count = 0;
    ignoreCount = count;
    state = 1;
}

function handleResponse(resp) {
    if (state == 1) {
        var data = resp.result;
        if (data == undefined) {
            message("No breakpoint number " + breakpointId + ".");
            return;
        }
        data.ignoreCount = ignoreCount;
        scheduleSetBreakpointData(breakpointId, data);
        state = 2;
    } else if (state == 2) {
        message("Breakpoint " + breakpointId + " will be ignored the next " + ignoreCount + " time(s).");
    }
}
