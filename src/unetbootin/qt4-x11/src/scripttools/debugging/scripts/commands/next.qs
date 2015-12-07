name = "next";

group = "running";

shortDescription = "Step program, proceeding through subroutine calls";

longDescription = "Like the \"step\" command as long as subroutine calls do not happen;";
longDescription += "\nwhen they do, the call is treated as one instruction.";
longDescription += "\nIf a number N is given as argument, this will be done N times before execution is stopped.";
aliases = [ "n" ];

seeAlso = [ "step", "continue", "finish", "advance" ];

function execute() {
    var count = 1;
    if (arguments.length != 0) {
        var arg = arguments[0];
        // ### evaluate the expression in the current frame?
        var num = parseInt(arg);
        if (!isNaN(num) && (num >= 1))
            count = num;
    }
    scheduleStepOver(count);
};

function handleResponse(resp) {
}
