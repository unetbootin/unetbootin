/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvfb.h"
#include "qvfbview.h"
#ifdef Q_WS_X11
#include "qvfbx11view.h"
#endif
#include "qvfbratedlg.h"
#include "ui_config.h"
#include "qanimationwriter.h"

#include <deviceskin.h>

#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QSlider>
#include <QSpinBox>
#include <QLayout>
#include <QRadioButton>
#include <QImage>
#include <QPixmap>
#include <QCheckBox>
#include <QCursor>
#include <QTime>
#include <QScrollArea>
#include <QProgressBar>
#include <QPushButton>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

QT_BEGIN_NAMESPACE

// =====================================================================

static const char *red_on_led_xpm[] = {
"11 11 10 1",
" 	c None",
".	c #FF0000",
"+	c #FF4C4C",
"@	c #FF7A7A",
"#	c #D30000",
"$	c #FF9393",
"%	c #BA0000",
"&	c #FFFFFF",
"*	c #7F0000",
"=	c #000000",
"           ",
"   .++@@   ",
"  .....+@  ",
" ##...$.+@ ",
" %#..$&$.+ ",
" *#...$..+ ",
" *%#...... ",
" =*%#..... ",
"  =*%###.  ",
"   ===*.   ",
"           "};

static const char *red_off_led_xpm[] = {
"11 11 12 1",
" 	c None",
".	c #CDB7B4",
"+	c #D2BFBD",
"@	c #DBCBCA",
"#	c #E5D9D8",
"$	c #BC9E9B",
"%	c #E2D6D5",
"&	c #AD8986",
"*	c #FFFFFF",
"=	c #A8817D",
"-	c #B2908D",
";	c #6F4D4A",
"           ",
"   .++@#   ",
"  .....@#  ",
" $$...%.@# ",
" &$..%*%.@ ",
" =-...%..+ ",
" =&-...... ",
" ;==-..... ",
"  ;=&-$$.  ",
"   ;==&$   ",
"           "};

static bool copyButtonConfiguration(const QString &prefix, int displayId)
{
    const QString destDir = QString(QLatin1String("/tmp/qtembedded-%1/")).arg(displayId);
    const QFileInfo src(prefix + QLatin1String("defaultbuttons.conf"));
    const QFileInfo dst(destDir + QLatin1String("defaultbuttons.conf"));
    unlink(dst.absoluteFilePath().toLatin1().constData());
    if (!src.exists())
        return false;
    const bool rc = QFile::copy(src.absoluteFilePath(), dst.absoluteFilePath());
    if (!rc)
        qWarning() << "Failed to copy the button configuration file " << src.absoluteFilePath() << " to " <<  dst.absoluteFilePath() << '.';
    return rc;
}

// =====================================================================

class AnimationSaveWidget : public QWidget {
    Q_OBJECT
public:
    AnimationSaveWidget(QVFbAbstractView *v);
    ~AnimationSaveWidget();
    bool detectPpmtoMpegCommand();
    void timerEvent(QTimerEvent *te);
    void convertToMpeg(QString filename);
    void removeTemporaryFiles();
protected slots:
    void toggleRecord();
    void reset();
    void save();
private:
    QVFbAbstractView *view;
    QProgressBar *progressBar;
    QLabel *statusText;
    bool haveMpeg, savingAsMpeg, recording;
    QCheckBox *mpegSave;
    QAnimationWriter *animation;
    QPushButton *recBt, *resetBt, *saveBt;
    QLabel *timeDpy, *recLED;
    int timerId, progressTimerId;
    QPixmap recOn, recOff;
    QTime tm;
    int elapsed, imageNum;
};

// =====================================================================

Zoomer::Zoomer(QVFb* target) :
    qvfb(target)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QSlider *sl = new QSlider(Qt::Horizontal);
    sl->setMinimum(10);
    sl->setMaximum(64);
    sl->setPageStep(1);
    sl->setValue(32);
    layout->addWidget(sl);
    connect(sl,SIGNAL(valueChanged(int)),this,SLOT(zoom(int)));
    label = new QLabel();
    layout->addWidget(label);
}

void Zoomer::zoom(int z)
{
    double d = (double)z/32.0;
    qvfb->setZoom(d);
    label->setText(QString::number(d,'g',2));
}

// =====================================================================

