//! [0]
    ...
    Q3SqlSelectCursor* cur = new Q3SqlSelectCursor("SELECT id, firstname, lastname FROM author");
    Q3DataTable* table = new Q3DataTable(this);
    table->setSqlCursor(cur, true, true);
    table->refresh();
    ...
    cur->exec("SELECT * FROM books");
    table->refresh();
    ...
//! [0]


