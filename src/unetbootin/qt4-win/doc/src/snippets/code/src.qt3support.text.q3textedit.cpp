//! [0]
    QFile file(fileName); // Read the text from a file
    if (file.open(IO_ReadOnly)) {
        QTextStream stream(&file);
        textEdit->setText(stream.read());
    }

    QFile file(fileName); // Write the text to a file
    if (file.open(IO_WriteOnly)) {
        QTextStream stream(&file);
        stream << textEdit->text();
        textEdit->setModified(false);
    }
//! [0]


//! [1]
    This is <font color=red>red</font> while <b>this</b> is <font color=blue>blue</font>.
    <font color=green><font color=yellow>Yellow,</font> and <u>green</u>.
//! [1]


//! [2]
    Q3TextEdit * log = new Q3TextEdit(this);
    log->setTextFormat(Qt::LogText);
    Q3StyleSheetItem * item = new Q3StyleSheetItem(log->styleSheet(), "mytag");
    item->setColor("red");
    item->setFontWeight(QFont::Bold);
    item->setFontUnderline(true);
    log->append("This is a <mytag>custom tag</mytag>!");
//! [2]