QVFb::QVFb( int display_id, int w, int h, int d, int r, const QString &skin, DisplayType displayType, QWidget *parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags )
{
    this->displayType = displayType;
    view = 0;
    secondaryView = 0;
    scroller = 0;
    this->skin = 0;
    currentSkinIndex = -1;
    findSkins(skin);
    zoomer = 0;
    QPixmap pix(":/res/images/logo.png");
    setWindowIcon( pix );
    rateDlg = 0;
    refreshRate = 30;
#if QT_VERSION >= 0x030000
    // When compiling with Qt 3 we need to create the menu first to
    // avoid scroll bars in the main window
    createMenu( menuBar() );
    init( display_id, w, h, d, r, skin );
    enableCursor( true );
#else
    init( display_id, w, h, d, r, skin );
    createMenu( menuBar() );
#endif
}

QVFb::~QVFb()
{
}

void QVFb::popupMenu()
{
    QMenu *pm = new QMenu( this );
    createMenu( pm );
    pm->exec(QCursor::pos());
}

void QVFb::init( int display_id, int pw, int ph, int d, int r, const QString& skin_name )
{
    delete view;
    view = 0;
    delete secondaryView;
    secondaryView = 0;
    delete scroller;
    scroller = 0;
    delete skin;
    skin = 0;

    skinscaleH = skinscaleV = 1.0;
    QVFbView::Rotation rot = ((r ==  90) ? QVFbView::Rot90  :
			     ((r == 180) ? QVFbView::Rot180 :
			     ((r == 270) ? QVFbView::Rot270 :
					   QVFbView::Rot0 )));
    if ( !skin_name.isEmpty() ) {
	const bool vis = isVisible();
	DeviceSkinParameters parameters;
	QString readError;
	if (parameters.read(skin_name,DeviceSkinParameters::ReadAll, &readError)) {
	    skin = new DeviceSkin(parameters, this);
	    connect(skin, SIGNAL(popupMenu()), this, SLOT(popupMenu()));
	    const int sw = parameters.screenSize().width();
	    const int sh = parameters.screenSize().height();
	    const int sd = parameters.screenDepth;
            if (!pw) pw = sw;
            if (!ph) ph = sh;
            if (d < 0) {
                if (sd)
                    d = sd;
                else
                    d = -d;
            }
            if (vis)
                hide();
    	    menuBar()->hide();
	    scroller = 0;
#ifdef Q_WS_X11
	    if (displayType == X11)
		view = new QVFbX11View( display_id, pw, ph, d, rot, skin );
	    else
#endif
		view = new QVFbView( display_id, pw, ph, d, rot, skin );
	    skin->setView( view );
	    view->setContentsMargins( 0, 0, 0, 0 );
            view->setTouchscreenEmulation(!parameters.hasMouseHover);
	    connect(skin, SIGNAL(skinKeyPressEvent(int,QString,bool)), view, SLOT(skinKeyPressEvent(int,QString,bool)));
	    connect(skin, SIGNAL(skinKeyReleaseEvent(int,QString,bool)), view, SLOT(skinKeyReleaseEvent(int,QString,bool)));

	    copyButtonConfiguration(skin->prefix(), view->displayId());

	    setCentralWidget( skin );
	    adjustSize();
	    skinscaleH = (double)sw/pw;
	    skinscaleV = (double)sh/ph;
	    if ( skinscaleH != 1.0 || skinscaleH != 1.0 )
		setZoom(skinscaleH);
	    view->show();

            if (parameters.hasSecondaryScreen()) {
                const QSize ssize = parameters.secondaryScreenSize();
                // assumes same depth and rotation
#ifdef Q_WS_X11
		if (displayType == X11)
		    secondaryView = new QVFbX11View( display_id+1, ssize.width(), ssize.height(), d, rot, skin );
		else
#endif
		    secondaryView = new QVFbView( display_id+1, ssize.width(), ssize.height(), d, rot, skin );
                skin->setSecondaryView(secondaryView);
                secondaryView->show();
            }

	    if ( vis ) show();
	} else {
	    qWarning("%s", qPrintable(readError));
	}
    }

    // If we failed to get a skin or we were not supplied
    //	    with one then fallback to a framebuffer without
    //	    a skin
    if (!skin){
	// Default values
	if (!pw)
            pw = 240;
	if (!ph)
            ph = 320;
        if (!d)
            d = 32;
        else if (d < 0)
            d = -d;

     	if (currentSkinIndex != -1) {
	    clearMask();
            setParent( 0, 0 );
            move( pos() );
            show();
	    //unset fixed size:
	    setMinimumSize(0,0);
	    setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
	}
	menuBar()->show();
	scroller = new QScrollArea(this);
	scroller->setFocusPolicy(Qt::NoFocus); // don't steal key events from the embedded app
#ifdef Q_WS_X11
	if (displayType == X11)
	    view = new QVFbX11View( display_id, pw, ph, d, rot, scroller );
	else
#endif
	    view = new QVFbView( display_id, pw, ph, d, rot, scroller );
	scroller->setWidget(view);
	view->setContentsMargins( 0, 0, 0, 0 );
	setCentralWidget(scroller);
#if QT_VERSION >= 0x030000
	ph += 2;					// avoid scrollbar
#endif
	scroller->show();
	// delete defaultbuttons.conf if it was left behind...
	unlink(QFileInfo(QString("/tmp/qtembedded-%1/defaultbuttons.conf").arg(view->displayId())).absoluteFilePath().toLatin1().constData());
        if (secondaryView)
            unlink(QFileInfo(QString("/tmp/qtembedded-%1/defaultbuttons.conf").arg(view->displayId()+1)).absoluteFilePath().toLatin1().constData());
    }
    view->setRate(refreshRate);
    if (secondaryView) {
        secondaryView->setRate(refreshRate);
    }
    // Resize QVFb to the new size
    QSize newSize = view->sizeHint();

    // ... fudge factor
    newSize += QSize(20, 35);

    resize(newSize);

    setWindowTitle(QString("Virtual framebuffer %1x%2 %3bpp Display :%4 Rotate %5")
               .arg(view->displayWidth()).arg(view->displayHeight())
               .arg(d).arg(display_id).arg(r));
}

