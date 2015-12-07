//! [0]
    Q3NetworkProtocol::registerNetworkProtocol( "nntp", new Q3NetworkProtocolFactory<Nntp> );
//! [0]


//! [1]
    Q3Ftp *ftp = Q3NetworkProtocol::getNetworkProtocol( "ftp" );
//! [1]


