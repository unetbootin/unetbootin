name = "continue";

group = "running";

shortDescription = "Continue evaluation";

longDescription = "Evaluation will continue until an uncaught exception occurs, "
longDescription += "a breakpoint is hit or evaluation is explicitly interrupted.";

aliases = [ "c", "fg" ];

seeAlso = [ "step", "interrupt" ];

function execute() {
    scheduleContinue();
};

function handleResponse(resp) {
    if (!resp.async) {
        message("The target is not evaluating code.");
    }
}