void QVFb::enableCursor( bool e )
{
    if ( skin && skin->hasCursor() ) {
	view->setCursor( Qt::BlankCursor );
        if (secondaryView)
            secondaryView->setCursor( Qt::BlankCursor );
    } else {
	view->setCursor( e ? Qt::ArrowCursor : Qt::BlankCursor );
        if (secondaryView)
            secondaryView->setCursor( e ? Qt::ArrowCursor : Qt::BlankCursor );
    }
    cursorAction->setChecked( e );
}

template <typename T>
void QVFb::createMenu(T *menu)
{
    menu->addMenu( createFileMenu() );
    menu->addMenu( createViewMenu() );
    menu->addSeparator();
    menu->addMenu( createHelpMenu() );
}

QMenu* QVFb::createFileMenu()
{
    QMenu *file = new QMenu( "File", this );
    file->addAction( "Configure...", this, SLOT(configure()), 0 );
    file->addSeparator();
    file->addAction( "&Save image...", this, SLOT(saveImage()), 0 );
    file->addAction( "&Animation...", this, SLOT(toggleAnimation()), 0 );
    file->addSeparator();
    file->addAction( "&Quit", qApp, SLOT(quit()) );
    return file;
}

QMenu* QVFb::createViewMenu()
{
    viewMenu = new QMenu( "View", this );
    cursorAction = viewMenu->addAction( "Show &Cursor", this,
                                        SLOT(toggleCursor()) );
    cursorAction->setCheckable(true);
    if ( view )
	enableCursor(true);
    viewMenu->addAction( "&Refresh Rate...", this, SLOT(changeRate()) );
    viewMenu->addSeparator();
    viewMenu->addAction( "No rotation", this, SLOT(setRot0()) );
    viewMenu->addAction( "90\260 rotation", this, SLOT(setRot90()) );
    viewMenu->addAction( "180\260 rotation", this, SLOT(setRot180()) );
    viewMenu->addAction( "270\260 rotation", this, SLOT(setRot270()) );
    viewMenu->addSeparator();
    viewMenu->addAction( "Zoom scale &0.5", this, SLOT(setZoomHalf()) );
    viewMenu->addAction( "Zoom scale 0.75", this, SLOT(setZoom075()) );
    viewMenu->addAction( "Zoom scale &1", this, SLOT(setZoom1()) );
    viewMenu->addAction( "Zoom scale &2", this, SLOT(setZoom2()) );
    viewMenu->addAction( "Zoom scale &3", this, SLOT(setZoom3()) );
    viewMenu->addAction( "Zoom scale &4", this, SLOT(setZoom4()) );
    viewMenu->addSeparator();
    viewMenu->addAction( "Zoom scale...", this, SLOT(setZoom()) );
    return viewMenu;
}


QMenu* QVFb::createHelpMenu()
{
    QMenu *help = new QMenu( "Help", this );
    help->addAction("About...", this, SLOT(about()));
    return help;
}

void QVFb::setZoom(double z)
{
    view->setZoom(z,z*skinscaleV/skinscaleH);
    if (secondaryView)
        secondaryView->setZoom(z,z*skinscaleV/skinscaleH);

    if (skin) {
	skin->setTransform(QMatrix().scale(z/skinscaleH,z/skinscaleV).rotate(90*view->displayRotation()));
        if (secondaryView)
            secondaryView->setFixedSize(
                    int(secondaryView->displayWidth()*z),
                    int(secondaryView->displayHeight()*z*skinscaleV/skinscaleH));
    }
}

