//! [0]
    Q3SqlPropertyMap *myMap  = new Q3SqlPropertyMap();
    Q3SqlForm        *myForm = new Q3SqlForm(this);
    MyEditor myEditor(this);

    // Set the Q3SqlForm's record buffer to the update buffer of
    // a pre-existing Q3SqlCursor called 'cur'.
    myForm->setRecord(cur->primeUpdate());

    // Install the customized map
    myMap->insert("MyEditor", "content");
    myForm->installPropertyMap(myMap); // myForm now owns myMap
    ...
    // Insert a field into the form that uses a myEditor to edit the
    // field 'somefield'
    myForm->insert(&myEditor, "somefield");

    // Update myEditor with the value from the mapped database field
    myForm->readFields();
    ...
    // Let the user edit the form
    ...
    // Update the database fields with the values in the form
    myForm->writeFields();
    ...
//! [0]


//! [1]
    Q3SqlPropertyMap *myMap = new Q3SqlPropertyMap;

    myMap->insert("MyEditor", "content");
    Q3SqlPropertyMap::installDefaultMap(myMap);
    ...
//! [1]


