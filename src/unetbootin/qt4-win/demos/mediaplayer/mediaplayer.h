/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef MEDIALAYER_H
#define MEDIAPLAYER_H

#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <QtCore/QTimerEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QIcon>
#include <QtCore/QHash>

#include <phonon>
QT_BEGIN_NAMESPACE
class QThread;
class QPushButton;
class QLabel;
class QSlider;
class QTextEdit;
class QMenu;
class Ui_settings;
QT_END_NAMESPACE

class MediaPlayer :
            public QWidget
{
    Q_OBJECT
public:
    MediaPlayer(const QString &);
public slots:
    void openFile();
    void pause();
    void play();
    void rewind();
    void forward();
    void setVolume(int);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
    void handleDrop(QDropEvent *e);
    void setFile(const QString &text);
    void initVideoWindow();
    void initSettingsDialog();
    void updateEffect();
    void updateInfo();
    void updateTime();
    void finished();
    void scaleChanged(QAction *);
    void aspectChanged(QAction *);
    void playPause();

private slots:
    void setAspect(int);
    void setScale(int);
    void setSaturation(int);
    void setContrast(int);
    void setHue(int);
    void setBrightness(int);
    void stateChanged(Phonon::State newstate, Phonon::State oldstate);
    void effectChanged();
    void showSettingsDialog();
    void showContextMenu(const QPoint &);
    void bufferStatus(int percent);
    void openUrl();
    void configureEffect();

private:
    QIcon playIcon;
    QIcon pauseIcon;
    QPixmap volumeIcon;
    QPixmap mutedIcon;
    QMenu *fileMenu;
    QPushButton *playButton;
    QPushButton *rewindButton;
    QPushButton *forwardButton;
    Phonon::SeekSlider *slider;
    QLabel *volumeLabel;
    QLabel *timeLabel;
    QLabel *progressLabel;
    QSlider *volume;
    QSlider *m_hueSlider;
    QSlider *m_satSlider;
    QSlider *m_contSlider;
    QLabel *info;
    long duration;
    QHash <QString, QWidget*> propertyControllers;
    Phonon::Effect *nextEffect;
    QDialog *settingsDialog;
    Ui_settings *ui;
        
    QWidget m_videoWindow;
    Phonon::MediaObject m_MediaObject;
    Phonon::AudioOutput m_AudioOutput;
    Phonon::VideoWidget *m_videoWidget;
    Phonon::Path m_audioOutputPath;
};

#endif //MEDIAPLAYER_H
