name = "frame";

group = "stack";

shortDescription = "Select and print a stack frame";

longDescription = "";

aliases = [ "f" ];

function execute() {
    if (arguments.length == 0)
        requestedFrameIndex = getCurrentFrameIndex();
    else
        requestedFrameIndex = parseInt(arguments[0]);
    scheduleGetContextInfo(requestedFrameIndex);
    state = 1;
};

function handleResponse(resp, id) {
    if (state == 1) {
        var info = resp.result;
        if (info == undefined) {
            message("Frame index out of range.");
            return;
        }
        setCurrentFrameIndex(requestedFrameIndex);
        setCurrentScriptId(info.scriptId);
        setCurrentLineNumber(info.lineNumber);
        scheduleGetBacktrace();
        state = 2;
    } else if (state == 2) {
        var backtrace = resp.result;
        message("#" + getCurrentFrameIndex() + "  " + backtrace[getCurrentFrameIndex()]);
    }
}
