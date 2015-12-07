//! [0]
Q3ProgressDialog progress("Copying files...", "Abort Copy", numFiles,
                          this, "progress", true);
for (int i = 0; i < numFiles; i++) {
    progress.setProgress(i);
    qApp->processEvents();

    if (progress.wasCanceled())
        break;
    //... copy one file
}
progress.setProgress(numFiles);
//! [0]


//! [1]
Operation::Operation(QObject *parent = 0)
    : QObject(parent), steps(0)
{
    pd = new Q3ProgressDialog("Operation in progress.", "Cancel", 100);
    connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
    t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(perform()));
    t->start(0);
}

void Operation::perform()
{
    pd->setProgress(steps);
    //... perform one percent of the operation
    steps++;
    if (steps > pd->totalSteps())
        t->stop();
}

void Operation::cancel()
{
    t->stop();
    //... cleanup
}
//! [1]


