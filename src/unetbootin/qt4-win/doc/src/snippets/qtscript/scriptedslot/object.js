function Object(timer, editor)
{
    this.editor = editor;
    this.counter = 0;
    timer.timeout.connect(notify);
    timer.timeout.connect(this, this.addLine);
}

Object.prototype.addLine = function()
{
    this.editor.append(this.counter);
    this.counter += 1;
}

function notify()
{
    print("timeout() received");
}
