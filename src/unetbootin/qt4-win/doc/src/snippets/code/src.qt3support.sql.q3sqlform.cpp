//! [0]
    QLineEdit  myEditor(this);
    Q3SqlForm   myForm(this);
    Q3SqlCursor myCursor("mytable");

    // Execute a query to make the cursor valid
    myCursor.select();
    // Move the cursor to a valid record (the first record)
    myCursor.next();
    // Set the form's record pointer to the cursor's edit buffer (which
    // contains the current record's values)
    myForm.setRecord(myCursor.primeUpdate());

    // Insert a field into the form that uses myEditor to edit the
    // field 'somefield' in 'mytable'
    myForm.insert(&myEditor, "somefield");

    // Update myEditor with the value from the mapped database field
    myForm.readFields();
    ...
    // Let the user edit the form
    ...
    // Update the database
    myForm.writeFields();  // Update the cursor's edit buffer from the form
    myCursor.update();        // Update the database from the cursor's buffer
//! [0]


