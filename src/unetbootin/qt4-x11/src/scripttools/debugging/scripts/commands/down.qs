name = "down";

group = "stack";

shortDescription = "Select and print the stack frame below the current one";

longDescription = "";

seeAlso = [ "up", "frame" ];

function execute() {
    var idx = getCurrentFrameIndex();
    if (idx == 0) {
        warning("Already at bottom (innermost) frame.");
        return;
    }
    setCurrentFrameIndex(idx - 1);
    scheduleGetContextInfo(idx - 1);
    state = 1;
}

function handleResponse(resp, id) {
    if (state == 1) {
        var info = resp.result;
        setCurrentScriptId(info.scriptId);
        setCurrentLineNumber(info.lineNumber);
        scheduleGetBacktrace();
        state = 2;
    } else if (state == 2) {
        var backtrace = resp.result;
        message("#" + getCurrentFrameIndex() + "  " + backtrace[getCurrentFrameIndex()]);
    }
}
