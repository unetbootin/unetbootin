//! [0]
    QStringList list = myDns.qualifiedNames();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
        myProcessing( *it );
        ++it;
    }
//! [0]


//! [1]
    Q3ValueList<QHostAddress> list = myDns.addresses();
    Q3ValueList<QHostAddress>::Iterator it = list.begin();
    while( it != list.end() ) {
        myProcessing( *it );
        ++it;
    }
//! [1]


//! [2]
    Q3ValueList<Q3Dns::MailServer> list = myDns.mailServers();
    Q3ValueList<Q3Dns::MailServer>::Iterator it = list.begin();
    while( it != list.end() ) {
        myProcessing( *it );
        ++it;
    }
//! [2]


//! [3]
    Q3ValueList<Q3Dns::Server> list = myDns.servers();
    Q3ValueList<Q3Dns::Server>::Iterator it = list.begin();
    while( it != list.end() ) {
        myProcessing( *it );
        ++it;
    }
//! [3]


//! [4]
    QStringList list = myDns.hostNames();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
        myProcessing( *it );
        ++it;
    }
//! [4]


//! [5]
    QStringList list = myDns.texts();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
        myProcessing( *it );
        ++it;
    }
//! [5]


