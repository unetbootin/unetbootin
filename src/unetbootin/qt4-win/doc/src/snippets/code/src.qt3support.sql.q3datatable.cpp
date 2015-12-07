//! [0]
    QStringList list = myDataTable.sort();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
//! [0]


