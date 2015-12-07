//! [0]
        QStringList wordList;
        wordList << "alpha" << "omega" << "omicron" << "zeta";

        QLineEdit *lineEdit = new QLineEdit(this);

        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        lineEdit->setCompleter(completer);
//! [0]


//! [1]
        QCompleter *completer = new QCompleter(this);
        completer->setModel(new QDirModel(completer));
        lineEdit->setCompleter(completer);
//! [1]


//! [2]
        for (int i = 0; completer->setCurrentRow(i); i++)
            qDebug() << completer->currentCompletion() << " is match number " << i;
//! [2]


