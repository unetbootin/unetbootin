//! [0]
    (void) new Q3IconViewItem(
                    iconView,        // A pointer to a Q3IconView
                    "This is the text of the item",
                    aPixmap);
//! [0]


//! [1]
    Q3IconViewItem *item;
    for (item = iconView->firstItem(); item; item = item->nextItem())
        do_something_with(item);
//! [1]


//! [2]
    Q3IconViewItem *item;
    for (item = iconView->firstItem(); item; item = item->nextItem())
        do_something_with(item);
//! [2]


//! [3]
        int MyIconViewItem::compare(Q3IconViewItem *i) const
        {
            return key().compare(i->key());
        }
//! [3]


//! [4]
    Q3IconView *iv = new Q3IconView(this);
    QDir dir(path, "*.xpm");
    for (uint i = 0; i < dir.count(); i++) {
        (void) new Q3IconViewItem(iv, dir[i], QPixmap(path + dir[i]));
    }
    iv->resize(600, 400);
    iv->show();
//! [4]


//! [5]
        for (Q3IconViewItem *item = iv->firstItem(); item; item = item->nextItem())
            do_something(item);
//! [5]


//! [6]
    Q3DragObject *MyIconView::dragObject()
    {
        return new Q3TextDrag(currentItem()->text(), this);
    }
//! [6]


//! [7]
    bool MyIconViewItem::acceptDrop(const QMimeSource *mime) const
    {
        if (mime->provides("text/plain"))
            return true;
        return false;
    }

    void MyIconViewItem::dropped(QDropEvent *evt, const Q3ValueList<Q3IconDragItem>&)
    {
        QString label;
        if (Q3TextDrag::decode(evt, label))
            setText(label);
    }
//! [7]


//! [8]
        (void) new Q3IconViewItem(myIconview, "The text of the item", aPixmap);
//! [8]


