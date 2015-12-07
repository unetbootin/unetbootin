//! [0]
        QtSpinBoxFactory *factory;
        QSet<QtIntPropertyManager *> managers = factory->propertyManagers();
//! [0]


//! [1]
        QtBrowserItem *item;
        QList<QtBrowserItem *> childrenItems = item->children();

        QList<QtProperty *> childrenProperties = item->property()->subProperties();
//! [1]


//! [2]
        QtProperty *property1, *property2, *property3;

        property2->addSubProperty(property1);
        property3->addSubProperty(property2);

        QtAbstractPropertyBrowser *editor;

        editor->addProperty(property1);
        editor->addProperty(property2);
        editor->addProperty(property3);
//! [2]


//! [3]
        QtIntPropertyManager *intManager;
        QtDoublePropertyManager *doubleManager;

        QtProperty *myInteger = intManager->addProperty();
        QtProperty *myDouble = doubleManager->addProperty();

        QtSpinBoxFactory  *spinBoxFactory;
        QtDoubleSpinBoxFactory *doubleSpinBoxFactory;

        QtAbstractPropertyBrowser *editor;
        editor->setFactoryForManager(intManager, spinBoxFactory);
        editor->setFactoryForManager(doubleManager, doubleSpinBoxFactory);

        editor->addProperty(myInteger);
        editor->addProperty(myDouble);
//! [3]


