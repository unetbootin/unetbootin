/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
***************************************************************************/

#include <QtGui>

#include "mainwindow.h"

//![0]
MainWindow::MainWindow()
{
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    metaInformationResolver = new Phonon::MediaObject(this);

    mediaObject->setTickInterval(1000);
//![0]
//![2]
    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            this, SLOT(stateChanged(Phonon::State, Phonon::State)));
    connect(metaInformationResolver, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(metaStateChanged(Phonon::State, Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
            this, SLOT(sourceChanged(const Phonon::MediaSource &)));
    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish()));
//![2]

//![1]
    Phonon::createPath(mediaObject, audioOutput);
//![1]

    setupActions();
    setupMenus();
    setupUi();
    timeLcd->display("00:00"); 
}

//![6]
void MainWindow::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"), 
        QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

    if (files.isEmpty())
        return;

    int index = sources.size();
    foreach (QString string, files) {
            Phonon::MediaSource source(string);
        
        sources.append(source);
    } 
    if (!sources.isEmpty())
        metaInformationResolver->setCurrentSource(sources.at(index));

}
//![6]

void MainWindow::about()
{
    QMessageBox::information(this, tr("About Music Player"),
        tr("The Music Player example shows how to use Phonon - the multimedia"
           " framework that comes with Qt - to create a simple music player."));
}

//![9]
void MainWindow::stateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    switch (newState) {
        case Phonon::ErrorState:
            if (mediaObject->errorType() == Phonon::FatalError) {
                QMessageBox::warning(this, tr("Fatal Error"),
                mediaObject->errorString());
            } else {
                QMessageBox::warning(this, tr("Error"),
                mediaObject->errorString());
            }
            break;
//![9]
//![10]
        case Phonon::PlayingState:
                playAction->setEnabled(false);
                pauseAction->setEnabled(true);
                stopAction->setEnabled(true);
                break;
        case Phonon::StoppedState:
                stopAction->setEnabled(false);
                playAction->setEnabled(true);
                pauseAction->setEnabled(false);
                timeLcd->display("00:00");
                break;
        case Phonon::PausedState:
                pauseAction->setEnabled(false);
                stopAction->setEnabled(true);
                playAction->setEnabled(true);
                break;
//![10]
        case Phonon::BufferingState:
                break;
        default:
            ;
    }
}

//![11]
void MainWindow::tick(qint64 time)
{
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);

    timeLcd->display(displayTime.toString("mm:ss"));
}
//![11]

//![12]
void MainWindow::tableClicked(int row, int /* column */)
{
    bool wasPlaying = mediaObject->state() == Phonon::PlayingState;

    mediaObject->stop();
    mediaObject->clearQueue();

    mediaObject->setCurrentSource(sources[row]);

    if (wasPlaying) 
        mediaObject->play();
    else
        mediaObject->stop();
}
//![12]

//![13]
void MainWindow::sourceChanged(const Phonon::MediaSource &source)
{
    musicTable->selectRow(sources.indexOf(source));
    timeLcd->display("00:00");
}
//![13]

//![14]
void MainWindow::metaStateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    if (newState == Phonon::ErrorState) {
        QMessageBox::warning(this, tr("Error opening files"),
            metaInformationResolver->errorString());
        while (!sources.isEmpty() &&
               !(sources.takeLast() == metaInformationResolver->currentSource()));
        return;
    }

    if (newState != Phonon::StoppedState && newState != Phonon::PausedState)
        return;

    if (metaInformationResolver->currentSource().type() == Phonon::MediaSource::Invalid)
            return;

    QMap<QString, QString> metaData = metaInformationResolver->metaData();

    QString title = metaData.value("TITLE");
    if (title == "")
        title = metaInformationResolver->currentSource().fileName();

    QTableWidgetItem *titleItem = new QTableWidgetItem(title);
    titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem *artistItem = new QTableWidgetItem(metaData.value("ARTIST"));
    artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem *albumItem = new QTableWidgetItem(metaData.value("ALBUM"));
    albumItem->setFlags(albumItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem *yearItem = new QTableWidgetItem(metaData.value("DATE"));
    yearItem->setFlags(yearItem->flags() ^ Qt::ItemIsEditable);
//![14]

    int currentRow = musicTable->rowCount();
    musicTable->insertRow(currentRow);
    musicTable->setItem(currentRow, 0, titleItem);
    musicTable->setItem(currentRow, 1, artistItem);
    musicTable->setItem(currentRow, 2, albumItem);
    musicTable->setItem(currentRow, 3, yearItem);

//![15]
    if (musicTable->selectedItems().isEmpty()) {
        musicTable->selectRow(0);
        mediaObject->setCurrentSource(metaInformationResolver->currentSource());
    }

    Phonon::MediaSource source = metaInformationResolver->currentSource();
    int index = sources.indexOf(metaInformationResolver->currentSource()) + 1;
    if (sources.size() > index) {
        metaInformationResolver->setCurrentSource(sources.at(index));
    }
    else {
        musicTable->resizeColumnsToContents();
        if (musicTable->columnWidth(0) > 300)
            musicTable->setColumnWidth(0, 300);
    }
}
//![15]

