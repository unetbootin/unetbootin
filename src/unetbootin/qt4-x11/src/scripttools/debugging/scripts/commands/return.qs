name = "return";

group = "running";

shortDescription = "Make selected stack frame return to its caller";

longDescription = "";

argumentTypes = [ "script" ];

function execute() {
    // TODO:
    // 1. schedule evaluate of the expression.
    // 2. install event handler/filter, so that we're notified when the evaluate is done.
    //    - what if another event occurs while we evaluate? (e.g. an exception or breakpoint)
    //    - the event filter needs to uninstall itself, or the event needs to be consumed internally
    // 3. in the event handler, schedule forced return with the result as argument.
    setEvaluateAction(1);
    scheduleEvaluate(getCurrentFrameIndex(), arguments[0], "console input (" + Date() + ")");
};
