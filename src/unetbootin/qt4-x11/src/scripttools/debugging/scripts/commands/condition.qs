name = "condition";

group = "breakpoints";

shortDescription = "Specify breakpoint condition";

longDescription = "condition <breakpoint-id> <expression> : Specify that the breakpoint with the given id should only be triggered if the given expression evaluates to true.";

argumentTypes = [ "breakpoint-id", "script" ];

seeAlso = [ "ignore" ];

function execute() {
    if (arguments.length == 0) {
        message("Missing arguments (breakpoint number and condition).");
        return;
    }
    var arg = arguments[0];
    var spaceIndex = arg.indexOf(' ');
    if (spaceIndex == -1)
        spaceIndex = arg.length;
    var id = parseInt(arg.slice(0, spaceIndex));
    if (isNaN(id)) {
        message("First argument must be a number (breakpoint id).");
        return;
    }
    var cond = arg.slice(spaceIndex+1);
    if ((cond.length != 0) && !checkSyntax(cond)) {
        message("The condition has a syntax error.");
        return;
    }
    scheduleGetBreakpointData(id);
    breakpointId = id;
    condition = cond;
    state = 1;
}

function handleResponse(resp) {
    if (state == 1) {
        var data = resp.result;
        if (data == undefined) {
            message("No breakpoint number " + breakpointId + ".");
            return;
        }
        data.condition = condition;
        scheduleSetBreakpointData(breakpointId, data);
        state = 2;
    } else if (state == 2) {
        if (condition.length == 0)
            message("Breakpoint " + breakpointId + " now unconditional.");
    }
}
