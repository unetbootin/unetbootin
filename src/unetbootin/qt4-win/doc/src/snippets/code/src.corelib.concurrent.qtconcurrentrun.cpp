//! [0]
    extern void aFunction();
    QFuture<void> future = QtConcurrent::run(aFunction);
//! [0]


//! [1]
    extern void aFunctionWithArguments(int arg1, double arg2, const QString &string);

    int integer = ...;
    double floatingPoint = ...;
    QString string = ...;

    QFuture<void> future = QtConcurrent::run(aFunctionWithArguments, integer, floatingPoint, string);
//! [1]


//! [2]
    extern QString functionReturningAString();
    QFuture<QString> future = QtConcurrent::run(functionReturningAString);
    ...
    QString result = future.result();
//! [2]


//! [3]
    extern QString someFunction(const QByteArray &input);
    
    QByteArray bytearray = ...;
    
    QFuture<QString> future = QtConcurrent::run(someFunction, bytearray);
    ...
    QString result = future.result();
//! [3]


//! [4]
    // call 'QStringList QString::split(const QString &sep, SplitBehavior behavior, Qt::CaseSensitivity cs) const' in a separate thread
    QString string = ...;
    QFuture<QStringList> future = QtConcurrent::run(string, &QString::split, QString(", "), QString::KeepEmptyParts, Qt::CaseSensitive);
    ...
    QStringList result = future.result();
//! [4]


//! [5]
    // call 'void QImage::invertPixels(InvertMode mode)' in a separate thread
    QImage image = ...;
    QFuture<void> future = QtConcurrent::run(image, &QImage::invertPixels, QImage::InvertRgba);
    ...
    future.waitForFinished();
    // At this point, the pixels in 'image' have been inverted
//! [5]


//! [6]
    void someFunction(int arg1, double arg2);
    QFuture<void> future = QtConcurrent::run(boost::bind(someFunction, 1, 2.0));
    ...
//! [6]


