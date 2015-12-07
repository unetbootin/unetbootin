//! [0]
        QGraphicsScene scene;
        QGraphicsWidget *textEdit = scene.addWidget(new QTextEdit);
        QGraphicsWidget *pushButton = scene.addWidget(new QPushButton);

        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
        layout->addItem(textEdit, 0, 0);
        layout->addItem(pushButton, 0, 1);

        QGraphicsWidget *form = new QGraphicsWidget;
        form->setLayout(layout);
        scene.addItem(form);
//! [0]


