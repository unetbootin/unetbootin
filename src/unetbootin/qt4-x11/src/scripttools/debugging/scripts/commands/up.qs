name = "up";

group = "stack";

shortDescription = "Select and print the stack frame above the current one";

longDescription = "";

seeAlso = [ "down", "frame" ];

function execute() {
    scheduleGetContextCount();
    state = 1;
}

function handleResponse(resp) {
    if (state == 1) {
        var count = resp.result;
        var idx = getCurrentFrameIndex() + 1;
        if (idx == count) {
            warning("Already at top (outermost) frame.");
            return;
        }
        setCurrentFrameIndex(idx);
        scheduleGetContextInfo(idx);
        state = 2;
    } else if (state == 2) {
        var info = resp.result;
        setCurrentScriptId(info.scriptId);
        setCurrentLineNumber(info.lineNumber);
        scheduleGetBacktrace();
        state = 3;
    } else if (state == 3) {
        var backtrace = resp.result;
        message("#" + getCurrentFrameIndex() + "  " + backtrace[getCurrentFrameIndex()]);
    }
}
