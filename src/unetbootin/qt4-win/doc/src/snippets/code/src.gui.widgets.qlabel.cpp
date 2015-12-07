//! [0]
    QLabel *label = new QLabel(this);
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setText("first line\nsecond line");
    label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
//! [0]


//! [1]
    QLineEdit* phoneEdit = new QLineEdit(this);
    QLabel* phoneLabel = new QLabel("&Phone:", this);
    phoneLabel->setBuddy(phoneEdit);
//! [1]


//! [2]
    QLineEdit *nameEd  = new QLineEdit(this);
    QLabel    *nameLb  = new QLabel("&Name:", this);
    nameLb->setBuddy(nameEd);
    QLineEdit *phoneEd = new QLineEdit(this);
    QLabel    *phoneLb = new QLabel("&Phone:", this);
    phoneLb->setBuddy(phoneEd);
    // (layout setup not shown)
//! [2]


