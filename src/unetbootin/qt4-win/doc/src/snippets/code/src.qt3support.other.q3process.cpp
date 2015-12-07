//! [0]
    QStringList list = myProcess.arguments();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
//! [0]