void QVFb::setRotation(QVFbView::Rotation r)
{
    view->setRotation(r);
    if (secondaryView)
        secondaryView->setRotation(r);
    setZoom(view->zoomH());
}

void QVFb::setRot0()
{
    setRotation(QVFbView::Rot0);
}

void QVFb::setRot90()
{
    setRotation(QVFbView::Rot90);
}

void QVFb::setRot180()
{
    setRotation(QVFbView::Rot180);
}

void QVFb::setRot270()
{
    setRotation(QVFbView::Rot270);
}

void QVFb::setZoomHalf()
{
    setZoom(0.5);
}

void QVFb::setZoom075()
{
    setZoom(0.75);
}

void QVFb::setZoom1()
{
    setZoom(1);
}

void QVFb::setZoom()
{
    if ( !zoomer )
	zoomer = new Zoomer(this);
    zoomer->show();
}

void QVFb::setZoom2()
{
    setZoom(2);
}

void QVFb::setZoom3()
{
    setZoom(3);
}

void QVFb::setZoom4()
{
    setZoom(4);
}

void QVFb::saveImage()
{
    QImage img = view->image();
    QString filename = QFileDialog::getSaveFileName(this, "Save Main Screen image", "snapshot.png", "Portable Network Graphics (*.png)");
    if (!filename.isEmpty()){
        if(!img.save(filename,"PNG"))
                QMessageBox::critical(this, "Save Main Screen Image", "Save failed. Check that you have permission to write to the target directory.");
    }
    if (secondaryView) {
        QImage img = view->image();
        QString filename = QFileDialog::getSaveFileName(this, "Save Second Screen image", "snapshot.png", "Portable Network Graphics (*.png)");
        if (!filename.isEmpty()) {
            if(!img.save(filename,"PNG"))
                QMessageBox::critical(this, "Save Second Screen Image", "Save failed. Check that you have permission to write to the target directory.");
        }
    }
}

void QVFb::toggleAnimation()
{
    static AnimationSaveWidget *animWidget = 0;
    if ( !animWidget )
	animWidget = new AnimationSaveWidget(view);
    if ( animWidget->isVisible() )
	animWidget->hide();
    else
	animWidget->show();
}

void QVFb::toggleCursor()
{
    enableCursor(cursorAction->isChecked());
}

void QVFb::changeRate()
{
    if ( !rateDlg ) {
	rateDlg = new QVFbRateDialog( refreshRate, this );
	connect( rateDlg, SIGNAL(updateRate(int)), this, SLOT(setRate(int)) );
    }

    rateDlg->show();
}

void QVFb::setRate(int i)
{
    refreshRate = i;
    view->setRate(i);
    if (secondaryView)
        secondaryView->setRate(i);
}


void QVFb::about()
{
    QMessageBox::about(this, "About QVFB",
	"<h2>The Qt for Embedded Linux Virtual X11 Framebuffer</h2>"
	"<p>This application runs under Qt for X11, emulating a simple framebuffer, "
	"which the Qt for Embedded Linux server and clients can attach to just as if "
	"it was a hardware Linux framebuffer. "
	"<p>With the aid of this development tool, you can develop Qt for Embedded  "
	"Linux applications under X11 without having to switch to a virtual console. "
	"This means you can comfortably use your other development tools such "
	"as GUI profilers and debuggers."
    );
}

void QVFb::findSkins(const QString &currentSkin)
{
    skinnames.clear();
    skinfiles.clear();
    QDir dir(":/skins/","*.skin");
    const QFileInfoList l = dir.entryInfoList();
    int i = 1; // "None" is already in list at index 0
    for (QFileInfoList::const_iterator it = l.begin(); it != l.end(); ++it) {
	skinnames.append((*it).baseName()); // should perhaps be in file
	skinfiles.append((*it).filePath());
	if (((*it).baseName() + ".skin") == currentSkin)
	    currentSkinIndex = i;
	i++;
    }
}

class Config : public QDialog, public Ui::Config
{
public:
    Config(QWidget *parent)
        : QDialog(parent)
    {
        setupUi(this);
        setModal(true);

        connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
        connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    }
};

