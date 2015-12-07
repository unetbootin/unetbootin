//! [0]
    QStringList list = myIndex.toStringList();
    QStringList::Iterator it = list.begin();
    while(it != list.end()) {
        myProcessing(*it);
        ++it;
    }
//! [0]


