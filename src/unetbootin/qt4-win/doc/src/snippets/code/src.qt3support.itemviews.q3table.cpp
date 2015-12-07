//! [0]
    for (int row = 0; row < table->numRows(); row++) {
        for (int col = 0; col < table->numCols(); col++) {
            table->setItem(row, col,
                new Q3TableItem(table, Q3TableItem::WhenCurrent, QString::number(row * col)));
        }
    }
//! [0]


//! [1]
    QWidget* MyTableItem::createEditor() const
    {
        QHBox* hbox = new QHBox(table()->viewport());
        hbox->setFocusProxy(new QLineEdit(hbox));
        new QLineEdit(hbox);
        return hbox;
    }
//! [1]


//! [2]
    p->setClipRect(table()->cellRect(row, col), QPainter::ClipPainter);
    //... your drawing code
    p->setClipping(false);
//! [2]


//! [3]
    Q3Table *table = new Q3Table(100, 250, this);
    table->setPixmap(3, 2, pix);
    table->setText(3, 2, "A pixmap");
//! [3]


//! [4]
    p->setClipRect(cellRect(row, col), QPainter::CoordPainter);
    //... your drawing code
    p->setClipping(false);
//! [4]


//! [5]
    Q3TableItem *i = item(row, col);
    if (initFromCell || (i && !i->isReplaceable()))
        // If we had a Q3TableItem ask the base class to create the editor
        return Q3Table::createEditor(row, col, initFromCell);
    else
        return ...(create your own editor)
//! [5]


