//! [0]
        void MyGroupBoxWidget::initStyleOption(QStyleOption *option) const
        {
            QGraphicsWidget::initStyleOption(option);
            if (QStyleOptionGroupBox *box = qstyleoption_cast<QStyleOptionGroupBox *>(option)) {
                // Add group box specific state.
                box->flat = isFlat();
                ...
            }
        }
//! [0]


//! [1]
        setTabOrder(a, b); // a to b
        setTabOrder(b, c); // a to b to c
        setTabOrder(c, d); // a to b to c to d
//! [1]


//! [2]
        // WRONG
        setTabOrder(c, d); // c to d
        setTabOrder(a, b); // a to b AND c to d
        setTabOrder(b, c); // a to b to c, but not c to d
//! [2]


