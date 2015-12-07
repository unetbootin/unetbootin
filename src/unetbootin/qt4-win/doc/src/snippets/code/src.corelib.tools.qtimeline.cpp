//! [0]
        ...
        progressBar = new QProgressBar(this);
        progressBar->setRange(0, 100);

        // Construct a 1-second timeline with a frame range of 0 - 100
        QTimeLine *timeLine = new QTimeLine(1000, this);
        timeLine->setFrameRange(0, 100);
        connect(timeLine, SIGNAL(frameChanged(int)), progressBar, SLOT(setValue(int)));

        // Clicking the push button will start the progress bar animation
        pushButton = new QPushButton(tr("Start animation"), this);
        connect(pushButton, SIGNAL(clicked()), timeLine, SLOT(start()));
        ...
//! [0]


