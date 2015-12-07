//! [0]
        Q3ComboBox *c = new Q3ComboBox(this, tr("read-only combobox"));
//! [0]


//! [1]
        Q3ComboBox *c1 = new Q3ComboBox(false, this, tr("read-only combobox"));
        Q3ComboBox *c2 = new Q3ComboBox(true, this, tr("editable combobox"));
//! [1]


//! [2]
	static const char* items[] = { "red", "green", "blue", 0 };
	combo->insertStrList( items );
//! [2]


