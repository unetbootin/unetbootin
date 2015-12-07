name = "interrupt";

group = "running";

shortDescription = "Interrupt evaluation";

longDescription = "Interruption will occur as soon as a new script statement is reached.";

function execute() {
    scheduleInterrupt();
}

function handleResponse(resp) {
}
