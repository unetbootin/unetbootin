//! [0]
        QFont serifFont("Times", 10, QFont::Bold);
        QFont sansFont("Helvetica [Cronyx]", 12);
//! [0]


//! [1]
        QFont f("Helvetica");
//! [1]


//! [2]
        QFont f("Helvetica [Cronyx]");
//! [2]


//! [3]
        QFontInfo info(f1);
        QString family = info.family();
//! [3]


//! [4]
        QFontMetrics fm(f1);
        int textWidthInPixels = fm.width("How many pixels wide is this text?");
        int textHeightInPixels = fm.height();
//! [4]


