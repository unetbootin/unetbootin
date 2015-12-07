//! [0]
        QDesignerPropertyEditorInterface *propertyEditor = 0;
        propertyEditor = formEditor->propertyEditor();

        connect(propertyEditor, SIGNAL(propertyChanged(QString, QVariant)),
                this, SLOT(checkProperty(QString, QVariant)));
//! [0]


//! [1]
        void checkProperty(QString property, QVariant value) {
            QDesignerPropertyEditorInterface *propertyEditor = 0;
            propertyEditor = formEditor->propertyEditor();

            QObject *object = propertyeditor->object();
            MyCustomWidget *widget = qobject_cast<MyCustomWidget>(object);

            if (widget && property == aProperty && value != expectedValue)
                {...}
        }
//! [1]


