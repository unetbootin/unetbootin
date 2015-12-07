//! [0]
    QUrlOperator op( "ftp://ftp.trolltech.com" );
    op.listChildren(); // Asks the server to provide a directory listing
//! [0]


//! [1]
    Q3Ftp *ftp = new Q3Ftp( this ); // this is an optional QObject parent
    ftp->connectToHost( "ftp.trolltech.com" );
    ftp->login();
//! [1]


//! [2]
    ftp->connectToHost( "ftp.trolltech.com" );  // id == 1
    ftp->login();                               // id == 2
    ftp->cd( "qt" );                            // id == 3
    ftp->get( "INSTALL" );                      // id == 4
    ftp->close();                               // id == 5
//! [2]


//! [3]
    start( 1 )
    stateChanged( HostLookup )
    stateChanged( Connecting )
    stateChanged( Connected )
    finished( 1, false )

    start( 2 )
    stateChanged( LoggedIn )
    finished( 2, false )

    start( 3 )
    finished( 3, false )

    start( 4 )
    dataTransferProgress( 0, 3798 )
    dataTransferProgress( 2896, 3798 )
    readyRead()
    dataTransferProgress( 3798, 3798 )
    readyRead()
    finished( 4, false )

    start( 5 )
    stateChanged( Closing )
    stateChanged( Unconnected )
    finished( 5, false )

    done( false )
//! [3]


//! [4]
    start( 1 )
    stateChanged( HostLookup )
    stateChanged( Connecting )
    stateChanged( Connected )
    finished( 1, false )

    start( 2 )
    finished( 2, true )

    done( true )
//! [4]