void QVFb::configure()
{
    config = new Config(this);

    int w = view->displayWidth();
    int h = view->displayHeight();

    // Need to block signals, because we connect to animateClick(),
    // since QCheckBox doesn't have setChecked(bool) in 2.x.
    chooseSize(QSize(w,h));
    config->skin->insertItems(config->skin->count(), skinnames);
    if (currentSkinIndex > 0)
	config->skin->setCurrentIndex(currentSkinIndex);
    config->skin->addItem(tr("Browse..."));
    config->touchScreen->setChecked(view->touchScreenEmulation());
    config->lcdScreen->setChecked(view->lcdScreenEmulation());
    chooseDepth(view->displayDepth(), view->displayFormat());
    connect(config->skin, SIGNAL(activated(int)), this, SLOT(skinConfigChosen(int)));
    if ( view->gammaRed() == view->gammaGreen() && view->gammaGreen() == view->gammaBlue() ) {
	config->gammaslider->setValue(int(view->gammaRed()*400));
	config->rslider->setValue(100);
	config->gslider->setValue(100);
	config->bslider->setValue(100);
    } else {
	config->gammaslider->setValue(100);
	config->rslider->setValue(int(view->gammaRed()*400));
	config->gslider->setValue(int(view->gammaGreen()*400));
	config->bslider->setValue(int(view->gammaBlue()*400));
    }
    connect(config->gammaslider, SIGNAL(valueChanged(int)), this, SLOT(setGamma400(int)));
    connect(config->rslider, SIGNAL(valueChanged(int)), this, SLOT(setR400(int)));
    connect(config->gslider, SIGNAL(valueChanged(int)), this, SLOT(setG400(int)));
    connect(config->bslider, SIGNAL(valueChanged(int)), this, SLOT(setB400(int)));
    updateGammaLabels();

    double ogr=view->gammaRed(), ogg=view->gammaGreen(), ogb=view->gammaBlue();
    qApp->setQuitOnLastWindowClosed(false);

    hide();
    if ( config->exec() ) {
	int id = view->displayId(); // not settable yet
	if ( config->size_176_220->isChecked() ) {
	    w=176; h=220;
	} else if ( config->size_240_320->isChecked() ) {
	    w=240; h=320;
	} else if ( config->size_320_240->isChecked() ) {
	    w=320; h=240;
	} else if ( config->size_640_480->isChecked() ) {
	    w=640; h=480;
	} else if ( config->size_800_600->isChecked() ) {
	    w=800; h=600;
	} else if ( config->size_1024_768->isChecked() ) {
	    w=1024; h=768;
	} else {
	    w=config->size_width->value();
	    h=config->size_height->value();
	}
	int d;
	if ( config->depth_1->isChecked() )
	    d=1;
	else if ( config->depth_4gray->isChecked() )
	    d=4;
	else if ( config->depth_8->isChecked() )
	    d=8;
	else if ( config->depth_12->isChecked() )
	    d=12;
	else if ( config->depth_15->isChecked() )
	    d = 15;
	else if ( config->depth_16->isChecked() )
	    d=16;
	else if ( config->depth_18->isChecked() )
	    d=18;
	else if ( config->depth_24->isChecked() )
	    d=24;
	else
	    d=32;
	QVFbView::PixelFormat displayFormat = config->depth_32_argb->isChecked()
					      ? QVFbView::ARGBFormat : QVFbView::DefaultFormat;
	int skinIndex = config->skin->currentIndex();
	if ( w != view->displayWidth() || h != view->displayHeight()
		|| d != view->displayDepth() || skinIndex != currentSkinIndex ) {
	    QVFbView::Rotation rot = view->displayRotation();
	    int r = ((rot == QVFbView::Rot90)  ?  90 :
		    ((rot == QVFbView::Rot180) ? 180 :
		    ((rot == QVFbView::Rot270) ? 270 : 0 )));
	    currentSkinIndex = skinIndex;
	    init( id, w, h, d, r, skinIndex > 0 ? skinfiles[skinIndex-1] : QString::null );
	}
	view->setViewFormat(displayFormat);
	view->setTouchscreenEmulation( config->touchScreen->isChecked() );
	bool lcdEmulation = config->lcdScreen->isChecked();
	view->setLcdScreenEmulation( lcdEmulation );
	if ( lcdEmulation )
	    setZoom3();
    } else {
	view->setGamma(ogr, ogg, ogb);
    }
    show();
    qApp->setQuitOnLastWindowClosed(true);
    delete config;
    config=0;
}

void QVFb::chooseSize(const QSize& sz)
{
    config->size_width->blockSignals(true);
    config->size_height->blockSignals(true);
    config->size_width->setValue(sz.width());
    config->size_height->setValue(sz.height());
    config->size_width->blockSignals(false);
    config->size_height->blockSignals(false);
    config->size_custom->setChecked(true); // unless changed by settings below
    config->size_176_220->setChecked(sz == QSize(176,220));
    config->size_240_320->setChecked(sz == QSize(240,320));
    config->size_320_240->setChecked(sz == QSize(320,240));
    config->size_640_480->setChecked(sz == QSize(640,480));
    config->size_800_600->setChecked(sz == QSize(800,600));
    config->size_1024_768->setChecked(sz == QSize(1024,768));
}

