//! [0]
        void LineEdit::contextMenuEvent(QContextMenuEvent *event)
        {
            QMenu *menu = createStandardContextMenu();
            menu->addAction(tr("My Menu Item"));
            //...
            menu->exec(event->globalPos());
            delete menu;
        }
//! [0]


