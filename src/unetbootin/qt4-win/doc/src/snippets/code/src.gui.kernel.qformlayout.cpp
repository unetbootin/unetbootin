//! [0]
        QFormLayout *formLayout = new QFormLayout;
        formLayout->addRow(tr("&Name:"), nameLineEdit);
        formLayout->addRow(tr("&Email:"), emailLineEdit);
        formLayout->addRow(tr("&Age:"), ageSpinBox);
        setLayout(formLayout);
//! [0]


//! [1]
        nameLabel = new QLabel(tr("&Name:"));
        nameLabel->setBuddy(nameLineEdit);

        emailLabel = new QLabel(tr("&Name:"));
        emailLabel->setBuddy(emailLineEdit);

        ageLabel = new QLabel(tr("&Name:"));
        ageLabel->setBuddy(ageSpinBox);

        QGridLayout *gridLayout = new QGridLayout;
        gridLayout->addWidget(nameLabel, 0, 0);
        gridLayout->addWidget(nameLineEdit, 0, 1);
        gridLayout->addWidget(emailLabel, 1, 0);
        gridLayout->addWidget(emailLineEdit, 1, 1);
        gridLayout->addWidget(ageLabel, 2, 0);
        gridLayout->addWidget(ageSpinBox, 2, 1);
        setLayout(gridLayout);
//! [1]


//! [2]
        formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
        formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
        formLayout->setLabelAlignment(Qt::AlignLeft);
//! [2]