void QVFb::chooseDepth(int depth, QVFbView::PixelFormat displayFormat)
{
    config->depth_1->setChecked(depth==1);
    config->depth_4gray->setChecked(depth==4);
    config->depth_8->setChecked(depth==8);
    config->depth_12->setChecked(depth==12);
    config->depth_15->setChecked(depth==15);
    config->depth_16->setChecked(depth==16);
    config->depth_18->setChecked(depth==18);
    config->depth_24->setChecked(depth==24);
    config->depth_32->setChecked(depth==32 && displayFormat != QVFbView::ARGBFormat);
    config->depth_32_argb->setChecked(depth==32 && displayFormat == QVFbView::ARGBFormat);
}

void QVFb::skinConfigChosen(int i)
{
    if (i == config->skin->count() - 1) { // Browse... ?
        QFileDialog dlg(this);
        dlg.setFileMode(QFileDialog::DirectoryOnly);
        dlg.setWindowTitle(tr("Load Custom Skin..."));
        dlg.setFilter(tr("All QVFB Skins (*.skin)"));
        dlg.setDirectory(QDir::current());
        if (dlg.exec() && dlg.selectedFiles().count() == 1) {
            skinfiles.append(dlg.selectedFiles().first());
            i = skinfiles.count();
            config->skin->insertItem(i, QFileInfo(skinfiles.last()).baseName());
            config->skin->setCurrentIndex(i);
        } else {
            i = 0;
        }
    }
    if ( i ) {
	DeviceSkinParameters parameters;
	QString readError;
	if (parameters.read(skinfiles[i-1], DeviceSkinParameters::ReadSizeOnly, &readError)) {
	    chooseSize(parameters.screenSize());
            if (parameters.screenDepth)
                chooseDepth(parameters.screenDepth,QVFbView::ARGBFormat);
            config->touchScreen->setChecked(!parameters.hasMouseHover);
	} else {
	    qWarning("%s", qPrintable(readError));
	}
    }
}

void QVFb::setGamma400(int n)
{
    double g = n/100.0;
    view->setGamma(config->rslider->value()/100.0*g,
                   config->gslider->value()/100.0*g,
                   config->bslider->value()/100.0*g);
    updateGammaLabels();
}

void QVFb::setR400(int n)
{
    double g = n/100.0;
    view->setGamma(config->rslider->value()/100.0*g,
                   view->gammaGreen(),
                   view->gammaBlue());
    updateGammaLabels();
}

void QVFb::setG400(int n)
{
    double g = n/100.0;
    view->setGamma(view->gammaRed(),
                   config->gslider->value()/100.0*g,
                   view->gammaBlue());
    updateGammaLabels();
}

void QVFb::setB400(int n)
{
    double g = n/100.0;
    view->setGamma(view->gammaRed(),
                   view->gammaGreen(),
                   config->bslider->value()/100.0*g);
    updateGammaLabels();
}

void QVFb::updateGammaLabels()
{
    config->rlabel->setText(QString::number(view->gammaRed(),'g',2));
    config->glabel->setText(QString::number(view->gammaGreen(),'g',2));
    config->blabel->setText(QString::number(view->gammaBlue(),'g',2));
}

QSize QVFb::sizeHint() const
{
    return QSize(int(view->displayWidth()*view->zoomH()),
	    int(menuBar()->height()+view->displayHeight()*view->zoomV()));
}

// =====================================================================

AnimationSaveWidget::AnimationSaveWidget(QVFbAbstractView *v) :
	QWidget((QWidget*)0,0),
	view(v), recording(false), animation(0),
	timerId(-1), progressTimerId(-1),
	recOn(red_on_led_xpm), recOff(red_off_led_xpm),
	imageNum(0)
{
    // Create the animation record UI dialog
    QVBoxLayout *vlayout = new QVBoxLayout( this );

    QWidget *hbox = new QWidget( this );
    vlayout->addWidget(hbox);
    QHBoxLayout *hlayout = new QHBoxLayout(hbox);
    recBt = new QPushButton( tr("Record"), hbox );
    hlayout->addWidget(recBt);
    resetBt = new QPushButton( tr("Reset"), hbox );
    hlayout->addWidget(resetBt);
    saveBt = new QPushButton( tr("Save"), hbox );
    hlayout->addWidget(saveBt);
    recBt->setFixedWidth( 100 );
    resetBt->setFixedWidth( 100 );
    saveBt->setFixedWidth( 100 );
    timeDpy = new QLabel( "00:00", hbox );
    hlayout->addWidget(timeDpy);
    recLED = new QLabel( hbox );
    hlayout->addWidget(recLED);
    recLED->setPixmap( recOff );
    timeDpy->setMargin( 5 );
    connect( recBt, SIGNAL(clicked()), this, SLOT(toggleRecord()) );
    connect( resetBt, SIGNAL(clicked()), this, SLOT(reset()) );
    connect( saveBt, SIGNAL(clicked()), this, SLOT(save()) );
    elapsed = 0;
    vlayout->setMargin( 5 );
    vlayout->setSpacing( 5 );
    haveMpeg = detectPpmtoMpegCommand();
    mpegSave = new QCheckBox( tr("Save in MPEG format (requires netpbm package installed)"), this );
    vlayout->addWidget(mpegSave);
    mpegSave->setChecked( haveMpeg );
    mpegSave->setEnabled( haveMpeg );
    savingAsMpeg = haveMpeg;
    QWidget *hbox2 = new QWidget( this );
    vlayout->addWidget(hbox2);
    QHBoxLayout *hlayout2 = new QHBoxLayout( hbox2 );
    statusText = new QLabel( tr("Click record to begin recording."), hbox2 );
    hlayout2->addWidget(statusText);
    progressBar = new QProgressBar( hbox2 );
    progressBar->setValue( 0 );
    hlayout2->addWidget(progressBar);
    progressBar->hide();
}

