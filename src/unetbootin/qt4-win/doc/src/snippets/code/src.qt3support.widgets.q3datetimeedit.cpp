//! [0]
    Q3DateEdit *dateEdit = new Q3DateEdit(QDate::currentDate(), this);
    dateEdit->setRange(QDate::currentDate().addDays(-365),
                        QDate::currentDate().addDays( 365));
    dateEdit->setOrder(Q3DateEdit::MDY);
    dateEdit->setAutoAdvance(true);
//! [0]


//! [1]
    QTime timeNow = QTime::currentTime();
    Q3TimeEdit *timeEdit = new Q3TimeEdit(timeNow, this);
    timeEdit->setRange(timeNow, timeNow.addSecs(60 * 60));
//! [1]


//! [2]
    Q3DateTimeEdit *dateTimeEdit = new Q3DateTimeEdit(QDateTime::currentDateTime(), this);
    dateTimeEdit->dateEdit()->setRange(QDateTime::currentDate(),
                                        QDateTime::currentDate().addDays(7));
//! [2]


