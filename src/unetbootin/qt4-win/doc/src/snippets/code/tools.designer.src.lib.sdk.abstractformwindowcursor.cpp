//! [0]
    QDesignerFormWindowInterface *formWindow = 0;
    formWindow = QDesignerFormWindowInterface::findFormWindow(myWidget);

    formWindow->cursor()->setProperty(myWidget, myProperty, newValue);
//! [0]