AnimationSaveWidget::~AnimationSaveWidget()
{
    // clean up
    removeTemporaryFiles();
    delete animation;
}

// returns true if we have ppmtompeg command, else returns false
bool AnimationSaveWidget::detectPpmtoMpegCommand()
{
    // search the PATH for the ppmtompeg command to test we can record to mpeg
    QStringList paths = QString(::getenv("PATH")).split(":");
    for ( int i = 0; i < paths.count(); i++ )
	if ( QFile::exists( paths[i] + "/" + "ppmtompeg" ) )
	    return true;
    return false;
}

void AnimationSaveWidget::timerEvent( QTimerEvent *te )
{
    QString str;

    // Recording timer
    if ( te->timerId() == timerId ) {

	// Add a frame to the animation
	if ( savingAsMpeg && view )
	    view->image().save( str.sprintf("/tmp/qvfb_tmp_image_%04d.ppm", imageNum), "PPM");
	else if ( animation && view )
	    animation->appendFrame(view->image());//QPoint(0,0));
	imageNum++;

	// Update the display of number of seconds that have been recorded.
	int tmMsec = tm.elapsed();
	timeDpy->setText( str.sprintf("%02d:%02d", tmMsec/60000, (tmMsec%60000)/1000) );
	QObject::timerEvent( te );

	// Make the recording LED blink
	static int tick = 0;
	static bool on = false;
	if ( tick > 10 ) {
	    tick = 0;
	    if ( on )
		recLED->setPixmap( recOff );
	    else
		recLED->setPixmap( recOn );
	    on = !on;
	}
	tick++;
    }

    // Saving progress timer
    if ( te->timerId() == progressTimerId ) {
	// Parse output log file to work out the encoding progress.
	QFile f("/tmp/qvfb_tmp_output.log");
	f.open(QIODevice::ReadOnly);
	int largestNum = 0;
	bool done = false;
	char buffer[1024];
	while ( !f.atEnd() ) {
	    // example of the output log entries
	    //   During each frame:
	    //      "FRAME 764 (B):  I BLOCKS:  0......
	    //   When complete:
	    //      "======FRAMES READ:  766"
	    f.readLine(buffer, 1024);
	    str = QString(buffer);
	    if ( str.left(6) == "FRAME " ) {
		int num = str.mid(6, str.indexOf(QChar(' '), 6) - 6).toInt();
		if ( num > largestNum )
		    largestNum = num;
	    } else if ( str.left(18) == "======FRAMES READ:" ) {
		done = true;
	    }
	}
	f.close();

	// Update the progress bar with the frame we are up to
	progressBar->setValue( largestNum );

	// Finished saving
	if ( done ) {
	    progressBar->hide();
	    statusText->setText( tr("Finished saving."));
	    removeTemporaryFiles();
	    killTimer( progressTimerId );
	    progressTimerId = -1;
	    reset();
	}
    }
}

