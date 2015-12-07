//! [0]
        QWidget *widget = splitter->widget(index);
        QSizePolicy policy = widget->sizePolicy();
        policy.setHorizontalStretch(stretch);
        policy.setVerticalStretch(stretch);
        widget->setSizePolicy(policy);
//! [0]


