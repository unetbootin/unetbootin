//! [0]
        QPixmap* pp;
        QPixmap p;
        if ((pp=QPixmapCache::find("my_big_image", pm))) {
            p = *pp;
        } else {
            p.load("bigimage.png");
            QPixmapCache::insert("my_big_image", new QPixmap(p));
        }
        painter->drawPixmap(0, 0, p);
//! [0]


//! [1]
        QPixmap pm;
        if (!QPixmapCache::find("my_big_image", pm)) {
            pm.load("bigimage.png");
            QPixmapCache::insert("my_big_image", pm);
        }
        painter->drawPixmap(0, 0, pm);
//! [1]


