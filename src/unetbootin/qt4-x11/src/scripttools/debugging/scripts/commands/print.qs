// ### exactly the same as eval, but provided for convenience

name = "print";

group = "status";

shortDescription = "Print value of an expression";

longDescription = "";

argumentTypes = [ "script" ];

function execute() {
    if (arguments.length == 0) {
        message("Missing argument (expression).");
        return;
    }
    setEvaluateAction(0);
    scheduleEvaluate(getCurrentFrameIndex(), arguments[0], "console input (" + Date() + ")");
};

function handleResponse(resp, id) {
}
