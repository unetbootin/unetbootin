//! [0]
        QByteArray encodedString = "...";
        QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
        QString string = codec->toUnicode(encodedString);
//! [0]


//! [1]
        QString string = "...";
        QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
        QByteArray encodedString = codec->fromUnicode(string);
//! [1]


//! [2]
        QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");
        QTextDecoder *decoder = codec->makeDecoder();

        QString string;
        while (new_data_available()) {
            QByteArray chunk = get_new_data();
            string += decoder->toUnicode(chunk);
        }
//! [2]


//! [3]
        int main(int argc, char *argv[])
        {
            QApplication app(argc, argv);
            QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
            ...
        }
//! [3]


