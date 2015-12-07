//! [0]
      exec(QCursor::pos());
//! [0]


//! [1]
      exec(somewidget.mapToGlobal(QPoint(0,0)));
//! [1]


//! [2]
      exec(e->globalPos());
//! [2]


//! [3]
        exec(QCursor::pos());
//! [3]


//! [4]
        exec(somewidget.mapToGlobal(QPoint(0, 0)));
//! [4]


//! [5]
      exec(e->globalPos());
//! [5]


//! [6]
       QMenu menu;
       QAction *at = actions[0]; // Assumes actions is not empty
       foreach (QAction *a, actions)
          menu.addAction(a);
       menu.exec(pos, at);
//! [6]


