//! [0]
    view->page()->history();
//! [0]


//! [1]
    view->page()->settings();
//! [1]


//! [2]
    view->triggerAction(QWebPage::Copy);
//! [2]


//! [3]
    view->page()->triggerAction(QWebPage::Stop);
//! [3]


//! [4]
    view->page()->triggerAction(QWebPage::GoBack);
//! [4]


//! [5]
    view->page()->triggerAction(QWebPage::GoForward);
//! [5]


