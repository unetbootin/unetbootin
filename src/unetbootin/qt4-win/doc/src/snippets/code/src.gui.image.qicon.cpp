//! [0]
    QToolButton *button = new QToolButton;
    button->setIcon(QIcon("open.xpm"));
//! [0]


//! [1]
   button->setIcon(QIcon());
//! [1]


//! [2]
    void MyWidget::drawIcon(QPainter *painter, QPoint pos)
    {
        QPixmap pixmap = icon.pixmap(QSize(22, 22),
                                       isEnabled() ? QIcon::Normal
                                                   : QIcon::Disabled,
                                       isOn() ? QIcon::On
                                              : QIcon::Off);
        painter->drawPixmap(pos, pixmap);
    }
//! [2]


