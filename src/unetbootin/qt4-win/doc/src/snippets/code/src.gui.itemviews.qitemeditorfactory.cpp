//! [0]
        Q_PROPERTY(QColor color READ color WRITE setColor USER true)
//! [0]


//! [1]
    QItemEditorCreator<MyEditor> *itemCreator =
        new QItemEditorCreator<MyEditor>("myProperty");

    QItemEditorFactory *factory = new QItemEditorFactory;
//! [1]


//! [2]
    QItemEditorFactory *editorFactory = new QItemEditorFactory;
    QItemEditorCreatorBase *creator = new QStandardItemEditorCreator<MyFancyDateTimeEdit>();
    editorFactory->registerEditor(QVariant::DateType, creator);
//! [2]


//! [3]
	Q_PROPERTY(QColor color READ color WRITE setColor USER true)	
//! [3]


