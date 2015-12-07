//! [0]
        MyForm::MyForm(QWidget *parent)
            : QWidget(parent)
        {
            QFormBuilder builder;
            QFile file(":/forms/myWidget.ui");
            file.open(QFile::ReadOnly);
            QWidget *myWidget = builder.load(&file, this);
            file.close();

            QVBoxLayout *layout = new QVBoxLayout;
            layout->addWidget(myWidget);
            setLayout(layout);
        }
//! [0]


//! [1]
    <!DOCTYPE RCC><RCC version="1.0">
    <qresource prefix="/forms">
       <file>mywidget.ui</file>
    </qresource>
    </RCC>
//! [1]


