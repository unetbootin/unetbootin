//! [0]
    QItemSelection *selection = new QItemSelection(topLeft, bottomRight);
//! [0]


//! [1]
    QItemSelection *selection = new QItemSelection();
    ...
    selection->select(topLeft, bottomRight);
//! [1]


