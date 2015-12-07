//! [0]
    // all 5 strings share the same data
    QString s1 = "abcd";
    QString s2 = s1;
    QString s3 = s2;
    QString s4 = s3;
    QString s5 = s2;
//! [0]


//! [1]
    // s1, s2 and s5 share the same data, neither s3 nor s4 are shared
    QString s1 = "abcd";
    QString s2 = s1;
    Q3DeepCopy<QString> s3 = s2;  // s3 is a deep copy of s2
    QString s4 = s3;             // s4 is a deep copy of s3
    QString s5 = s2;
//! [1]


//! [2]
    // s1, s2 and s5 share the same data, s3 and s4 share the same data
    QString s1 = "abcd";
    QString s2 = s1;
    QString s3 = Q3DeepCopy<QString>( s2 );  // s3 is a deep copy of s2
    QString s4 = s3;                        // s4 is a shallow copy of s3
    QString s5 = s2;
//! [2]


//! [3]
    Q3DeepCopy<QString> global_string;  // global string data
    QMutex global_mutex;               // mutex to protext global_string

    ...

    void setGlobalString( const QString &str )
    {
	global_mutex.lock();
	global_string = str;           // global_string is a deep copy of str
	global_mutex.unlock();
    }

    ...

    void MyThread::run()
    {
	global_mutex.lock();
	QString str = global_string;          // str is a deep copy of global_string
	global_mutex.unlock();

	// process the string data
	...

	// update global_string
	setGlobalString( str );
    }
//! [3]


