//! [0]
    Q3Url url( "http://www.trolltech.com" );
    // or
    Q3Url url( "file:///home/myself/Mail", "Inbox" );
//! [0]


//! [1]
    Q3Url url( "http://www.trolltech.com" );
    QString s = url;
    // or
    QString s( "http://www.trolltech.com" );
    Q3Url url( s );
//! [1]


//! [2]
    Q3Url url( "ftp://ftp.trolltech.com/qt/source", "qt-2.1.0.tar.gz" );
//! [2]


//! [3]
    Q3Url url( "ftp://ftp.trolltech.com/qt/source", "/usr/local" );
//! [3]


//! [4]
    Q3Url url( "ftp://ftp.trolltech.com/qt/source", "file:///usr/local" );
//! [4]


//! [5]
	QString url = http://www.trolltech.com
	Q3Url::encode( url );
	// url is now "http%3A//www%20trolltech%20com"
//! [5]


//! [6]
	QString url = "http%3A//www%20trolltech%20com"
	Q3Url::decode( url );
	// url is now "http://www.trolltech.com"
//! [6]


