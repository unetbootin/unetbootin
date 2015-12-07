name = "list";

group = "files";

shortDescription = "List lines of a script";

longDescription = "list <file>:<line> : Lists lines around the given location.";
longDescription += "\nlist <line> : Lists lines in the current file.";

argumentTypes = [ "script-filename" ];

listLineNumber = 1;
listScriptId = -1;
lastSessionId = -1;
lastFrameIndex = -1;

function execute() {
    state = 0;
    if (arguments.length > 0) {
        var arg = arguments[0];
        var colonIndex = arg.lastIndexOf(':');
        var fileName;
        var lineNumber;
        if (colonIndex == -1) {
            lineNumber = parseInt(arg);
            if (isNaN(lineNumber)) {
                fileName = arg;
                lineNumber = 1;
            }
        } else if (colonIndex == 0) {
            fileName = arg;
            lineNumber = 1;
        } else {
            fileName = arg.slice(0, colonIndex);
            lineNumber = parseInt(arg.slice(colonIndex+1));
        }
        listLineNumber = Math.max(lineNumber, 1);
        if (fileName != undefined) {
            scheduleResolveScript(fileName);
            state = 1;
        } else {
            setCurrentLineNumber(listLineNumber);
            execute();
        }
    } else {
        if ((getSessionId() != lastSessionId)
            || (getCurrentFrameIndex() != lastFrameIndex)
            || (listScriptId == -1)) {
            listScriptId = getCurrentScriptId();
            listLineNumber = getCurrentLineNumber();
            lastSessionId = getSessionId();
            lastFrameIndex = getCurrentFrameIndex();
        }
        scheduleGetScriptData(listScriptId);
        state = 2;
    }
};

function handleResponse(resp) {
    if (state == 1) {
        var id = resp.result;
        if (id == -1) {
            message("That script isn't loaded.");
            state = 0;
            return;
        }
        listScriptId = id;
        scheduleGetScriptData(listScriptId);
        state = 2;
    } else if (state == 2) {
        var data = resp.result;
        if (data == undefined) {
            message("No script.");
            state = 0;
            return;
        }
        var base = data.baseLineNumber;
        var lines = data.contents.split('\n');
        var start = Math.max(listLineNumber - 5, base);
        for (var i = start; i < start + 10; ++i) {
            var ln = lines[i - base];
            var msg = String(i);
            if (ln != undefined)
                msg += "\t" + ln;
            message(msg);
        }
        listLineNumber += 10;
        state = 0;
    }
}
