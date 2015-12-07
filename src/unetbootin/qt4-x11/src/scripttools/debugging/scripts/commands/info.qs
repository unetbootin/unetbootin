name = "info";

group = "status";

shortDescription = "Display information about something";

longDescription = "info scripts : Names of scripts being debugged";
longDescription += "\ninfo breakpoints : Status of breakpoints currently set";
longDescription += "\ninfo locals : Local variables of current stack frame";

argumentTypes = [ "subcommand-name" ];

subCommands = [ "breakpoints", "locals", "scripts" ];

function execute() {
    var arg = arguments[0];
    if (arg == undefined) {
        message("\"info\" must be followed by the name of an info command.");
        return;
    } else if (arg == "scripts") {
        scheduleGetScripts();
        state = 1;
    } else if (arg == "breakpoints") {
        if (arguments.length > 1) {
            var id = parseInt(arguments[1]);
            if (isNaN(id)) {
                message("Breakpoint id expected.");
                return;
            }
            scheduleGetBreakpointData(id);
            breakpointId = id;
            state = 3;
        } else {
            scheduleGetBreakpoints();
            state = 2;
        }
    } else if (arg == "locals") {
        scheduleGetActivationObject(getCurrentFrameIndex());
        state = 4;
    } else {
        warning("Undefined info command \"" + arg + "\". Try \"help info\".");
    }
}

function breakpointString(id, data) {
    var fn = data.fileName;
    if (fn.length == 0)
        fn = "<anonymous script, id=" + data.scriptId + ">";
    var ret = id + "\t" + (data.enabled ? "yes" : "no")
        + "\t" + fn + ":" + data.lineNumber;
    if (data.condition.length != 0) {
        ret += "\n\tstop only if " + data.condition;
    }
    return ret;
}

function handleResponse(resp) {
    if (state == 1) {
        // info scripts
        var scripts = resp.result;
        if (scripts == undefined) {
            message("No scripts loaded.");
            return;
        }
        for (var id in scripts) {
            var fn = scripts[id].fileName;
            if (fn.length == 0)
                fn = "<anonymous script, id=" + id + ">";
            message("\t" + fn);
        }
    }

    else if (state == 2) {
        // info breakpoints
        var breakpoints = resp.result;
        if (breakpoints == undefined) {
            message("No breakpoints set.");
            return;
        }
        message("Id\tEnabled\tWhere");
        for (var id in breakpoints) {
            var data = breakpoints[id];
            message(breakpointString(id, data));
        }
    } else if (state == 3) {
        // info breakpoints N
        var data = resp.result;
        if (data == undefined) {
            message("No breakpoint number " + breakpointId + ".");
            return;
        }
        message("Id\tEnabled\tWhere");
        message(breakpointString(breakpointId, data));
    }

    else if (state == 4) {
        // info locals
        var act = resp.result;
        scheduleNewScriptValueIterator(act);
        state = 5;
    } else if (state == 5) {
        var id = resp.result;
        scheduleGetPropertiesByIterator(id, 100);
        iteratorId = id;
        state = 6;
    } else if (state == 6) {
        var props = resp.result;
        if (props.length == 0) {
            scheduleDeleteScriptValueIterator(iteratorId);
            state = 7;
            return;
        }
        var maxLength = 0;
        for (var i = 0; i < props.length; ++i)
            maxLength = Math.max(props[i].name.length, maxLength);
        for (var i = 0; i < props.length; ++i) {
            var prop = props[i]; 
            var msg = prop.name;
            var pad = maxLength - prop.name.length;
            for (var j = 0; j < pad; ++j)
                msg += ' ';
            message(msg + " : " + prop.valueAsString);
        }
        scheduleGetPropertiesByIterator(iteratorId, 100);
    } else if (state == 7) {
        // done
    }
}
