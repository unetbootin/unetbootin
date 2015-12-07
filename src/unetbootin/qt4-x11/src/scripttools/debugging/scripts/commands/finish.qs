name = "finish";

group = "running";

shortDescription = "Execute until the current stack frame returns";

longDescription = "Upon return, the value returned is printed.";

seeAlso = [ "next", "continue" ];

function execute() {
    scheduleStepOut();
};

function handleResponse(resp) {
}
