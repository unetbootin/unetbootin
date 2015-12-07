//! [0]
    QSvgRenderer *renderer = new QSvgRenderer(QLatin1String("SvgCardDeck.svg"));
    QGraphicsSvgItem *black = new QGraphicsSvgItem();
    QGraphicsSvgItem *red   = new QGraphicsSvgItem();

    black->setSharedRenderer(renderer);
    black->setElementId(QLatin1String("black_joker"));

    red->setSharedRenderer(renderer);
    red->setElementId(QLatin1String("red_joker"));
//! [0]


