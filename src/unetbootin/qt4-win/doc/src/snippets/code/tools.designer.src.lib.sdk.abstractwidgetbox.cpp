//! [0]
        QDesignerWidgetBoxInterface *widgetBox = 0:
        widgetBox = formEditor->widgetBox();

        widgetBox->load();
//! [0]


//! [1]
        QString originalFile = widgetBox->fileName();

        widgetBox->setFileName("myWidgetBox.xml");
        widgetBox->save();
//! [1]


//! [2]
        widgetBox->setFileName(originalFile);
        widgetBox->load();
//! [2]


//! [3]
        if (widgetBox->filename() != "myWidgetBox.xml") {
            widgetBox->setFileName("myWidgetBox.xml");
            widgetBox->load();
        }
//! [3]


