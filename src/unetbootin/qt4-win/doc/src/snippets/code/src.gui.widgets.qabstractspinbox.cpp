//! [0]
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setRange(0, 100);
        spinBox->setWrapping(true);
        spinBox->setValue(100);
        spinBox->stepBy(1);
        // value is 0
//! [0]


