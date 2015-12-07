//! [0]
        Q3ScrollView* sv = new Q3ScrollView(...);
        QWidget *widget = new QWidget(sv->viewport());
        QVBoxLayout *layout = new QVBoxLayout(widget);
        addChild(widget);
//! [0]


//! [1]
        QLabel* child1 = new QLabel("CHILD", widget);
        QLabel* child2 = new QLabel("CHILD", widget);
        QLabel* child3 = new QLabel("CHILD", widget);
        layout->addWidget(child1);
        layout->addWidget(child2);
        layout->addWidget(child3);
        ...
//! [1]


//! [2]
        Q3ScrollView* sv = new Q3ScrollView(...);
        QLabel* child1 = new QLabel("CHILD", sv->viewport());
        sv->addChild(child1);
        QLabel* child2 = new QLabel("CHILD", sv->viewport());
        sv->addChild(child2);
        QLabel* child3 = new QLabel("CHILD", sv->viewport());
        sv->addChild(child3);
//! [2]


//! [3]
        Q3ScrollView* sv = new Q3ScrollView(...);
        sv->enableClipper(true);
        QLabel* child1 = new QLabel("CHILD", sv->viewport());
        sv->addChild(child1);
        QLabel* child2 = new QLabel("CHILD", sv->viewport());
        sv->addChild(child2);
        QLabel* child3 = new QLabel("CHILD", sv->viewport());
        sv->addChild(child3);
//! [3]


//! [4]
    {
        // Fill a 40000 by 50000 rectangle at (100000,150000)

        // Calculate the coordinates...
        int x1 = 100000, y1 = 150000;
        int x2 = x1+40000-1, y2 = y1+50000-1;

        // Clip the coordinates so X/Windows will not have problems...
        if (x1 < clipx) x1=clipx;
        if (y1 < clipy) y1=clipy;
        if (x2 > clipx+clipw-1) x2=clipx+clipw-1;
        if (y2 > clipy+cliph-1) y2=clipy+cliph-1;

        // Paint using the small coordinates...
        if (x2 >= x1 && y2 >= y1)
            p->fillRect(x1, y1, x2-x1+1, y2-y1+1, red);
    }
//! [4]


