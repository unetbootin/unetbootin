//! [0]
    QDataWidgetMapper *mapper = new QDataWidgetMapper;
    mapper->setModel(model);
    mapper->addMapping(mySpinBox, 0);
    mapper->addMapping(myLineEdit, 1);
    mapper->addMapping(myCountryChooser, 2);
    mapper->toFirst();
//! [0]


//! [1]
    QDataWidgetMapper *mapper = new QDataWidgetMapper();
    mapper->setModel(myModel);
    mapper->addMapping(nameLineEdit, 0);
    mapper->addMapping(ageSpinBox, 1);
//! [1]


//! [2]
    QDataWidgetMapper *mapper = new QDataWidgetMapper(); 
    connect(myTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));
//! [2]