//![16]
void MainWindow::aboutToFinish()
{
    int index = sources.indexOf(mediaObject->currentSource()) + 1;
    if (sources.size() > index) {
        mediaObject->enqueue(sources.at(index));
    }
}
//![16]

void MainWindow::setupActions()
{
    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    playAction->setShortcut(tr("Crl+P"));
    playAction->setDisabled(true);
    pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    pauseAction->setShortcut(tr("Ctrl+A"));
    pauseAction->setDisabled(true);
    stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
    stopAction->setShortcut(tr("Ctrl+S"));
    stopAction->setDisabled(true);
    nextAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), tr("Next"), this);
    nextAction->setShortcut(tr("Ctrl+N"));
    previousAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), tr("Previous"), this);
    previousAction->setShortcut(tr("Ctrl+R"));
    addFilesAction = new QAction(tr("Add &Files"), this);
    addFilesAction->setShortcut(tr("Ctrl+F"));
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+X"));
    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setShortcut(tr("Ctrl+Q"));

//![5]
    connect(playAction, SIGNAL(triggered()), mediaObject, SLOT(play()));
    connect(pauseAction, SIGNAL(triggered()), mediaObject, SLOT(pause()) );
    connect(stopAction, SIGNAL(triggered()), mediaObject, SLOT(stop()));
//![5]
    connect(addFilesAction, SIGNAL(triggered()), this, SLOT(addFiles()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(addFilesAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
    aboutMenu->addAction(aboutQtAction);
}

//![3]
void MainWindow::setupUi()
{
//![3]
    QToolBar *bar = new QToolBar;

    bar->addAction(playAction);
    bar->addAction(pauseAction);
    bar->addAction(stopAction);
    
//![4]
    seekSlider = new Phonon::SeekSlider(this);
    seekSlider->setMediaObject(mediaObject);

    volumeSlider = new Phonon::VolumeSlider(this);
    volumeSlider->setAudioOutput(audioOutput);
//![4]
    volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QLabel *volumeLabel = new QLabel;
    volumeLabel->setPixmap(QPixmap("images/volume.png"));

    QPalette palette;
    palette.setBrush(QPalette::Light, Qt::darkGray);

    timeLcd = new QLCDNumber;
    timeLcd->setPalette(palette);

    QStringList headers;
    headers << tr("Title") << tr("Artist") << tr("Album") << tr("Year");

    musicTable = new QTableWidget(0, 4);
    musicTable->setHorizontalHeaderLabels(headers);
    musicTable->setSelectionMode(QAbstractItemView::SingleSelection);
    musicTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(musicTable, SIGNAL(cellPressed(int, int)),
            this, SLOT(tableClicked(int, int)));

    QHBoxLayout *seekerLayout = new QHBoxLayout;
    seekerLayout->addWidget(seekSlider);
    seekerLayout->addWidget(timeLcd);

    QHBoxLayout *playbackLayout = new QHBoxLayout;
    playbackLayout->addWidget(bar);
    playbackLayout->addStretch();
    playbackLayout->addWidget(volumeLabel);
    playbackLayout->addWidget(volumeSlider);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(musicTable);
    mainLayout->addLayout(seekerLayout);
    mainLayout->addLayout(playbackLayout);

    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);

    setCentralWidget(widget);
    setWindowTitle("Phonon Music Player");
}

