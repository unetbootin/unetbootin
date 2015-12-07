//! [0]
    static uchar bindata[] = { 231, 1, 44, ... };
    QByteArray	a;
    a.setRawData(bindata, sizeof(bindata));	// a points to bindata
    QDataStream s(a, IO_ReadOnly);		// open on a's data
    s >> <something>;				// read raw bindata
    s.close();
    a.resetRawData(bindata, sizeof(bindata)); // finished
//! [0]


//! [1]
    static uchar bindata[] = { 231, 1, 44, ... };
    QByteArray	a, b;
    a.setRawData(bindata, sizeof(bindata));	// a points to bindata
    a.resize(8);				// will crash
    b = a;					// will crash
    a[2] = 123;					// might crash
      // forget to resetRawData - will crash
//! [1]


