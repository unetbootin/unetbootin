name = "step";

group = "running";

shortDescription = "Step program until a new statement is reached";

longDescription = "If a number N is given as argument, this will be done N times before execution is stopped.";

aliases = [ "s" ];

seeAlso = [ "next" ];

function execute() {
    var count = 1;
    if (arguments.length != 0) {
        var arg = arguments[0];
        // ### evaluate the expression in the current frame?
        var num = parseInt(arg);
        if (!isNaN(num) && (num >= 1))
            count = num;
    }
    scheduleStepInto(count);
};

function handleResponse(resp) {
}
