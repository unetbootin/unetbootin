name = "help";

group = "void";

shortDescription = "Print list of commands";

longDescription = "";

argumentTypes = [ "command-or-group-name" ];

function execute() {
    if (arguments.length == 0) {
        var groups = getCommandGroups();
        message("List of command categories:");
        message("");
        for (var name in groups) {
            if (name == "void")
                continue;
            var data = groups[name];
            message(name + " :: " + data.shortDescription);
        }
        message("");
        message("Type \"help\" followed by a category name for a list of commands in that category.");
        message("Type \"help all\" for the list of all commands.");
        message("Type \"help\" followed by a command name for full documentation.");
        message("Command name abbreviations are allowed if they are unambiguous.");
    } else {
        var arg = arguments[0];
        if (arg == "all") {
            var groups = getCommandGroups();
            for (var name in groups) {
                if (name == "void")
                    continue;
                message("Command category: " + name);
                message("");
                var commands = getCommandsInGroup(name);
                for (var i = 0; i < commands.length; ++i) {
                    var data = commands[i];
                    message(data.name + " :: " + data.shortDescription);
                }
                message("");
            }
        } else {
            var data = findCommand(arg);
            if (data != undefined) {
                message(data.shortDescription + ".");
                if (data.longDescription.length != 0)
                    message(data.longDescription);
                if (data.aliases.length != 0)
                    message("Aliases: " + data.aliases.join(", "));
                if (data.seeAlso.length != 0)
                    message("See also: " + data.seeAlso.join(", "));
            } else {
                data = getCommandGroups()[arg];
                if (data != undefined) {
                    message(data.shortDescription + ".");
                    message("");
                    message("List of commands:");
                    message("");
                    var commands = getCommandsInGroup(arg);
                    for (var i = 0; i < commands.length; ++i) {
                        var data = commands[i];
                        message(data.name + " :: " + data.shortDescription);
                    }
                } else {
                    message("Undefined command \"" + arg + "\". Try \"help\".");
                }
            }
        }
    }
};
