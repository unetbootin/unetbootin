name = "eval";

group = "running";

shortDescription = "Evaluate program";

longDescription = "";

argumentTypes = [ "script" ];

function execute() {
    if (arguments.length == 0) {
        message("Missing argument (program).");
        return;
    }
    setEvaluateAction(0);
    scheduleEvaluate(getCurrentFrameIndex(), arguments[0], "console input (" + Date() + ")");
};

function handleResponse(resp, id) {
}
