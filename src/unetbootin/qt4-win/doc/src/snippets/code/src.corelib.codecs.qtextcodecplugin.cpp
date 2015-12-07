//! [0]
        QList<QByteArray> MyCodecPlugin::names() const
        {
            return QList<QByteArray> << "IBM01140" << "hp15-tw";
        }

        QTextCodec *MyCodecPlugin::createForName(const QByteArray &name)
        {
            if (name == "IBM01140") {
                return new Ibm01140Codec;
            } else if (name == "hp15-tw") {
                return new Hp15TwCodec;
            }
            return 0;
        }
//! [0]


