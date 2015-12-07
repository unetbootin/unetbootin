//! [0]
        QFuture<QString> future = ...;

        QFuture<QString>::const_iterator i;
        for (i = future.constBegin(); i != future.constEnd(); ++i)
            cout << *i << endl;
//! [0]


//! [1]
        QFuture<QString> future;
        ...
        QFutureIterator<QString> i(future);
        while (i.hasNext())
            qDebug() << i.next();
//! [1]


//! [2]
        QFutureIterator<QString> i(future);
        i.toBack();
        while (i.hasPrevious())
	    qDebug() << i.previous();
//! [2]