// Takes the saved ppm files and converts them to a mpeg file named filename
void AnimationSaveWidget::convertToMpeg(QString filename)
{
    recLED->setPixmap( recOff );
    killTimer( timerId );

    progressBar->show();
    progressBar->setRange( 0, imageNum );
    progressBar->setValue( 0 );

    // Build parameter file required by ppmtompeg
    QFile file("/tmp/qvfb_tmp_ppmtompeg.params");
    if ( file.open( QIODevice::WriteOnly ) ) {
	QTextStream t( &file );
	t << "PATTERN IBBPBBPBBPBBPBB\n";
	t << "OUTPUT " << filename << "\n";
	t << "INPUT_DIR /tmp\n";
	t << "INPUT\n";
	QString str;
	str = str.sprintf("%04d", imageNum - 1);
	t << "qvfb_tmp_image_*.ppm [0000-" << str << "]\n";
	t << "END_INPUT\n";
	t << "BASE_FILE_FORMAT PPM\n";
	t << "INPUT_CONVERT *\n";
	t << "GOP_SIZE 15\n";
	t << "SLICES_PER_FRAME 1\n";
	t << "PIXEL HALF\n";
	t << "RANGE 5\n";
	t << "PSEARCH_ALG LOGARITHMIC\n";
	t << "BSEARCH_ALG SIMPLE\n";
	t << "IQSCALE 1\n";
	t << "PQSCALE 1\n";
	t << "BQSCALE 1\n";
	t << "REFERENCE_FRAME DECODED\n";
	t << "ASPECT_RATIO 1\n";
	t << "FRAME_RATE 24\n";
	t << "BIT_RATE 64000\n";	    // Quality
	t << "BUFFER_SIZE 2048\n";
    }
    file.close();

    // ### can't use QProcess, not in Qt 2.3
    // ### but it's certainly in Qt 4! use it?
    // Execute the ppmtompeg command as a seperate process to do the encoding
    pid_t pid = ::fork();
    if ( !pid ) {
	// Child process
	// redirect stdout to log file
	freopen("/tmp/qvfb_tmp_output.log", "w", stdout);
	// ppmtompeg tool is from the netpbm package
	::execlp("ppmtompeg", "ppmtompeg", "/tmp/qvfb_tmp_ppmtompeg.params", (void *)0);
	exit(0);
    }

    // Update the saving progress bar every 200ms
    progressTimerId = startTimer( 200 );
}

// Cleanup temporary files created during creating a mpeg file
void AnimationSaveWidget::removeTemporaryFiles()
{
    QString str;
    for ( int i = 0; i < imageNum; i++ )
	QFile::remove( str.sprintf("/tmp/qvfb_tmp_image_%04d.ppm", i) );
    QFile::remove("/tmp/qvfb_tmp_ppmtompeg.params");
    QFile::remove("/tmp/qvfb_tmp_output.log");
    imageNum = 0;
}

// toggles between recording and paused (usually when record button clicked)
void AnimationSaveWidget::toggleRecord()
{
    if ( recording ) {
	recLED->setPixmap( recOff );
	recBt->setText( tr("Record") );
	statusText->setText( tr("Paused. Click record to resume, or save if done."));
	killTimer( timerId );
	timerId = -1;
	elapsed = tm.elapsed();
    } else {
	recLED->setPixmap( recOn );
	recBt->setText( tr("Pause") );
	statusText->setText( tr("Recording..."));
	tm.start();
	if ( elapsed == 0 ) {
	    savingAsMpeg = mpegSave->isChecked();
	    if ( !savingAsMpeg ) {
		delete animation;
		animation = new QAnimationWriter("/tmp/qvfb_tmp_animation.mng","MNG");
		animation->setFrameRate(24);
		if ( view )
		    animation->appendFrame(view->image());
	    }
	}
	tm = tm.addMSecs(-elapsed);
	elapsed = 0;
	timerId = startTimer(1000 / 24);
    }
    recording = !recording;
}

// Reset everything to initial state of not recording
void AnimationSaveWidget::reset()
{
    if ( recording ) {
	toggleRecord();
	statusText->setText( tr("Click record to begin recording."));
	removeTemporaryFiles();
    }
    progressBar->setValue( 0 );
    timeDpy->setText( "00:00" );
    elapsed = 0;
    imageNum = 0;
    delete animation;
    animation = 0;
}

// Prompt for filename to save to and put animation in that file
void AnimationSaveWidget::save()
{
    if ( recording )
	toggleRecord(); // pauses
    statusText->setText( tr("Saving... "));

    QString filename;
    if ( savingAsMpeg ) {
	filename = QFileDialog::getSaveFileName(this, tr("Save animation..."), "", "*.mpg");
	if ( !filename.isNull() )
	    convertToMpeg(filename);
    } else {
	filename = QFileDialog::getSaveFileName(this, tr("Save animation..."), "", "*.mng");
	if (filename.isNull()) {
            statusText->setText(tr("Save canceled."));
        } else {
            QFile::remove(filename);
            bool success = QFile::rename(QLatin1String("/tmp/qvfb_tmp_animation.mng"),
                                         filename);
            if (success) {
                statusText->setText(tr("Finished saving."));
                reset();
            } else {
                statusText->setText(tr("Save failed!"));
            }
	}
    }
}

QT_END_NAMESPACE

#include "qvfb.moc"
