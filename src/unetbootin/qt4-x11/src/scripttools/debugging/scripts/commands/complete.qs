name = "complete";

group = "void";

shortDescription = "List the completions for the rest of the line as a command";

longDescription = "";

function execute() {
    var prefix = (arguments.length > 0) ? arguments[0] : "";
    var completions = getCommandCompletions(prefix);
    for (var i = 0; i < completions.length; ++i)
        message(completions[i]);
}
