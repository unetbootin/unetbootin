//! [0]
    QFont font("times", 24);
    QFontMetrics fm(font);
    int pixelsWide = fm.width("What's the width of this text?");
    int pixelsHigh = fm.height();
//! [0]


//! [1]
    QFont font("times", 24);
    QFontMetricsF fm(font);
    qreal pixelsWide = fm.width("What's the width of this text?");
    qreal pixelsHigh = fm.height();
//! [1]


