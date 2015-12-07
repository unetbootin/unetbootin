//! [0]
        (void) new Q3ListViewItem(listView, "Column 1", "Column 2");
        (void) new Q3ListViewItem(listView->firstChild(), "A", "B", "C");
//! [0]


//! [1]
        Q3ListViewItem * myChild = myItem->firstChild();
        while(myChild) {
            doSomething(myChild);
            myChild = myChild->nextSibling();
        }
//! [1]


//! [2]
        Q3ListViewItemIterator it(listview);
        while (it.current()) {
            Q3ListViewItem *item = it.current();
            doSomething(item);
            ++it;
        }
//! [2]


//! [3]
    int MyListViewItem::compare(Q3ListViewItem *i, int col,
                                 bool ascending) const
    {
        return key(col, ascending).compare(i->key(col, ascending));
    }
//! [3]


//! [4]
    Q3ListViewItem *i = itemAt(p);
    if (i) {
        if (p.x() > header()->sectionPos(header()->mapToIndex(0)) +
                treeStepSize() * (i->depth() + (rootIsDecorated() ? 1 : 0)) + itemMargin() ||
                p.x() < header()->sectionPos(header()->mapToIndex(0))) {
            ; // p is not on root decoration
        else
            ; // p is on the root decoration
    }
//! [4]


//! [5]
    QRect r(listView->itemRect(item));
    r.setHeight(qMin(item->totalHeight(),
                     listView->viewport->height() - r.y()))
//! [5]


//! [6]
    QList<Q3ListViewItem *> lst;
    Q3ListViewItemIterator it(myListView);
    while (it.current()) {
        if (it.current()->isSelected())
            lst.append(it.current());
        ++it;
    }
//! [6]


//! [7]
    QList<Q3ListViewItem *> lst;
    Q3ListViewItemIterator it(myListView, Selected);
    while (it.current()) {
        lst.append(it.current());
        ++it;
    }
//! [7]


