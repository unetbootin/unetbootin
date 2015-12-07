//! [0]
           QTextEdit *editor = new QTextEdit;
           MyHighlighter *highlighter = new MyHighlighter(editor->document());
//! [0]


//! [1]
        void MyHighlighter::highlightBlock(const QString &text)
        {
            QTextCharFormat myClassFormat;
            myClassFormat.setFontWeight(QFont::Bold);
            myClassFormat.setForeground(Qt::darkMagenta);
            QString pattern = "\\bMy[A-Za-z]+\\b";

            QRegExp expression(pattern);
            int index = text.indexOf(expression);
            while (index >= 0) {
                int length = expression.matchedLength();
                setFormat(index, length, myClassFormat);
                index = text.indexOf(expression, index + length);
             }
         }
//! [1]


//! [2]
        QTextCharFormat multiLineCommentFormat;
        multiLineCommentFormat.setForeground(Qt::red);

        QRegExp startExpression("/\\*");
        QRegExp endExpression("\\* /");

        setCurrentBlockState(0);

        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = text.indexOf(startExpression);

        while (startIndex >= 0) {
           int endIndex = text.indexOf(endExpression, startIndex);
           int commentLength;
           if (endIndex == -1) {
               setCurrentBlockState(1);
               commentLength = text.length() - startIndex;
           } else {
               commentLength = endIndex - startIndex
                               + endExpression.matchedLength();
           }
           setFormat(startIndex, commentLength, multiLineCommentFormat);
           startIndex = text.indexOf(startExpression,
                                     startIndex + commentLength);
        }
//! [2]


//! [3]
        void MyHighlighter::highlightBlock(const QString &text)
        {
            QTextCharFormat myClassFormat;
            myClassFormat.setFontWeight(QFont::Bold);
            myClassFormat.setForeground(Qt::darkMagenta);
            QString pattern = "\\bMy[A-Za-z]+\\b";

            QRegExp expression(pattern);
            int index = text.indexOf(expression);
            while (index >= 0) {
                int length = expression.matchedLength();
                setFormat(index, length, myClassFormat);
                index = text.indexOf(expression, index + length);
             }
         }
//! [3]


//! [4]
        struct ParenthesisInfo
        {
            QChar char;
            int position;
        };

        struct BlockData : public QTextBlockUserData
        {
            QVector<ParenthesisInfo> parentheses;
        };
//! [4]


