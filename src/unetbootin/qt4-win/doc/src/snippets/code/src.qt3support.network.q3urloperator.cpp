//! [0]
    Q3UrlOperator *op = new Q3UrlOperator();
    op->copy( QString("ftp://ftp.trolltech.com/qt/source/qt-2.1.0.tar.gz"),
	     "file:///tmp" );
//! [0]


//! [1]
    Q3UrlOperator op( "http://www.whatever.org/cgi-bin/search.pl?cmd=Hello" );
    op.get();
//! [1]


//! [2]
    Q3UrlOperator op( "ftp://ftp.whatever.org/pub" );
    // do some other stuff like op.listChildren() or op.mkdir( "new_dir" )
    op.get( "a_file.txt" );
//! [2]


//! [3]
    Q3UrlOperator op( "http://www.whatever.org/cgi-bin" );
    op.get( "search.pl?cmd=Hello" ); // WRONG!
//! [3]


//! [4]
    Q3UrlOperator op( "ftp://ftp.whatever.com/home/me/filename.dat" );
    op.put( data );
//! [4]


//! [5]
    Q3UrlOperator op( "ftp://ftp.whatever.com/home/me" );
    // do some other stuff like op.listChildren() or op.mkdir( "new_dir" )
    op.put( data, "filename.dat" );
//! [5]


