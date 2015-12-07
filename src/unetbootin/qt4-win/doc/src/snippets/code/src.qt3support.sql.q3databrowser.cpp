//! [0]
    QStringList list = myDataBrowser.sort();
    QStringList::Iterator it = list.begin();
    while(it != list.end()) {
        myProcessing(*it);
        ++it;
    }
//! [0]


