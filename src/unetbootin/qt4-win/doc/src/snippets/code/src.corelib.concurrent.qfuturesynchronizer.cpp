//! [0]
    void someFunction()
    {
        QFutureSynchronizer<void> synchronizer;
        
        ...
        
        synchronizer.addFuture(QtConcurrent::run(anotherFunction));
        synchronizer.addFuture(QtConcurrent::map(list, mapFunction));
        
        return; // QFutureSynchronizer waits for all futures to finish
    }
//! [0]


