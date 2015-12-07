//! [0]
    // Instantiate the objects and connect to the finished signal.
    MyClass myObject;
    QFutureWatcher<int> watcher;
    connect(&watcher, SIGNAL(finished()), &myObject, SLOT(handleFinished()));

    // Start the computation.
    QFuture<int> future = QtConcurrent::run(...);
    watcher.setFuture(future);
//! [0]


