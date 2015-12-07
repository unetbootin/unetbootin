//! [0]
            QStandardItemModel model(4, 4);
            for (int row = 0; row < 4; ++row) {
                for (int column = 0; column < 4; ++column) {
                    QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
                    model.setItem(row, column, item);
                }
            }
//! [0]


//! [1]
            QStandardItemModel model;
            QStandardItem *parentItem = model.invisibleRootItem();
            for (int i = 0; i < 4; ++i) {
                QStandardItem *item = new QStandardItem(QString("item %0").arg(i));
                parentItem->appendRow(item);
                parentItem = item;
            }
//! [1]


//! [2]
        QTreeView *treeView = new QTreeView(this);
        treeView->setModel(myStandardItemModel);
        connect(treeView, SIGNAL(clicked(QModelIndex)),
                this, SLOT(clicked(QModelIndex)));
//! [2]


//! [3]
        void MyWidget::clicked(const QModelIndex &index)
        {
            QStandardItem *item = myStandardItemModel->itemFromIndex(index);
            // Do stuff with the item ...
        }
//! [3]


//! [4]
        treeView->scrollTo(item->index());
//! [4]


