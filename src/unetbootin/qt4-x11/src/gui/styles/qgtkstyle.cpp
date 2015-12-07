/*******    *********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qgtkstyle.h"

#if !defined(QT_NO_STYLE_GTK)

#include <private/qapplication_p.h>
#include <QtCore/QLibrary>
#include <QtCore/QSettings>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QStatusBar>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtGui/QListView>
#include <QtGui/QApplication>
#include <QtGui/QStyleOption>
#include <QtGui/QPushButton>
#include <QtGui/QPainter>
#include <QtGui/QMainWindow>
#include <QtGui/QToolBar>
#include <QtGui/QHeaderView>
#include <QtGui/QMenuBar>
#include <QtGui/QComboBox>
#include <QtGui/QSpinBox>
#include <QtGui/QScrollBar>
#include <QtGui/QAbstractButton>
#include <QtGui/QToolButton>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QTreeView>
#include <qpixmapcache.h>
#undef signals // Collides with GTK stymbols
#include "qgtkpainter_p.h"

#include <private/qcleanlooksstyle_p.h>


QT_BEGIN_NAMESPACE

typedef QStringList (*_qt_filedialog_open_filenames_hook)(QWidget * parent, const QString &caption, const QString &dir,
                                                          const QString &filter, QString *selectedFilter, QFileDialog::Options options);
typedef QString (*_qt_filedialog_open_filename_hook)     (QWidget * parent, const QString &caption, const QString &dir,
                                                          const QString &filter, QString *selectedFilter, QFileDialog::Options options);
typedef QString (*_qt_filedialog_save_filename_hook)     (QWidget * parent, const QString &caption, const QString &dir,
                                                          const QString &filter, QString *selectedFilter, QFileDialog::Options options);
typedef QString (*_qt_filedialog_existing_directory_hook)(QWidget *parent, const QString &caption, const QString &dir,
                                                          QFileDialog::Options options);

extern Q_GUI_EXPORT _qt_filedialog_open_filename_hook qt_filedialog_open_filename_hook;
extern Q_GUI_EXPORT _qt_filedialog_open_filenames_hook qt_filedialog_open_filenames_hook;
extern Q_GUI_EXPORT _qt_filedialog_save_filename_hook qt_filedialog_save_filename_hook;
extern Q_GUI_EXPORT _qt_filedialog_existing_directory_hook qt_filedialog_existing_directory_hook;

static const char * const dock_widget_close_xpm[] =
    {
        "11 13 5 1",
        "  c None",
        ". c #D5CFCB",
        "+ c #6C6A67",
        "@ c #6C6A67",
        "$ c #B5B0AC",
        "           ",
        " @@@@@@@@@ ",
        "@+       +@",
        "@ +@   @+ @",
        "@ @@@ @@@ @",
        "@  @@@@@  @",
        "@   @@@   @",
        "@  @@@@@  @",
        "@ @@@ @@@ @",
        "@ +@   @+ @",
        "@+       +@",
        " @@@@@@@@@ ",
        "           "
    };

static const char * const dock_widget_restore_xpm[] =
    {
        "11 13 5 1",
        "  c None",
        ". c #D5CFCB",
        "+ c #6C6A67",
        "@ c #6C6A67",
        "# c #6C6A67",
        "           ",
        " @@@@@@@@@ ",
        "@+       +@",
        "@   #@@@# @",
        "@   @   @ @",
        "@ #@@@# @ @",
        "@ @   @ @ @",
        "@ @   @@@ @",
        "@ @   @   @",
        "@ #@@@@   @",
        "@+       +@",
        " @@@@@@@@@ ",
        "           "
    };


class QGtkStylePrivate : public QCleanlooksStylePrivate
{
    Q_DECLARE_PUBLIC(QGtkStyle)
public:
    QGtkStylePrivate()
            : QCleanlooksStylePrivate()
    {}
};

static const int groupBoxBottomMargin    =  2;  // space below the groupbox
static const int groupBoxTitleMargin     =  6;  // space between contents and title
static const int groupBoxTopMargin       =  2;
static bool UsePixmapCache = true;

// Get size of the arrow controls in a GtkSpinButton
static int spinboxArrowSize()
{
    const int MIN_ARROW_WIDTH = 6;
    GtkWidget *spinButton = QGtk::gtkWidget(QLS("GtkSpinButton"));
    GtkStyle *style = spinButton->style;
    gint size = QGtk::pango_font_description_get_size (style->font_desc);
    gint arrow_size;
    arrow_size = qMax(PANGO_PIXELS (size), MIN_ARROW_WIDTH) + style->xthickness;
    arrow_size += arrow_size%2 + 1;
    return arrow_size;
}

static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50)
{
    const int maxFactor = 100;
    QColor tmp = colorA;
    tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

static GdkColor fromQColor(const QColor &color)
{
    GdkColor retval;
    retval.red = color.red() * 255;
    retval.green = color.green() * 255;
    retval.blue = color.blue() * 255;
    return retval;
}

// Note this is different from uniqueName as used in QGtkPainter
static QString uniqueName(const QString &key, const QStyleOption *option, const QSize &size)
{
    QString tmp;
    const QStyleOptionComplex *complexOption = qstyleoption_cast<const QStyleOptionComplex *>(option);
    tmp.sprintf("%s-%d-%d-%d-%lld-%dx%d", key.toLatin1().constData(), uint(option->state),
                option->direction, complexOption ? uint(complexOption->activeSubControls) : uint(0),
                option->palette.cacheKey(), size.width(), size.height());
    return tmp;
}

/*!
    \class QGtkStyle
    \brief The QGtkStyle class provides a widget style rendered by GTK+
    \since 4.5

    The QGtkStyle style provides a look and feel that integrates well
    into GTK-based desktop environments such as the XFCe and GNOME.

    It does this by making use of the GTK+ theme engine, ensuring
    that Qt applications look and feel native on these platforms.

    Note: The style requires GTK+ version 2.10 or later.
          The Qt3-based "Qt" GTK+ theme engine will not work with QGtkStyle.

    \sa {Cleanlooks Style Widget Gallery}, QWindowsXPStyle, QMacStyle, QWindowsStyle,
        QCDEStyle, QMotifStyle, QPlastiqueStyle, QCleanlooksStyle
*/

/*!
    Constructs a QGtkStyle object.
*/
QGtkStyle::QGtkStyle()
{
    QGtk::initGtkWidgets();
}

/*!
    Destroys the QGtkStyle object.
*/
QGtkStyle::~QGtkStyle()
{
}

/*!
    \reimp
*/
QPalette QGtkStyle::standardPalette() const
{
    QPalette palette = QCleanlooksStyle::standardPalette();
    if (QGtk::isThemeAvailable()) {
        GtkStyle *style = QGtk::gtkStyle();
        GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
        GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("GtkEntry"));

        GdkColor gdkBg, gdkBase, gdkText, gdkForeground, gdkSbg, gdkSfg;
        QColor bg, base, text, fg, highlight, highlightText;
        gdkBg = style->bg[GTK_STATE_NORMAL];
        gdkForeground = gtkButton->style->fg[GTK_STATE_NORMAL];

        // Our base and selected color is primarily used for text
        // so we assume a gtkEntry will have the most correct value
        gdkBase = gtkEntry->style->base[GTK_STATE_NORMAL];
        gdkText = gtkEntry->style->text[GTK_STATE_NORMAL];
        gdkSbg = gtkEntry->style->base[GTK_STATE_SELECTED];
        gdkSfg = gtkEntry->style->text[GTK_STATE_SELECTED];
        bg = QColor(gdkBg.red>>8, gdkBg.green>>8, gdkBg.blue>>8);
        text = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
        fg = QColor(gdkForeground.red>>8, gdkForeground.green>>8, gdkForeground.blue>>8);
        base = QColor(gdkBase.red>>8, gdkBase.green>>8, gdkBase.blue>>8);
        highlight = QColor(gdkSbg.red>>8, gdkSbg.green>>8, gdkSbg.blue>>8);
        highlightText = QColor(gdkSfg.red>>8, gdkSfg.green>>8, gdkSfg.blue>>8);

        palette.setColor(QPalette::HighlightedText, highlightText);
        palette.setColor(QPalette::Light, bg.lighter(125));
        palette.setColor(QPalette::Shadow, bg.darker(130));
        palette.setColor(QPalette::Dark, bg.darker(120));
        palette.setColor(QPalette::Text, text);
        palette.setColor(QPalette::WindowText, fg);
        palette.setColor(QPalette::ButtonText, fg);
        palette.setColor(QPalette::Base, base);

        QColor alternateRowColor = palette.base().color().lighter(93); // ref gtkstyle.c draw_flat_box
        GtkWidget *gtkTreeView = QGtk::gtkWidget(QLS("GtkTreeView"));
        GdkColor *gtkAltBase = NULL;
        QGtk::gtk_widget_style_get(gtkTreeView, "odd-row-color", &gtkAltBase, NULL);
        if (gtkAltBase) {
            alternateRowColor = QColor(gtkAltBase->red>>8, gtkAltBase->green>>8, gtkAltBase->blue>>8);
            QGtk::gdk_color_free(gtkAltBase);
        }
        palette.setColor(QPalette::AlternateBase, alternateRowColor);

        palette.setColor(QPalette::Window, bg);
        palette.setColor(QPalette::Button, bg);
        palette.setColor(QPalette::Background, bg);
        QColor disabled((fg.red()   + bg.red())  / 2,
                        (fg.green() + bg.green())/ 2,
                        (fg.blue()  + bg.blue()) / 2);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
        palette.setColor(QPalette::Disabled, QPalette::Foreground, disabled);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
        palette.setColor(QPalette::Highlight, highlight);
        // calculate disabled colors by removing saturation
        highlight.setHsv(highlight.hue(), 0, highlight.value(), highlight.alpha());
        highlightText.setHsv(highlightText.hue(), 0, highlightText.value(), highlightText.alpha());
        palette.setColor(QPalette::Disabled, QPalette::Highlight, highlight);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, highlightText);
        style = QGtk::gtk_rc_get_style_by_paths(QGtk::gtk_settings_get_default(), "gtk-tooltips", "GtkWindow", Q_GTK_TYPE_WINDOW);
        if (style) {
            gdkText = style->fg[GTK_STATE_NORMAL];
            text = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
            palette.setColor(QPalette::ToolTipText, text);
        }
    }
    return palette;
}

/*!
    \reimp
*/
void QGtkStyle::polish(QPalette &palette)
{
    // QCleanlooksStyle will alter the palette, hence we do
    // not want to polish the palette unless we are using it as
    // the fallback
    if (!QGtk::isThemeAvailable())
        QCleanlooksStyle::polish(palette);
    else
        palette = standardPalette();
}

/*!
    \reimp
*/
void QGtkStyle::polish(QApplication *app)
{
    QCleanlooksStyle::polish(app);
    // Custom fonts and palettes with QtConfig are intentionally 
    // not supported as these should be entirely determined by
    // current Gtk settings
    if (app->desktopSettingsAware() && QGtk::isThemeAvailable()) {
        QApplicationPrivate::setSystemPalette(standardPalette());
        QApplicationPrivate::setSystemFont(QGtk::getThemeFont());
        if (!QGtk::isKDE4Session()) {
            qt_filedialog_open_filename_hook = &QGtk::openFilename;
            qt_filedialog_save_filename_hook = &QGtk::saveFilename;
            qt_filedialog_open_filenames_hook = &QGtk::openFilenames;
            qt_filedialog_existing_directory_hook = &QGtk::openDirectory;
        }
    }
}

/*!
    \reimp
*/
void QGtkStyle::unpolish(QApplication *app)
{
    QCleanlooksStyle::unpolish(app);
    QPixmapCache::clear();

    if (app->desktopSettingsAware() && QGtk::isThemeAvailable()
        && !QGtk::isKDE4Session()) {
        qt_filedialog_open_filename_hook = 0;
        qt_filedialog_save_filename_hook = 0;
        qt_filedialog_open_filenames_hook = 0;
        qt_filedialog_existing_directory_hook = 0;
    }
}

/*!
    \reimp
*/
void QGtkStyle::polish(QWidget *widget)
{
    QCleanlooksStyle::polish(widget);
    if (!QGtk::isThemeAvailable())
        return;

    if (qobject_cast<QAbstractButton*>(widget)
            || qobject_cast<QToolButton*>(widget)
            || qobject_cast<QComboBox*>(widget)
            || qobject_cast<QGroupBox*>(widget)
            || qobject_cast<QScrollBar*>(widget)
            || qobject_cast<QSlider*>(widget)
            || qobject_cast<QAbstractSpinBox*>(widget)
            || qobject_cast<QSpinBox*>(widget)
            || qobject_cast<QHeaderView*>(widget))
        widget->setAttribute(Qt::WA_Hover);
    else if (QTreeView *tree = qobject_cast<QTreeView *> (widget))
        tree->viewport()->setAttribute(Qt::WA_Hover);

    QGtk::applyGtkSystemPalette(widget);
}

/*!
    \reimp
*/
void QGtkStyle::unpolish(QWidget *widget)
{
    QCleanlooksStyle::unpolish(widget);
}

/*!
    \reimp
*/
int QGtkStyle::pixelMetric(PixelMetric metric,

                           const QStyleOption *option,
                           const QWidget *widget) const
{
    if (!QGtk::isThemeAvailable())
        return QCleanlooksStyle::pixelMetric(metric, option, widget);

    switch (metric) {
    case PM_MenuButtonIndicator:
        return 20;

    case PM_TabBarBaseOverlap:
        return 1;

    case PM_ToolBarSeparatorExtent:
        return 11;

    case PM_ToolBarFrameWidth:
        return 1;

    case PM_ToolBarItemSpacing:
        return 0;

    case PM_ButtonShiftHorizontal: {
        GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
        guint horizontal_shift;
        QGtk::gtk_widget_style_get(gtkButton, "child-displacement-x", &horizontal_shift, NULL);
        return horizontal_shift;
    }

    case PM_ButtonShiftVertical: {
        GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
        guint vertical_shift;
        QGtk::gtk_widget_style_get(gtkButton, "child-displacement-y", &vertical_shift, NULL);
        return vertical_shift;
    }

    case PM_MenuBarPanelWidth:
        return 0;

    case PM_MenuPanelWidth: {
        GtkWidget *gtkMenu = QGtk::gtkWidget(QLS("GtkMenu"));
        guint horizontal_padding = 0;
        // horizontal-padding is used by Maemo to get thicker borders
        if (!QGtk::gtk_check_version(2, 10, 0))
            QGtk::gtk_widget_style_get(gtkMenu, "horizontal-padding", &horizontal_padding, NULL);
        int padding = qMax<int>(gtkMenu->style->xthickness, horizontal_padding);
        return padding;
    }

    case PM_ButtonIconSize: {
        int retVal = 24;
        GtkSettings *settings = QGtk::gtk_settings_get_default();
        gchararray icon_sizes;
        g_object_get(settings, "gtk-icon-sizes", &icon_sizes, NULL);
        QStringList values = QString(QLS(icon_sizes)).split(QLatin1Char(':'));
        g_free(icon_sizes);
        QChar splitChar(QLatin1Char(','));
        foreach (const QString &value, values) {
            if (value.startsWith(QLS("gtk-button="))) {
                QString iconSize = value.right(value.size() - 11);

                if (iconSize.contains(splitChar))
                    retVal = iconSize.split(splitChar)[0].toInt();
                break;
            }
        }
        return retVal;
    }

    case PM_MenuVMargin:

    case PM_MenuHMargin:
        return 0;

    case PM_DockWidgetTitleMargin:
        return 0;

    case PM_DockWidgetTitleBarButtonMargin:
        return 5;

    case PM_TabBarTabVSpace:
        return 12;

    case PM_TabBarTabHSpace:
        return 14;

    case PM_TabBarTabShiftVertical:
        return 2;

    case PM_ToolBarHandleExtent:
        return 9;

    case PM_SplitterWidth:
        return 6;

    case PM_SliderThickness:
    case PM_SliderControlThickness: {
        GtkWidget *gtkScale = QGtk::gtkWidget(QLS("GtkHScale"));
        gint val;
        QGtk::gtk_widget_style_get(gtkScale, "slider-width", &val, NULL);
        if (metric == PM_SliderControlThickness)
            return val + 2*gtkScale->style->ythickness;
        return val;
    }

    case PM_ScrollBarExtent: {
        gint sliderLength;
        gint trough_border;
        GtkWidget *hScrollbar = QGtk::gtkWidget(QLS("GtkHScrollbar"));
        QGtk::gtk_widget_style_get(hScrollbar,
                               "trough-border",   &trough_border,
                               "slider-width",    &sliderLength,
                               NULL);
        return sliderLength + trough_border*2;
    }

    case PM_ScrollBarSliderMin:
        return 34;

    case PM_SliderLength:
        gint val;
        QGtk::gtk_widget_style_get(QGtk::gtkWidget(QLS("GtkHScale")), "slider-length", &val, NULL);
        return val;

    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
    case PM_IndicatorWidth:
    case PM_IndicatorHeight: {
        GtkWidget *gtkCheckButton = QGtk::gtkWidget(QLS("GtkCheckButton"));
        gint size, spacing;
        QGtk::gtk_widget_style_get(gtkCheckButton, "indicator-spacing", &spacing, "indicator-size", &size, NULL);
        return size + 2 * spacing;
    }

    case PM_MenuBarVMargin: {
        GtkWidget *gtkMenubar = QGtk::gtkWidget(QLS("GtkMenuBar"));
        return  qMax(0, gtkMenubar->style->ythickness);
    }
    case PM_ScrollView_ScrollBarSpacing:
    {
        gint spacing = 3;
        GtkWidget *gtkScrollWindow = QGtk::gtkWidget(QLS("GtkScrolledWindow"));
        Q_ASSERT(gtkScrollWindow);
        QGtk::gtk_widget_style_get(gtkScrollWindow, "scrollbar-spacing", &spacing, NULL);
        return spacing;
    }
    case PM_SubMenuOverlap: {
        gint offset = 0;
        GtkWidget *gtkMenu = QGtk::gtkWidget(QLS("GtkMenu"));
        QGtk::gtk_widget_style_get(gtkMenu, "horizontal-offset", &offset, NULL);
        return offset;
    }
    default:
        return QCleanlooksStyle::pixelMetric(metric, option, widget);
    }
}

/*!
    \reimp
*/
int QGtkStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget,

                         QStyleHintReturn *returnData = 0) const
{
    if (!QGtk::isThemeAvailable())
        return QCleanlooksStyle::styleHint(hint, option, widget, returnData);

    switch (hint) {

    case SH_DialogButtonLayout: {
        int ret = QDialogButtonBox::GnomeLayout;
        gboolean alternateOrder = 0;
        GtkSettings *settings = QGtk::gtk_settings_get_default();
        g_object_get(settings, "gtk-alternative-button-order", &alternateOrder, NULL);

        if (alternateOrder)
            ret = QDialogButtonBox::WinLayout;

        return ret;
    }

    break;

    case SH_SpinControls_DisableOnBounds:
        return true;

    case SH_DitherDisabledText:
        return int(false);

    case SH_ComboBox_Popup: {
        GtkWidget *gtkComboBox = QGtk::gtkWidget(QLS("GtkComboBox"));
        gboolean appears_as_list;
        QGtk::gtk_widget_style_get((GtkWidget*)gtkComboBox, "appears-as-list", &appears_as_list, NULL);
        return appears_as_list ? 0 : 1;
    }

    case SH_EtchDisabledText:
        return int(false);

    case SH_Menu_SubMenuPopupDelay: {
        gint delay = 225;
        GtkSettings *settings = QGtk::gtk_settings_get_default();
        g_object_get(settings, "gtk-menu-popup-delay", &delay, NULL);
        return delay;
    }

    case SH_ScrollView_FrameOnlyAroundContents: {
        gboolean scrollbars_within_bevel = false;
        if (widget && widget->isWindow())
            scrollbars_within_bevel = true;
        else if (!QGtk::gtk_check_version(2, 12, 0)) {
            GtkWidget *gtkScrollWindow = QGtk::gtkWidget(QLS("GtkScrolledWindow"));
            QGtk::gtk_widget_style_get(gtkScrollWindow, "scrollbars-within-bevel", &scrollbars_within_bevel, NULL);
        }
        return !scrollbars_within_bevel;
    }

    default:
        return QCleanlooksStyle::styleHint(hint, option, widget, returnData);
    }
}

/*!
    \reimp
*/
void QGtkStyle::drawPrimitive(PrimitiveElement element,

                              const QStyleOption *option,
                              QPainter *painter,
                              const QWidget *widget) const
{
    if (!QGtk::isThemeAvailable()) {
        QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
        return;
    }

    GtkStyle* style = QGtk::gtkStyle();
    QGtkPainter gtkPainter(painter);

    switch (element) {

    case PE_PanelTipLabel: {
        GtkWidget *gtkWindow = QGtk::gtkWidget(QLS("GtkWindow")); // The Murrine Engine currently assumes a widget is passed
        style = QGtk::gtk_rc_get_style_by_paths(QGtk::gtk_settings_get_default(), "gtk-tooltips", "GtkWindow", Q_GTK_TYPE_WINDOW);
        gtkPainter.paintFlatBox(gtkWindow, "tooltip", option->rect, GTK_STATE_NORMAL, GTK_SHADOW_NONE, style);
    }
    break;

    case PE_PanelStatusBar: {
        if (widget && widget->testAttribute(Qt::WA_SetPalette) &&
            option->palette.resolve() & (1 << QPalette::Window)) {
            // Respect custom palette
            painter->fillRect(option->rect, option->palette.window());
            break;
        }
        GtkShadowType shadow_type;
        GtkWidget *gtkStatusbarFrame = QGtk::gtkWidget(QLS("GtkStatusbar.GtkFrame"));
        QGtk::gtk_widget_style_get(gtkStatusbarFrame->parent, "shadow-type", &shadow_type, NULL);
        gtkPainter.paintShadow(gtkStatusbarFrame, "frame", option->rect, GTK_STATE_NORMAL,
                               shadow_type, gtkStatusbarFrame->style);
    }
    break;

    case PE_IndicatorHeaderArrow:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            GtkWidget *gtkTreeHeader = QGtk::gtkWidget(QLS("GtkTreeView.GtkButton"));
            GtkStateType state = gtkPainter.gtkState(option);
            style = gtkTreeHeader->style;
            GtkArrowType type = GTK_ARROW_UP;
            QRect r = header->rect;
            QImage arrow;
            if (header->sortIndicator & QStyleOptionHeader::SortUp)
                type = GTK_ARROW_UP;
            else if (header->sortIndicator & QStyleOptionHeader::SortDown)
                type = GTK_ARROW_DOWN;

            gtkPainter.paintArrow(gtkTreeHeader, "button", option->rect.adjusted(1, 1, -1, -1), type, state,
                                  GTK_SHADOW_NONE, FALSE, style);
        }
        break;

    case PE_FrameFocusRect:
        if (!widget || qobject_cast<const QAbstractItemView*>(widget))
            QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
        else {
            // ### this mess should move to subcontrolrect
            QRect frameRect = option->rect.adjusted(1, 1, -1, -2);

            if (qobject_cast<const QTabBar*>(widget))
                frameRect.adjust(-1, 1, 1, 1);

            gtkPainter.paintFocus(NULL, "tab", frameRect, GTK_STATE_ACTIVE, style);
        }
        break;

    case PE_IndicatorBranch:
        if (option->state & State_Children) {
            QRect rect = option->rect;
            rect = QRect(0, 0, 10, 10);
            rect.moveCenter(option->rect.center());
            rect.translate(2, 0);
            GtkExpanderStyle openState = GTK_EXPANDER_EXPANDED;
            GtkExpanderStyle closedState = GTK_EXPANDER_COLLAPSED;
            GtkWidget *gtkExpander = QGtk::gtkWidget(QLS("GtkExpander"));
            guint expanderSize;
            QGtk::gtk_widget_style_get(gtkExpander, "expander-size", &expanderSize, NULL);
            // Note CleanIce will crash unless a GtkExpander is provided
            // but providing the expander will enforce the expander-size, which we
            // don't neccessarily have room for
            GtkStateType state = GTK_STATE_NORMAL;
            if (!(option->state & State_Enabled))
                state = GTK_STATE_INSENSITIVE;
            else if (option->state & State_MouseOver)
                state = GTK_STATE_PRELIGHT;
            gtkPainter.paintExpander(expanderSize <= 10 ? gtkExpander : NULL, "expander", rect, state,
                                     option->state & State_Open ? openState : closedState , gtkExpander->style);
        }
        break;
    case PE_PanelItemViewItem:
        if (const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option)) {
            if (vopt->state & State_Selected) {
                QLinearGradient gradient;
                gradient.setStart(option->rect.left(), option->rect.top());
                gradient.setFinalStop(option->rect.left(), option->rect.bottom());
                gradient.setColorAt(0, option->palette.highlight().color().lighter(105));
                gradient.setColorAt(0.5, option->palette.highlight().color().lighter(101));
                gradient.setColorAt(0.51, option->palette.highlight().color().darker(101));
                gradient.setColorAt(1, option->palette.highlight().color().darker(105));
                painter->fillRect(option->rect, gradient);
            } else {
                if (vopt->backgroundBrush.style() != Qt::NoBrush) {
                    QPointF oldBO = painter->brushOrigin();
                    painter->setBrushOrigin(vopt->rect.topLeft());
                    painter->fillRect(vopt->rect, vopt->backgroundBrush);
                    painter->setBrushOrigin(oldBO);
                }
            }
        }
        break;
    case PE_IndicatorToolBarSeparator:
        {
            const int margin = 6;
            GtkWidget *gtkSeparator = QGtk::gtkWidget(QLS("GtkToolbar.GtkSeparatorToolItem"));
            if (option->state & State_Horizontal) {
                const int offset = option->rect.width()/2;
                QRect rect = option->rect.adjusted(offset, margin, 0, -margin);
                painter->setPen(QPen(option->palette.background().color().darker(110)));
                gtkPainter.paintVline( gtkSeparator, "vseparator",
                                       rect, GTK_STATE_NORMAL, gtkSeparator->style,
                                       0, rect.height(), 0);
            } else { //Draw vertical separator
                const int offset = option->rect.height()/2;
                QRect rect = option->rect.adjusted(margin, offset, -margin, 0);
                painter->setPen(QPen(option->palette.background().color().darker(110)));
                gtkPainter.paintHline( gtkSeparator, "hseparator",
                                       rect, GTK_STATE_NORMAL, gtkSeparator->style,
                                       0, rect.width(), 0);
            }
       }
       break;

    case PE_IndicatorToolBarHandle: {
        GtkWidget *gtkToolbar = QGtk::gtkWidget(QLS("GtkToolbar"));
        GtkShadowType shadow_type;
        QGtk::gtk_widget_style_get(gtkToolbar, "shadow-type", &shadow_type, NULL);
        //Note when the toolbar is horizontal, the handle is vertical
        painter->setClipRect(option->rect);
        gtkPainter.paintHandle(gtkToolbar, "toolbar", option->rect.adjusted(-1, -1 ,0 ,1),
                               GTK_STATE_NORMAL, shadow_type, !(option->state & State_Horizontal) ?
                               GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL, gtkToolbar->style);
    }
    break;

    case PE_IndicatorArrowUp:
    case PE_IndicatorArrowDown:
    case PE_IndicatorArrowLeft:
    case PE_IndicatorArrowRight: {


        GtkArrowType type = GTK_ARROW_UP;

        switch (element) {

        case PE_IndicatorArrowDown:
            type = GTK_ARROW_DOWN;
            break;

        case PE_IndicatorArrowLeft:
            type = GTK_ARROW_LEFT;
            break;

        case PE_IndicatorArrowRight:
            type = GTK_ARROW_RIGHT;
            break;

        default:
            break;
        }
        int size = qMin(option->rect.height(), option->rect.width());
        int border = (size > 9) ? (size/4) : 0; //Allow small arrows to have exact dimensions
        int bsx = 0, bsy = 0;
        if (option->state & State_Sunken) {
            bsx = pixelMetric(PM_ButtonShiftHorizontal);
            bsy = pixelMetric(PM_ButtonShiftVertical);
        }
        QRect arrowRect = option->rect.adjusted(border + bsx, border + bsy, -border + bsx, -border + bsy);
        GtkShadowType shadow = option->state & State_Sunken ? GTK_SHADOW_IN : GTK_SHADOW_OUT;
        GtkStateType state = gtkPainter.gtkState(option);

        QColor arrowColor = option->palette.buttonText().color();
        GtkWidget *gtkArrow = QGtk::gtkWidget(QLS("GtkArrow"));
        GdkColor color = fromQColor(arrowColor);
        QGtk::gtk_widget_modify_fg (gtkArrow, state, &color);
        gtkPainter.paintArrow(gtkArrow, "button", arrowRect,
                              type, state, shadow, FALSE, gtkArrow->style,
                              QString::number(arrowColor.rgba(), 16));
        // Passing NULL will revert the color change
        QGtk::gtk_widget_modify_fg (gtkArrow, state, NULL);
    }
    break;

    case PE_FrameGroupBox:
        // Do nothing here, the GNOME groupboxes are flat
        break;

    case PE_PanelMenu: {
            GtkWidget *gtkMenu = QGtk::gtkWidget(QLS("GtkMenu"));
            gtkPainter.setAlphaSupport(false); // Note, alpha disabled for performance reasons
            gtkPainter.paintBox(gtkMenu, "menu", option->rect, GTK_STATE_NORMAL, GTK_SHADOW_OUT, gtkMenu->style, QString());
        }
        break;

    case PE_FrameMenu:
        //This is actually done by PE_Widget due to a clipping issue
        //Otherwise Menu items will not be able to span the entire menu width

        // This is only used by floating tool bars
        if (qobject_cast<const QToolBar *>(widget)) {
            GtkWidget *gtkMenubar = QGtk::gtkWidget(QLS("GtkMenuBar"));
            gtkPainter.paintBox( gtkMenubar, "toolbar",  option->rect,
                                 GTK_STATE_NORMAL, GTK_SHADOW_OUT, style);
            gtkPainter.paintBox( gtkMenubar, "menu",  option->rect,
                                 GTK_STATE_NORMAL, GTK_SHADOW_OUT, style);
        }
        break;

    case PE_FrameLineEdit: {
        GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("GtkEntry"));

        if (option->state & State_HasFocus)
            GTK_WIDGET_SET_FLAGS(gtkEntry, GTK_HAS_FOCUS);
        else
            GTK_WIDGET_UNSET_FLAGS(gtkEntry, GTK_HAS_FOCUS);

        gboolean interior_focus;
        gint focus_line_width;
        QRect rect = option->rect;
        QGtk::gtk_widget_style_get(gtkEntry,
                               "interior-focus", &interior_focus,
                               "focus-line-width", &focus_line_width, NULL);

        if (!interior_focus && option->state & State_HasFocus)
            rect.adjust(focus_line_width, focus_line_width, -focus_line_width, -focus_line_width);

        gtkPainter.paintShadow(gtkEntry, "entry", rect, option->state & State_Enabled ? 
                               GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE, 
                               GTK_SHADOW_IN, gtkEntry->style,
                               option->state & State_HasFocus ? QLS("focus") : QString());

        if (!interior_focus && option->state & State_HasFocus)
            gtkPainter.paintShadow(gtkEntry, "entry", option->rect, option->state & State_Enabled ? 
                                   GTK_STATE_ACTIVE : GTK_STATE_INSENSITIVE,
                                   GTK_SHADOW_IN, gtkEntry->style, QLS("GtkEntryShadowIn"));
    }
    break;

    case PE_PanelLineEdit:
        if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
            GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("GtkEntry"));
            if (panel->lineWidth > 0)
                drawPrimitive(PE_FrameLineEdit, option, painter, widget);
            uint resolve_mask = option->palette.resolve();
            QRect textRect = option->rect.adjusted(gtkEntry->style->xthickness, gtkEntry->style->ythickness,
                                                   -gtkEntry->style->xthickness, -gtkEntry->style->ythickness);

            if (widget && widget->testAttribute(Qt::WA_SetPalette) &&
                resolve_mask & (1 << QPalette::Base)) // Palette overridden by user
                painter->fillRect(textRect, option->palette.base().color());
            else
                gtkPainter.paintFlatBox( gtkEntry, "entry_bg", textRect,
                                         option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE, GTK_SHADOW_NONE, gtkEntry->style);
        }
        break;

    case PE_FrameTabWidget:
        if (const QStyleOptionTabWidgetFrame *frame = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>(option)) {
            GtkWidget *gtkNotebook = QGtk::gtkWidget(QLS("GtkNotebook"));
            style = gtkPainter.getStyle(gtkNotebook);
            gtkPainter.setAlphaSupport(false);
            GtkShadowType shadow = GTK_SHADOW_OUT;
            GtkStateType state = GTK_STATE_NORMAL; // Only state supported by gtknotebook
            if (const QTabWidget *tabwidget = qobject_cast<const QTabWidget*>(widget)) {
                // We should introduce QStyleOptionTabWidgetFrameV2 to obtain this information
                // No gap if we do not show the actual tabs
                QTabBar *tabBar = tabwidget->findChild<QTabBar*>();
                if (tabwidget->count() > 0 && tabBar->isVisible()) {
                    QRect tabRect = tabBar->tabRect(tabBar->currentIndex());
                    int begin = 0, size = 0;
                    GtkPositionType frameType = GTK_POS_TOP;
                    QTabBar::Shape shape = frame->shape;
                    if (shape == QTabBar::RoundedNorth || shape == QTabBar::RoundedSouth) {
                        begin = option->direction == Qt::LeftToRight ?
                                frame->leftCornerWidgetSize.width() + tabRect.left() :
                                frame->rect.width() - frame->tabBarSize.width() + tabRect.left()
                                - frame->rightCornerWidgetSize.width();
                        size = tabRect.width();
                        frameType = (shape == QTabBar::RoundedNorth) ? GTK_POS_TOP : GTK_POS_BOTTOM;
                    } else {
                        begin = frame->leftCornerWidgetSize.height() + tabRect.top();
                        size = tabRect.height();
                        frameType = (shape == QTabBar::RoundedWest) ? GTK_POS_LEFT : GTK_POS_RIGHT;
                    }
                    gtkPainter.paintBoxGap(gtkNotebook, "notebook",  option->rect, state, shadow, frameType,
                                            begin, size, style);
                    break; // done
                }
            }
            // Note this is only the fallback option
            gtkPainter.paintBox(gtkNotebook, "notebook",  option->rect, state, shadow, style);
        }
        break;

    case PE_PanelButtonCommand: {
        bool isDefault = false;
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton*>(option))
            isDefault = btn->features & QStyleOptionButton::DefaultButton;

        GtkStateType state = gtkPainter.gtkState(option);
        if (option->state & State_On || option->state & State_Sunken)
            state = GTK_STATE_ACTIVE;
        GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
        gint focusWidth, focusPad;
        gboolean interiorFocus = false;
        QGtk::gtk_widget_style_get (gtkButton,
                                "focus-line-width", &focusWidth,
                                "focus-padding", &focusPad,
                                "interior-focus", &interiorFocus, NULL);

        style = gtkButton->style;

        QRect buttonRect = option->rect;

        QString key;
        if (isDefault) {
            key += QLS("def");
            GTK_WIDGET_SET_FLAGS(gtkButton, GTK_HAS_DEFAULT);
            gtkPainter.paintBox(gtkButton, "buttondefault", buttonRect, state, GTK_SHADOW_IN,
                                style, isDefault ? QLS("d") : QString());
        } else
            GTK_WIDGET_UNSET_FLAGS(gtkButton, GTK_HAS_DEFAULT);

        bool hasFocus = option->state & State_HasFocus;

        if (hasFocus) {
            key += QLS("def");
            GTK_WIDGET_SET_FLAGS(gtkButton, GTK_HAS_FOCUS);

        } else {
            GTK_WIDGET_UNSET_FLAGS(gtkButton, GTK_HAS_FOCUS);
        }

        if (!interiorFocus)
            buttonRect = buttonRect.adjusted(focusWidth, focusWidth, -focusWidth, -focusWidth);

        GtkShadowType shadow = (option->state & State_Sunken || option->state & State_On ) ?
                               GTK_SHADOW_IN : GTK_SHADOW_OUT;

        gtkPainter.paintBox(gtkButton, "button", buttonRect, state, shadow,
                            style, key);
    }
    break;

    case PE_IndicatorRadioButton: {
        GtkShadowType shadow = GTK_SHADOW_OUT;
        GtkStateType state = gtkPainter.gtkState(option);

        if (option->state & State_Sunken)
            state = GTK_STATE_ACTIVE;

        if (option->state & State_NoChange)
            shadow = GTK_SHADOW_ETCHED_IN;
        else if (option->state & State_On)
            shadow = GTK_SHADOW_IN;
        else
            shadow = GTK_SHADOW_OUT;

        GtkWidget *gtkRadioButton = QGtk::gtkWidget(QLS("GtkRadioButton"));
        gint spacing;
        QGtk::gtk_widget_style_get(gtkRadioButton, "indicator-spacing", &spacing, NULL);
        QRect buttonRect = option->rect.adjusted(spacing, spacing, -spacing, -spacing);
        gtkPainter.setClipRect(option->rect);
        // ### Note: Ubuntulooks breaks when the proper widget is passed
        //           Murrine engine requires a widget not to get RGBA check - warnings
        GtkWidget *gtkCheckButton = QGtk::gtkWidget(QLS("GtkCheckButton"));
        gtkPainter.paintOption(gtkCheckButton , buttonRect, state, shadow, gtkRadioButton->style, QLS("radiobutton"));

    }
    break;

    case PE_IndicatorCheckBox: {
        GtkShadowType shadow = GTK_SHADOW_OUT;
        GtkStateType state = gtkPainter.gtkState(option);

        if (option->state & State_Sunken)
            state = GTK_STATE_ACTIVE;

        if (option->state & State_NoChange)
            shadow = GTK_SHADOW_ETCHED_IN;
        else if (option->state & State_On)
            shadow = GTK_SHADOW_IN;
        else
            shadow = GTK_SHADOW_OUT;

        int spacing;

        GtkWidget *gtkCheckButton = QGtk::gtkWidget(QLS("GtkCheckButton"));

        // Some styles such as aero-clone assume they can paint in the spacing area
        gtkPainter.setClipRect(option->rect);

        QGtk::gtk_widget_style_get(gtkCheckButton, "indicator-spacing", &spacing, NULL);

        QRect checkRect = option->rect.adjusted(spacing, spacing, -spacing, -spacing);

        gtkPainter.paintCheckbox(gtkCheckButton, checkRect, state, shadow, gtkCheckButton->style,
                                 QLS("checkbutton"));
    }
    break;

#ifndef QT_NO_TABBAR

    case PE_FrameTabBarBase:
        if (const QStyleOptionTabBarBase *tbb
                = qstyleoption_cast<const QStyleOptionTabBarBase *>(option)) {
            QRect tabRect = tbb->rect;
            QRegion region(tabRect);
            painter->save();
            painter->setPen(QPen(option->palette.dark().color().dark(110), 0));
            switch (tbb->shape) {

            case QTabBar::RoundedNorth:
                painter->drawLine(tabRect.topLeft(), tabRect.topRight());
                break;

            case QTabBar::RoundedWest:
                painter->drawLine(tabRect.left(), tabRect.top(), tabRect.left(), tabRect.bottom());
                break;

            case QTabBar::RoundedSouth:
                painter->drawLine(tbb->rect.left(), tbb->rect.bottom(),
                                  tabRect.right(), tabRect.bottom());
                break;

            case QTabBar::RoundedEast:
                painter->drawLine(tabRect.topRight(), tabRect.bottomRight());
                break;

            case QTabBar::TriangularNorth:
            case QTabBar::TriangularEast:
            case QTabBar::TriangularWest:
            case QTabBar::TriangularSouth:
                painter->restore();
                QWindowsStyle::drawPrimitive(element, option, painter, widget);
                return;
            }

            painter->restore();
        }
        return;

#endif // QT_NO_TABBAR

    case PE_Widget:
        break;

    default:
        QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
    }
}

/*!
    \reimp
*/
void QGtkStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,

                                   QPainter *painter, const QWidget *widget) const
{
    if (!QGtk::isThemeAvailable()) {
        QCleanlooksStyle::drawComplexControl(control, option, painter, widget);
        return;
    }

    GtkStyle* style = QGtk::gtkStyle();
    QGtkPainter gtkPainter(painter);
    QColor button = option->palette.button().color();
    QColor dark;
    QColor grooveColor;
    QColor darkOutline;
    dark.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*1.9)),
                qMin(255, (int)(button.value()*0.7)));
    grooveColor.setHsv(button.hue(),
                       qMin(255, (int)(button.saturation()*2.6)),
                       qMin(255, (int)(button.value()*0.9)));
    darkOutline.setHsv(button.hue(),
                       qMin(255, (int)(button.saturation()*3.0)),
                       qMin(255, (int)(button.value()*0.6)));

    QColor alphaCornerColor;

    if (widget)
        alphaCornerColor = mergedColors(option->palette.color(widget->backgroundRole()), darkOutline);
    else
        alphaCornerColor = mergedColors(option->palette.background().color(), darkOutline);

    QPalette palette = option->palette;

    switch (control) {

    case CC_TitleBar:
        if (const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(option)) {
            // Since this is drawn by metacity and not Gtk we
            // have to rely on Cleanlooks for a fallback
            QStyleOptionTitleBar copyOpt = *tb;
            QPalette pal = copyOpt.palette;
            // Bg color is closer to the window selection than
            // the base selection color
            GdkColor gdkBg = style->bg[GTK_STATE_SELECTED];
            QColor bgColor(gdkBg.red>>8, gdkBg.green>>8, gdkBg.blue>>8);
            pal.setBrush(QPalette::Active, QPalette::Highlight, bgColor);
            copyOpt.palette = pal;
            QCleanlooksStyle::drawComplexControl(control, &copyOpt, painter, widget);
        }
        break;

#ifndef QT_NO_GROUPBOX

    case CC_GroupBox:
        painter->save();

        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            QRect textRect = subControlRect(CC_GroupBox, groupBox, SC_GroupBoxLabel, widget);
            QRect checkBoxRect = subControlRect(CC_GroupBox, groupBox, SC_GroupBoxCheckBox, widget);
            // Draw title

            if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
                // Draw prelight background
                GtkWidget *gtkCheckButton = QGtk::gtkWidget(QLS("GtkCheckButton"));

                if (option->state & State_MouseOver) {
                    QRect bgRect = textRect | checkBoxRect;
                    gtkPainter.paintFlatBox(gtkCheckButton, "checkbutton", bgRect.adjusted(0, 0, 0, -2),
                                            GTK_STATE_PRELIGHT, GTK_SHADOW_ETCHED_OUT, gtkCheckButton->style);
                }

                if (!groupBox->text.isEmpty()) {
                    int alignment = int(groupBox->textAlignment);
                    if (!styleHint(QStyle::SH_UnderlineShortcut, option, widget))
                        alignment |= Qt::TextHideMnemonic;
                    QColor textColor = groupBox->textColor; // Note: custom textColor is currently ignored
                    int labelState = GTK_STATE_INSENSITIVE;

                    if (option->state & State_Enabled)
                        labelState = (option->state & State_MouseOver) ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL;

                    GdkColor gdkText = gtkCheckButton->style->fg[labelState];
                    textColor = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
                    painter->setPen(textColor);
                    QFont font = painter->font();
                    font.setBold(true);
                    painter->setFont(font);
                    painter->drawText(textRect, Qt::TextShowMnemonic | Qt::AlignLeft| alignment, groupBox->text);

                    if (option->state & State_HasFocus)
                        gtkPainter.paintFocus( NULL, "tab", textRect.adjusted(-4, -1, 0, -3), GTK_STATE_ACTIVE, style);
                }
            }

            if (groupBox->subControls & SC_GroupBoxCheckBox) {
                QStyleOptionButton box;
                box.QStyleOption::operator=(*groupBox);
                box.rect = checkBoxRect;
                drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
            }
        }

        painter->restore();
        break;
#endif // QT_NO_GROUPBOX

#ifndef QT_NO_COMBOBOX

    case CC_ComboBox:
        // See: http://live.gnome.org/GnomeArt/Tutorials/GtkThemes/GtkComboBox
        // and http://live.gnome.org/GnomeArt/Tutorials/GtkThemes/GtkComboBoxEntry
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            bool sunken = comboBox->state & State_On; // play dead, if combobox has no items
            BEGIN_STYLE_PIXMAPCACHE(QString::fromLatin1("cb-%0-%1").arg(sunken).arg(comboBox->editable));
            QGtkPainter gtkCachedPainter(p);
            gtkCachedPainter.setUsePixmapCache(false); // cached externally

            bool isEnabled = (comboBox->state & State_Enabled);
            bool focus = isEnabled && (comboBox->state & State_HasFocus);
            QColor buttonShadow = option->palette.dark().color();
            GtkStateType state = gtkPainter.gtkState(option);
            int appears_as_list = !styleHint(QStyle::SH_ComboBox_Popup, comboBox, widget);
            QPixmap cache;
            QString pixmapName;
            QStyleOptionComboBox comboBoxCopy = *comboBox;
            comboBoxCopy.rect = option->rect;

            bool reverse = (option->direction == Qt::RightToLeft);
            QRect rect = option->rect;
            QRect arrowButtonRect = subControlRect(CC_ComboBox, &comboBoxCopy,
                                                   SC_ComboBoxArrow, widget);
            QRect editRect = subControlRect(CC_ComboBox, &comboBoxCopy,
                                            SC_ComboBoxEditField, widget);

            GtkShadowType shadow = (option->state & State_Sunken || option->state & State_On ) ?
                                   GTK_SHADOW_IN : GTK_SHADOW_OUT;
            QString comboBoxPath = QLS(comboBox->editable ? "GtkComboBoxEntry" : "GtkComboBox");

            // We use the gtk widget to position arrows and separators for us
            GtkWidget *gtkCombo = QGtk::gtkWidget(comboBoxPath);
            GtkAllocation geometry = {0, 0, option->rect.width(), option->rect.height()};
            QGtk::gtk_widget_set_direction(gtkCombo, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
            QGtk::gtk_widget_size_allocate(gtkCombo, &geometry);

            QString buttonPath = comboBoxPath + QLS(".GtkToggleButton");
            GtkWidget *gtkToggleButton = QGtk::gtkWidget(buttonPath);
            QGtk::gtk_widget_set_direction(gtkToggleButton, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
            if (gtkToggleButton && (appears_as_list || comboBox->editable)) {
                // Draw the combo box as a line edit with a button next to it
                if (comboBox->editable || appears_as_list) {
                    GtkStateType frameState = (state == GTK_STATE_PRELIGHT) ? GTK_STATE_NORMAL : state;
                    QString entryPath = QLS(comboBox->editable ? "GtkComboBoxEntry.GtkEntry" : "GtkComboBox.GtkFrame");
                    GtkWidget *gtkEntry = QGtk::gtkWidget(entryPath);
                    QGtk::gtk_widget_set_direction(gtkEntry, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
                    QRect frameRect = option->rect;

                    if (reverse)
                        frameRect.setLeft(arrowButtonRect.right());
                    else
                        frameRect.setRight(arrowButtonRect.left());

                    // Required for inner blue highlight with clearlooks
                    if (focus) {
                        GTK_WIDGET_SET_FLAGS(gtkEntry, GTK_HAS_FOCUS);
                        GTK_WIDGET_SET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);

                    } else {
                        GTK_WIDGET_UNSET_FLAGS(gtkEntry, GTK_HAS_FOCUS);
                        GTK_WIDGET_UNSET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);
                    }

                    // Fill the line edit background
                    // We could have used flat_box with "entry_bg" but that is probably not worth the overhead
                    uint resolve_mask = option->palette.resolve();
                    int xt = gtkEntry->style->xthickness;
                    int yt = gtkEntry->style->ythickness;
                    QRect contentRect = frameRect.adjusted(xt, yt, -xt, -yt);
                    if (widget && widget->testAttribute(Qt::WA_SetPalette) &&
                        resolve_mask & (1 << QPalette::Base)) // Palette overridden by user
                        p->fillRect(contentRect, option->palette.base().color());
                    else {
                        gtkCachedPainter.paintFlatBox(gtkEntry, "entry_bg", contentRect,
                                                option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE,
                                                GTK_SHADOW_NONE, style, entryPath + QString::number(focus));
                    }

                    gtkCachedPainter.paintShadow(gtkEntry, comboBox->editable ? "entry" : "frame", frameRect, frameState,
                                           GTK_SHADOW_IN, gtkEntry->style, entryPath +
                                           QString::number(focus) + QString::number(comboBox->editable) +
                                           QString::number(option->direction));
                }

                GtkStateType buttonState = GTK_STATE_NORMAL;

                if (!(option->state & State_Enabled))
                    buttonState = GTK_STATE_INSENSITIVE;
                else if (option->state & State_Sunken || option->state & State_On)
                    buttonState = GTK_STATE_ACTIVE;
                else if (option->state & State_MouseOver && comboBox->activeSubControls & SC_ComboBoxArrow)
                    buttonState = GTK_STATE_PRELIGHT;

                QRect buttonrect = QRect(gtkToggleButton->allocation.x, gtkToggleButton->allocation.y,
                                         gtkToggleButton->allocation.width, gtkToggleButton->allocation.height);

                Q_ASSERT(gtkToggleButton);
                gtkCachedPainter.paintBox( gtkToggleButton, "button", arrowButtonRect, buttonState,
                                     shadow, gtkToggleButton->style, buttonPath +
                                     QString::number(focus) + QString::number(option->direction));

            } else {
                // Draw combo box as a button
                QRect buttonRect = option->rect;

                if (focus) { // Clearlooks actually check the widget for the default state
                    GTK_WIDGET_SET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);

                } else {
                    GTK_WIDGET_UNSET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);
                }

                gtkCachedPainter.paintBox(gtkToggleButton, "button",
                                    buttonRect, state,
                                    shadow, gtkToggleButton->style,
                                    buttonPath + QString::number(focus));
                // Draw the separator between label and arrows
                QString vSeparatorPath = buttonPath + QLS(".GtkHBox.GtkVSeparator");

                if (GtkWidget *gtkVSeparator = QGtk::gtkWidget(vSeparatorPath)) {
                    QRect vLineRect(gtkVSeparator->allocation.x,
                                    gtkVSeparator->allocation.y,
                                    gtkVSeparator->allocation.width,
                                    gtkVSeparator->allocation.height);

                    gtkCachedPainter.paintVline( gtkVSeparator, "vseparator",
                                           vLineRect, state, gtkVSeparator->style,
                                           0, vLineRect.height(), 0,  vSeparatorPath);


                    gint interiorFocus = true;
                    QGtk::gtk_widget_style_get(gtkToggleButton, "interior-focus", &interiorFocus, NULL);
                    int xt = interiorFocus ? gtkToggleButton->style->xthickness : 0;
                    int yt = interiorFocus ? gtkToggleButton->style->ythickness : 0;
                    if ((focus && (option->state & State_KeyboardFocusChange)))
                        gtkCachedPainter.paintFocus(gtkToggleButton, "button",
                                              option->rect.adjusted(xt, yt, -xt, -yt),
                                              option->state & State_Sunken ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL,
                                              gtkToggleButton->style);
                }
            }

            if (comboBox->subControls & SC_ComboBoxArrow) {
                if (!isEnabled)
                    state = GTK_STATE_INSENSITIVE;
                else if (sunken)
                    state = GTK_STATE_ACTIVE;
                else if (option->state & State_MouseOver)
                    state = GTK_STATE_PRELIGHT;
                else
                    state = GTK_STATE_NORMAL;

                QString arrowPath = comboBoxPath + QLS(appears_as_list ? ".GtkToggleButton.GtkArrow"
                                                    : ".GtkToggleButton.GtkHBox.GtkArrow");

                GtkWidget *gtkArrow = QGtk::gtkWidget(arrowPath);
                gfloat scale = 0.7;
                gint minSize = 15;
                QRect arrowWidgetRect;

                if (gtkArrow && !QGtk::gtk_check_version(2, 12, 0)) {
                    QGtk::gtk_widget_style_get(gtkArrow, "arrow-scaling", &scale, NULL);
                    QGtk::gtk_widget_style_get(gtkCombo, "arrow-size", &minSize, NULL);
                }
                if (gtkArrow) {
                    arrowWidgetRect = QRect(gtkArrow->allocation.x, gtkArrow->allocation.y,
                                            gtkArrow->allocation.width, gtkArrow->allocation.height);
                    style = gtkArrow->style;
                }

                // Note that for some reason the arrow-size is not properly respected with Hildon
                // Hence we enforce the minimum "arrow-size" ourselves
                int arrowSize = qMax(qMin(rect.height() - gtkCombo->style->ythickness * 2, minSize),
                                     qMin(arrowWidgetRect.width(), arrowWidgetRect.height()));
                QRect arrowRect(0, 0, static_cast<int>(arrowSize * scale), static_cast<int>(arrowSize * scale));

                arrowRect.moveCenter(arrowWidgetRect.center());

                if (sunken) {
                    int xoff, yoff;
                    GtkWidget *gtkButton = QGtk::gtkWidget(comboBoxPath + QLS(".GtkToggleButton"));
                    QGtk::gtk_widget_style_get(gtkButton, "child-displacement-x", &xoff, NULL);
                    QGtk::gtk_widget_style_get(gtkButton, "child-displacement-y", &yoff, NULL);
                    arrowRect = arrowRect.adjusted(xoff, yoff, xoff, yoff);
                }

                // Some styles such as Nimbus paint outside the arrowRect
                // hence we have provide the whole widget as the cliprect
                if (gtkArrow) {
                    gtkCachedPainter.setClipRect(option->rect);
                    gtkCachedPainter.paintArrow( gtkArrow, "arrow", arrowRect,
                                           GTK_ARROW_DOWN, state, GTK_SHADOW_NONE, TRUE,
                                           style, arrowPath + QString::number(option->direction));
                }
            }
            END_STYLE_PIXMAPCACHE;
        }
        break;
#endif // QT_NO_COMBOBOX
#ifndef QT_NO_TOOLBUTTON

    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolbutton
                = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            QRect button, menuarea;
            button = subControlRect(control, toolbutton, SC_ToolButton, widget);
            menuarea = subControlRect(control, toolbutton, SC_ToolButtonMenu, widget);
            State bflags = toolbutton->state & ~(State_Sunken | State_MouseOver);

            if (bflags & State_AutoRaise)
                if (!(bflags & State_MouseOver))
                    bflags &= ~State_Raised;

            State mflags = bflags;

            if (toolbutton->state & State_Sunken) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    bflags |= State_Sunken;
                if (toolbutton->activeSubControls & SC_ToolButtonMenu)
                    mflags |= State_Sunken;
            } else if (toolbutton->state & State_MouseOver) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    bflags |= State_MouseOver;
                if (toolbutton->activeSubControls & SC_ToolButtonMenu)
                    mflags |= State_MouseOver;
            }

            QStyleOption tool(0);

            tool.palette = toolbutton->palette;

            if (toolbutton->subControls & SC_ToolButton) {
                if (bflags & (State_Sunken | State_On | State_Raised | State_MouseOver)) {
                    tool.rect = button;
                    tool.state = bflags;
                    drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);
                }
            }

            bool drawMenuArrow = toolbutton->features & QStyleOptionToolButton::HasMenu &&
                                 !(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup);
            int popupArrowSize = drawMenuArrow ? 7 : 0;

            if (toolbutton->state & State_HasFocus) {
                QStyleOptionFocusRect fr;
                fr.QStyleOption::operator=(*toolbutton);
                fr.rect = subControlRect(CC_ToolButton, toolbutton, SC_ToolButton, widget);
                fr.rect.adjust(1, 1, -1, -1);
                drawPrimitive(PE_FrameFocusRect, &fr, painter, widget);
            }

            QStyleOptionToolButton label = *toolbutton;
            label.state = bflags;
            GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
            QPalette pal = toolbutton->palette;
            if (option->state & State_Enabled && 
                option->state & State_MouseOver && !(widget && widget->testAttribute(Qt::WA_SetPalette))) {
                GdkColor gdkText = gtkButton->style->fg[GTK_STATE_PRELIGHT];
                QColor textColor = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
                pal.setBrush(QPalette::All, QPalette::ButtonText, textColor);
                label.palette = pal;
            }
            label.rect = button.adjusted(style->xthickness, style->ythickness,
                                        -style->xthickness - popupArrowSize, -style->ythickness);
            drawControl(CE_ToolButtonLabel, &label, painter, widget);

            if (toolbutton->subControls & SC_ToolButtonMenu) {
                tool.rect = menuarea;
                tool.state = mflags;
                if ((mflags & State_Enabled && (mflags & (State_Sunken | State_Raised | State_MouseOver))) || !(mflags & State_AutoRaise))
                    drawPrimitive(PE_IndicatorButtonDropDown, &tool, painter, widget);

                drawPrimitive(PE_IndicatorArrowDown, &tool, painter, widget);

            } else if (drawMenuArrow) {
                QRect ir = toolbutton->rect;
                QStyleOptionToolButton newBtn = *toolbutton;
                newBtn.rect = QRect(ir.right() - popupArrowSize - style->xthickness - 3, ir.height()/2 - 1, popupArrowSize, popupArrowSize);
                drawPrimitive(PE_IndicatorArrowDown, &newBtn, painter, widget);
            }
        }
        break;

#endif // QT_NO_TOOLBUTTON
#ifndef QT_NO_SCROLLBAR

    case CC_ScrollBar:
        if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            GtkWidget *gtkHScrollBar = QGtk::gtkWidget(QLS("GtkHScrollbar"));
            GtkWidget *gtkVScrollBar = QGtk::gtkWidget(QLS("GtkVScrollbar"));

            // Fill background in case the scrollbar is partially transparent
            painter->fillRect(option->rect, option->palette.background());

            QRect rect = scrollBar->rect;
            QRect scrollBarSubLine = subControlRect(control, scrollBar, SC_ScrollBarSubLine, widget);
            QRect scrollBarAddLine = subControlRect(control, scrollBar, SC_ScrollBarAddLine, widget);
            QRect scrollBarSlider = subControlRect(control, scrollBar, SC_ScrollBarSlider, widget);
            QRect grooveRect = subControlRect(control, scrollBar, SC_ScrollBarGroove, widget);
            bool horizontal = scrollBar->orientation == Qt::Horizontal;
            GtkWidget * scrollbarWidget = horizontal ? gtkHScrollBar : gtkVScrollBar;
            style = scrollbarWidget->style;
            gboolean trough_under_steppers = true;
            gboolean trough_side_details = false;
            gboolean stepper_size = 14;
            gint trough_border = 1;
            if (!QGtk::gtk_check_version(2, 10, 0)) {
                QGtk::gtk_widget_style_get((GtkWidget*)(scrollbarWidget),
                                           "trough-border",   &trough_border,
                                           "trough-side-details",   &trough_side_details,
                                           "trough-under-steppers", &trough_under_steppers,
                                           "stepper-size",          &stepper_size, NULL);
            }
            if (trough_under_steppers) {
                scrollBarAddLine.adjust(trough_border, trough_border, -trough_border, -trough_border);
                scrollBarSubLine.adjust(trough_border, trough_border, -trough_border, -trough_border);
                scrollBarSlider.adjust(horizontal ? -trough_border : 0, horizontal ? 0 : -trough_border,
                                       horizontal ? trough_border : 0, horizontal ? 0 : trough_border);
            }

            // Some styles check the position of scrollbars in order to determine
            // if lines should be painted when the scrollbar is in max or min positions.
            int maximum = 2;
            int fakePos = 0;
            bool reverse = (option->direction == Qt::RightToLeft);
            if (scrollBar->minimum == scrollBar->maximum)
                maximum = 0;
            if (scrollBar->sliderPosition == scrollBar->maximum)
                fakePos = maximum;
            else if (scrollBar->sliderPosition > scrollBar->minimum)
                fakePos = maximum - 1;
            GtkObject *adjustment =  QGtk::gtk_adjustment_new(fakePos, 0, maximum, 0, 0, 0);

            if (horizontal)
                QGtk::gtk_range_set_adjustment((GtkRange*)(gtkHScrollBar), (GtkAdjustment*)(adjustment));
            else
                QGtk::gtk_range_set_adjustment((GtkRange*)(gtkVScrollBar), (GtkAdjustment*)(adjustment));

            if (scrollBar->subControls & SC_ScrollBarGroove) {
                GtkStateType state = GTK_STATE_ACTIVE;

                if (!(option->state & State_Enabled))
                    state = GTK_STATE_INSENSITIVE;

                if (trough_under_steppers)
                    grooveRect = option->rect;

                gtkPainter.paintBox( scrollbarWidget, "trough", grooveRect, state, GTK_SHADOW_IN, style);
            }

            //paint slider
            if (scrollBar->subControls & SC_ScrollBarSlider) {
                GtkStateType state = GTK_STATE_NORMAL;

                if (!(option->state & State_Enabled))
                    state = GTK_STATE_INSENSITIVE;
                else if (option->state & State_MouseOver && (scrollBar->activeSubControls & SC_ScrollBarSlider))
                    state = GTK_STATE_PRELIGHT;

                GtkShadowType shadow = GTK_SHADOW_OUT;

                if (trough_under_steppers) {
                    if (!horizontal)
                        scrollBarSlider.adjust(trough_border, 0, -trough_border, 0);
                    else
                        scrollBarSlider.adjust(0, trough_border, 0, -trough_border);
                }

                gtkPainter.paintSlider( scrollbarWidget, "slider", scrollBarSlider, state, shadow, style,

                                        horizontal ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL, QString(QLS("%0%1")).arg(fakePos).arg(maximum));
            }

            if (scrollBar->subControls & SC_ScrollBarAddLine) {
                gtkVScrollBar->allocation.y = scrollBarAddLine.top();
                gtkVScrollBar->allocation.height = scrollBarAddLine.height() - rect.height() + 6;
                gtkHScrollBar->allocation.x = scrollBarAddLine.right();
                gtkHScrollBar->allocation.width = scrollBarAddLine.width() - rect.width();

                GtkShadowType shadow = GTK_SHADOW_OUT;
                GtkStateType state = GTK_STATE_NORMAL;

                if (!(option->state & State_Enabled) || (fakePos == maximum))
                    state = GTK_STATE_INSENSITIVE;
                else if (option->state & State_Sunken && (scrollBar->activeSubControls & SC_ScrollBarAddLine)) {
                    state = GTK_STATE_ACTIVE;
                    shadow = GTK_SHADOW_IN;

                } else if (option->state & State_MouseOver && (scrollBar->activeSubControls & SC_ScrollBarAddLine))
                    state = GTK_STATE_PRELIGHT;

                gtkPainter.paintBox( scrollbarWidget,
                                     horizontal ? "hscrollbar" : "vscrollbar", scrollBarAddLine,
                                     state, shadow, style, QLS("add"));

                gtkPainter.paintArrow( scrollbarWidget, horizontal ? "hscrollbar" : "vscrollbar", scrollBarAddLine.adjusted(4, 4, -4, -4),
                                       horizontal ? (reverse ? GTK_ARROW_LEFT : GTK_ARROW_RIGHT) :
                                       GTK_ARROW_DOWN, state, GTK_SHADOW_NONE, FALSE, style);
            }

            if (scrollBar->subControls & SC_ScrollBarSubLine) {
                gtkVScrollBar->allocation.y = 0;
                gtkVScrollBar->allocation.height = scrollBarSubLine.height();
                gtkHScrollBar->allocation.x = 0;
                gtkHScrollBar->allocation.width = scrollBarSubLine.width();

                GtkShadowType shadow = GTK_SHADOW_OUT;
                GtkStateType state = GTK_STATE_NORMAL;

                if (!(option->state & State_Enabled) || (fakePos == 0))
                    state = GTK_STATE_INSENSITIVE;
                else if (option->state & State_Sunken && (scrollBar->activeSubControls & SC_ScrollBarSubLine)) {
                    shadow = GTK_SHADOW_IN;
                    state = GTK_STATE_ACTIVE;

                } else if (option->state & State_MouseOver && (scrollBar->activeSubControls & SC_ScrollBarSubLine))
                    state = GTK_STATE_PRELIGHT;

                gtkPainter.paintBox(scrollbarWidget, horizontal ? "hscrollbar" : "vscrollbar", scrollBarSubLine,
                                    state, shadow, style, QLS("sub"));

                gtkPainter.paintArrow(scrollbarWidget, horizontal ? "hscrollbar" : "vscrollbar", scrollBarSubLine.adjusted(4, 4, -4, -4),
                                      horizontal ? (reverse ? GTK_ARROW_RIGHT : GTK_ARROW_LEFT) :
                                      GTK_ARROW_UP, state, GTK_SHADOW_NONE, FALSE, style);
            }
        }
        break;

#endif //QT_NO_SCROLLBAR
#ifndef QT_NO_SPINBOX

    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            GtkWidget *gtkSpinButton = QGtk::gtkWidget(QLS("GtkSpinButton"));
            bool isEnabled = (spinBox->state & State_Enabled);
            bool hover = isEnabled && (spinBox->state & State_MouseOver);
            bool sunken = (spinBox->state & State_Sunken);
            bool upIsActive = (spinBox->activeSubControls == SC_SpinBoxUp);
            bool downIsActive = (spinBox->activeSubControls == SC_SpinBoxDown);
            bool reverse = (spinBox->direction == Qt::RightToLeft);

            //### Move this to subControlRect
            QRect upRect = subControlRect(CC_SpinBox, option, SC_SpinBoxUp, widget);
            upRect.setTop(option->rect.top());

            if (reverse)
                upRect.setLeft(option->rect.left());
            else
                upRect.setRight(option->rect.right());

            QRect editRect = subControlRect(CC_SpinBox, option, SC_SpinBoxEditField, widget);
            QRect downRect = subControlRect(CC_SpinBox, option, SC_SpinBoxDown, widget);
            downRect.setBottom(option->rect.bottom());

            if (reverse)
                downRect.setLeft(option->rect.left());
            else
                downRect.setRight(option->rect.right());

            QRect buttonRect = upRect | downRect;
            QRect editArea = option->rect;

            if (reverse)
                editArea.setLeft(upRect.right());
            else
                editArea.setRight(upRect.left());

            if (spinBox->frame) {
                GtkShadowType shadow = GTK_SHADOW_OUT;
                GtkStateType state = gtkPainter.gtkState(option);

                if (!(option->state & State_Enabled))
                    state = GTK_STATE_INSENSITIVE;
                else if (option->state & State_HasFocus)
                    state = GTK_STATE_NORMAL;
                else if (state == GTK_STATE_PRELIGHT)
                    state = GTK_STATE_NORMAL;

                shadow = GTK_SHADOW_IN;
                style = gtkPainter.getStyle(gtkSpinButton);

                if (option->state & State_HasFocus)
                    GTK_WIDGET_SET_FLAGS(gtkSpinButton, GTK_HAS_FOCUS);
                else
                    GTK_WIDGET_UNSET_FLAGS(gtkSpinButton, GTK_HAS_FOCUS);

                QString key;

                if (option->state & State_HasFocus)
                    key = QLS("f");

                uint resolve_mask = option->palette.resolve();

                if (resolve_mask & (1 << QPalette::Base)) // Palette overridden by user
                    painter->fillRect(editRect, option->palette.base().color());
                else
                    gtkPainter.paintFlatBox(gtkSpinButton, "entry_bg", editArea.adjusted(style->xthickness, style->ythickness,
                                            -style->xthickness, -style->ythickness),
                                            option->state & State_Enabled ?
                                            GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE, GTK_SHADOW_NONE, style, key);

                gtkPainter.paintShadow(gtkSpinButton, "entry", editArea, state, GTK_SHADOW_IN, gtkSpinButton->style, key);
                gtkPainter.paintBox(gtkSpinButton, "spinbutton", buttonRect, state, GTK_SHADOW_IN, style, key);

                upRect.setSize(downRect.size());
                if (!(option->state & State_Enabled))
                    gtkPainter.paintBox( gtkSpinButton, "spinbutton_up", upRect, GTK_STATE_INSENSITIVE, GTK_SHADOW_IN, style, key);
                else if (upIsActive && sunken)
                    gtkPainter.paintBox( gtkSpinButton, "spinbutton_up", upRect, GTK_STATE_ACTIVE, GTK_SHADOW_IN, style, key);
                else if (upIsActive && hover)
                    gtkPainter.paintBox( gtkSpinButton, "spinbutton_up", upRect, GTK_STATE_PRELIGHT, GTK_SHADOW_OUT, style, key);
                else
                    gtkPainter.paintBox( gtkSpinButton, "spinbutton_up", upRect, GTK_STATE_NORMAL, GTK_SHADOW_OUT, style, key);

                if (!(option->state & State_Enabled))
                    gtkPainter.paintBox( gtkSpinButton, "spinbutton_down", downRect, GTK_STATE_INSENSITIVE, GTK_SHADOW_IN, style, key);
                else if (downIsActive && sunken)
                    gtkPainter.paintBox( gtkSpinButton, "spinbutton_down", downRect, GTK_STATE_ACTIVE, GTK_SHADOW_IN, style, key);
                else if (downIsActive && hover)
                    gtkPainter.paintBox( gtkSpinButton, "spinbutton_down", downRect, GTK_STATE_PRELIGHT, GTK_SHADOW_OUT, style, key);
                else
                    gtkPainter.paintBox( gtkSpinButton, "spinbutton_down", downRect, GTK_STATE_NORMAL, GTK_SHADOW_OUT, style, key);
            }

            if (spinBox->buttonSymbols == QAbstractSpinBox::PlusMinus) {
                int centerX = upRect.center().x();
                int centerY = upRect.center().y();
                // plus/minus

                if (spinBox->activeSubControls == SC_SpinBoxUp && sunken) {
                    painter->drawLine(1 + centerX - 2, 1 + centerY, 1 + centerX + 2, 1 + centerY);
                    painter->drawLine(1 + centerX, 1 + centerY - 2, 1 + centerX, 1 + centerY + 2);

                } else {
                    painter->drawLine(centerX - 2, centerY, centerX + 2, centerY);
                    painter->drawLine(centerX, centerY - 2, centerX, centerY + 2);
                }
                centerX = downRect.center().x();
                centerY = downRect.center().y();

                if (spinBox->activeSubControls == SC_SpinBoxDown && sunken) {
                    painter->drawLine(1 + centerX - 2, 1 + centerY, 1 + centerX + 2, 1 + centerY);
                } else {
                    painter->drawLine(centerX - 2, centerY, centerX + 2, centerY);
                }

            } else {
                int size = spinboxArrowSize();
                int w = size / 2 - 1;
                w -= w % 2 - 1; // force odd
                int h = (w + 1)/2;
                QRect arrowRect(0, 0, w, h);
                arrowRect.moveCenter(upRect.center());
                // arrows
                GtkStateType state = GTK_STATE_NORMAL;

                if (!(option->state & State_Enabled) || !(spinBox->stepEnabled & QAbstractSpinBox::StepUpEnabled))
                    state = GTK_STATE_INSENSITIVE;

                gtkPainter.paintArrow( gtkSpinButton, "spinbutton", arrowRect, GTK_ARROW_UP, state,
                                       GTK_SHADOW_NONE, FALSE, style);

                arrowRect.moveCenter(downRect.center());

                if (!(option->state & State_Enabled) || !(spinBox->stepEnabled & QAbstractSpinBox::StepDownEnabled))
                    state = GTK_STATE_INSENSITIVE;

                gtkPainter.paintArrow( gtkSpinButton, "spinbutton", arrowRect, GTK_ARROW_DOWN, state,
                                       GTK_SHADOW_NONE, FALSE, style);
            }
        }
        break;

#endif // QT_NO_SPINBOX

#ifndef QT_NO_SLIDER

    case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            GtkWidget *hScaleWidget = QGtk::gtkWidget(QLS("GtkHScale"));
            GtkWidget *vScaleWidget = QGtk::gtkWidget(QLS("GtkVScale"));

            QRect groove = subControlRect(CC_Slider, option, SC_SliderGroove, widget);
            QRect handle = subControlRect(CC_Slider, option, SC_SliderHandle, widget);
            QRect ticks = subControlRect(CC_Slider, option, SC_SliderTickmarks, widget);

            bool horizontal = slider->orientation == Qt::Horizontal;
            bool ticksAbove = slider->tickPosition & QSlider::TicksAbove;
            bool ticksBelow = slider->tickPosition & QSlider::TicksBelow;
            QColor activeHighlight = option->palette.color(QPalette::Normal, QPalette::Highlight);

            QPixmap cache;
            QBrush oldBrush = painter->brush();
            QPen oldPen = painter->pen();

            QColor shadowAlpha(Qt::black);
            shadowAlpha.setAlpha(10);
            QColor highlightAlpha(Qt::white);
            highlightAlpha.setAlpha(80);

            GtkWidget *scaleWidget = horizontal ? hScaleWidget : vScaleWidget;
            style = scaleWidget->style;

            if ((option->subControls & SC_SliderGroove) && groove.isValid()) {
                GtkObject *adjustment =  QGtk::gtk_adjustment_new(slider->sliderPosition,
                                         slider->minimum,
                                         slider->maximum,
                                         slider->singleStep,
                                         slider->singleStep,
                                         slider->pageStep);
                int outerSize;
                QGtk::gtk_range_set_adjustment ((GtkRange*)(scaleWidget), (GtkAdjustment*)(adjustment));
                QGtk::gtk_range_set_inverted((GtkRange*)(scaleWidget), !horizontal);
                QGtk::gtk_widget_style_get(scaleWidget, "trough-border", &outerSize, NULL);
                outerSize++;

                GtkStateType state = gtkPainter.gtkState(option);
                int focusFrameMargin = 2;
                QRect grooveRect = option->rect.adjusted(focusFrameMargin, outerSize + focusFrameMargin,
                                   -focusFrameMargin, -outerSize - focusFrameMargin);

                gtkPainter.paintBox( scaleWidget, "trough", grooveRect, state,
                                     GTK_SHADOW_IN, style, QString(QLS("p%0")).arg(slider->sliderPosition));

                gboolean trough_side_details = false; // Indicates if the upper or lower scale background differs
                if (!QGtk::gtk_check_version(2, 10, 0))
                    QGtk::gtk_widget_style_get((GtkWidget*)(scaleWidget), "trough-side-details",   &trough_side_details, NULL);

                if (trough_side_details && horizontal) { //### Vertical sliders look broken with this for some reason
                    QRect lowerGroove = grooveRect;
                    lowerGroove.setRight(handle.center().x());
                    gtkPainter.paintBox( scaleWidget, "trough-lower", lowerGroove, state,
                                         GTK_SHADOW_IN, style, QString(QLS("p%0")).arg(slider->sliderPosition));
                }

            }

            if (option->subControls & SC_SliderTickmarks) {
                painter->setPen(darkOutline);
                int tickSize = pixelMetric(PM_SliderTickmarkOffset, option, widget);
                int available = pixelMetric(PM_SliderSpaceAvailable, slider, widget);
                int interval = slider->tickInterval;

                if (interval <= 0) {
                    interval = slider->singleStep;

                    if (QStyle::sliderPositionFromValue(slider->minimum, slider->maximum, interval,
                                                        available)
                            - QStyle::sliderPositionFromValue(slider->minimum, slider->maximum,
                                                              0, available) < 3)
                        interval = slider->pageStep;
                }

                if (interval <= 0)
                    interval = 1;

                int v = slider->minimum;
                int len = pixelMetric(PM_SliderLength, slider, widget);
                while (v <= slider->maximum + 1) {
                    if (v == slider->maximum + 1 && interval == 1)
                        break;
                    const int v_ = qMin(v, slider->maximum);
                    int pos = sliderPositionFromValue(slider->minimum, slider->maximum,
                                                      v_, (horizontal
                                                           ? slider->rect.width()
                                                           : slider->rect.height()) - len,
                                                      slider->upsideDown) + len / 2;
                    int extra = 2 - ((v_ == slider->minimum || v_ == slider->maximum) ? 1 : 0);
                    if (horizontal) {
                        if (ticksAbove)
                            painter->drawLine(pos, slider->rect.top() + extra,
                                              pos, slider->rect.top() + tickSize);
                        if (ticksBelow)
                            painter->drawLine(pos, slider->rect.bottom() - extra,
                                              pos, slider->rect.bottom() - tickSize);

                    } else {
                        if (ticksAbove)
                            painter->drawLine(slider->rect.left() + extra, pos,
                                              slider->rect.left() + tickSize, pos);
                        if (ticksBelow)
                            painter->drawLine(slider->rect.right() - extra, pos,
                                              slider->rect.right() - tickSize, pos);
                    }

                    // In the case where maximum is max int
                    int nextInterval = v + interval;
                    if (nextInterval < v)
                        break;
                    v = nextInterval;
                }
            }

            // Draw slider handle
            if (option->subControls & SC_SliderHandle) {
                GtkShadowType shadow =  GTK_SHADOW_OUT;
                GtkStateType state = GTK_STATE_NORMAL;

                if (!(option->state & State_Enabled))
                    state = GTK_STATE_INSENSITIVE;
                else if (option->state & State_MouseOver && option->activeSubControls & SC_SliderHandle)
                    state = GTK_STATE_PRELIGHT;

                bool horizontal = option->state & State_Horizontal;

                if (slider->state & State_HasFocus) {
                    QStyleOptionFocusRect fropt;
                    fropt.QStyleOption::operator=(*slider);
                    fropt.rect = slider->rect.adjusted(-1, -1 ,1, 1);

                    if (horizontal) {
                        fropt.rect.setTop(handle.top() - 3);
                        fropt.rect.setBottom(handle.bottom() + 4);

                    } else {
                        fropt.rect.setLeft(handle.left() - 3);
                        fropt.rect.setRight(handle.right() + 3);
                    }
                    drawPrimitive(PE_FrameFocusRect, &fropt, painter, widget);
                }
                gtkPainter.paintSlider( scaleWidget, horizontal ? "hscale" : "vscale", handle, state, shadow, style,

                                        horizontal ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL);
            }
            painter->setBrush(oldBrush);
            painter->setPen(oldPen);
        }
        break;

#endif // QT_NO_SLIDER

    default:
        QCleanlooksStyle::drawComplexControl(control, option, painter, widget);

        break;
    }
}


/*!
    \reimp
*/
void QGtkStyle::drawControl(ControlElement element,
                            const QStyleOption *option,
                            QPainter *painter,
                            const QWidget *widget) const
{
    if (!QGtk::isThemeAvailable()) {
        QCleanlooksStyle::drawControl(element, option, painter, widget);
        return;
    }

    GtkStyle* style = QGtk::gtkStyle();
    QGtkPainter gtkPainter(painter);

    switch (element) {
    case CE_ProgressBarLabel:
        if (const QStyleOptionProgressBar *bar = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
            GtkWidget *gtkProgressBar = QGtk::gtkWidget(QLS("GtkProgressBar"));
            if (!gtkProgressBar)
                return;

            QRect leftRect;
            QRect rect = bar->rect;
            GdkColor gdkText = gtkProgressBar->style->fg[GTK_STATE_NORMAL];
            QColor textColor = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
            gdkText = gtkProgressBar->style->fg[GTK_STATE_PRELIGHT];
            QColor alternateTextColor= QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);

            painter->save();
            bool vertical = false, inverted = false;
            if (const QStyleOptionProgressBarV2 *bar2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(option)) {
                vertical = (bar2->orientation == Qt::Vertical);
                inverted = bar2->invertedAppearance;
            }
            if (vertical)
                rect = QRect(rect.left(), rect.top(), rect.height(), rect.width()); // flip width and height
            const int progressIndicatorPos = static_cast<int>((bar->progress - qint64(bar->minimum)) /
                                       qMax(double(1.0), double(qint64(bar->maximum) - qint64(bar->minimum))) * rect.width());
            if (progressIndicatorPos >= 0 && progressIndicatorPos <= rect.width())
                leftRect = QRect(rect.left(), rect.top(), progressIndicatorPos, rect.height());
            if (vertical)
                leftRect.translate(rect.width() - progressIndicatorPos, 0);

            bool flip = (!vertical && (((bar->direction == Qt::RightToLeft) && !inverted) ||
                                       ((bar->direction == Qt::LeftToRight) && inverted)));

            QRegion rightRect = rect;
            rightRect = rightRect.subtracted(leftRect);
            painter->setClipRegion(rightRect);
            painter->setPen(flip ? alternateTextColor : textColor);
            painter->drawText(rect, bar->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
            if (!leftRect.isNull()) {
                painter->setPen(flip ? textColor : alternateTextColor);
                painter->setClipRect(leftRect);
                painter->drawText(rect, bar->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
            }
            painter->restore();
        }
        break;
    case CE_PushButtonLabel:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QRect ir = button->rect;
            uint tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
            QPoint buttonShift;

            if (option->state & State_Sunken)
                buttonShift = QPoint(pixelMetric(PM_ButtonShiftHorizontal, option, widget),
                                     pixelMetric(PM_ButtonShiftVertical, option, widget));

            if (styleHint(SH_UnderlineShortcut, button, widget))
                tf |= Qt::TextShowMnemonic;
            else
                tf |= Qt::TextHideMnemonic;

            if (!button->icon.isNull()) {
                //Center both icon and text
                QPoint point;

                QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
                if (mode == QIcon::Normal && button->state & State_HasFocus)
                    mode = QIcon::Active;

                QIcon::State state = QIcon::Off;

                if (button->state & State_On)
                    state = QIcon::On;

                QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
                int w = pixmap.width();
                int h = pixmap.height();

                if (!button->text.isEmpty())
                    w += button->fontMetrics.boundingRect(option->rect, tf, button->text).width() + 4;

                point = QPoint(ir.x() + ir.width() / 2 - w / 2,
                               ir.y() + ir.height() / 2 - h / 2);

                if (button->direction == Qt::RightToLeft)
                    point.rx() += pixmap.width();

                painter->drawPixmap(visualPos(button->direction, button->rect, point + buttonShift), pixmap);

                if (button->direction == Qt::RightToLeft)
                    ir.translate(-point.x() - 2, 0);
                else
                    ir.translate(point.x() + pixmap.width() + 2, 0);

                // left-align text if there is
                if (!button->text.isEmpty())
                    tf |= Qt::AlignLeft;

            } else {
                tf |= Qt::AlignHCenter;
            }

            ir.translate(buttonShift);

            if (button->features & QStyleOptionButton::HasMenu)
                ir = ir.adjusted(0, 0, -pixelMetric(PM_MenuButtonIndicator, button, widget), 0);

            GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
            QPalette pal = button->palette;
            int labelState = GTK_STATE_INSENSITIVE;
            if (option->state & State_Enabled)
                labelState = (option->state & State_MouseOver && !(option->state & State_Sunken)) ?
                             GTK_STATE_PRELIGHT : GTK_STATE_NORMAL;

            GdkColor gdkText = gtkButton->style->fg[labelState];
            QColor textColor = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
            pal.setBrush(QPalette::ButtonText, textColor);
            drawItemText(painter, ir, tf, pal, (button->state & State_Enabled),
                         button->text, QPalette::ButtonText);
        }
        break;

    case CE_RadioButton: // Fall through
    case CE_CheckBox:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            bool isRadio = (element == CE_RadioButton);

            // Draw prelight background
            GtkWidget *gtkRadioButton = QGtk::gtkWidget(QLS("GtkRadioButton"));

            if (option->state & State_MouseOver) {
                gtkPainter.paintFlatBox(gtkRadioButton, "checkbutton", option->rect,
                                        GTK_STATE_PRELIGHT, GTK_SHADOW_ETCHED_OUT, gtkRadioButton->style);
            }

            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(isRadio ? SE_RadioButtonIndicator
                                         : SE_CheckBoxIndicator, btn, widget);
            drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox,
                          &subopt, painter, widget);
            subopt.rect = subElementRect(isRadio ? SE_RadioButtonContents
                                         : SE_CheckBoxContents, btn, widget);
            // Get label text color
            QPalette pal = subopt.palette;
            int labelState = GTK_STATE_INSENSITIVE;
            if (option->state & State_Enabled)
                labelState = (option->state & State_MouseOver) ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL;

            GdkColor gdkText = gtkRadioButton->style->fg[labelState];
            QColor textColor = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
            pal.setBrush(QPalette::WindowText, textColor);
            subopt.palette = pal;
            drawControl(isRadio ? CE_RadioButtonLabel : CE_CheckBoxLabel, &subopt, painter, widget);

            if (btn->state & State_HasFocus) {
                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*btn);
                fropt.rect = subElementRect(isRadio ? SE_RadioButtonFocusRect
                                            : SE_CheckBoxFocusRect, btn, widget);
                drawPrimitive(PE_FrameFocusRect, &fropt, painter, widget);
            }
        }
        break;

#ifndef QT_NO_COMBOBOX

    case CE_ComboBoxLabel:
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);
            bool appearsAsList = !styleHint(QStyle::SH_ComboBox_Popup, cb, widget);
            painter->save();
            painter->setClipRect(editRect);

            if (!cb->currentIcon.isNull()) {
                QIcon::Mode mode = cb->state & State_Enabled ? QIcon::Normal
                                   : QIcon::Disabled;
                QPixmap pixmap = cb->currentIcon.pixmap(cb->iconSize, mode);
                QRect iconRect(editRect);
                iconRect.setWidth(cb->iconSize.width() + 4);

                iconRect = alignedRect(cb->direction,
                                       Qt::AlignLeft | Qt::AlignVCenter,
                                       iconRect.size(), editRect);

                if (cb->editable)
                    painter->fillRect(iconRect, option->palette.brush(QPalette::Base));

                drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);

                if (cb->direction == Qt::RightToLeft)
                    editRect.translate(-4 - cb->iconSize.width(), 0);
                else
                    editRect.translate(cb->iconSize.width() + 4, 0);
            }

            if (!cb->currentText.isEmpty() && !cb->editable) {
                GtkWidget *gtkCombo = QGtk::gtkWidget(QLS("GtkComboBox"));
                QPalette pal = cb->palette;
                int labelState = GTK_STATE_INSENSITIVE;

                if (option->state & State_Enabled)
                    labelState = (option->state & State_MouseOver && !appearsAsList) ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL;

                GdkColor gdkText = gtkCombo->style->fg[labelState];

                QColor textColor = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);

                pal.setBrush(QPalette::ButtonText, textColor);

                drawItemText(painter, editRect.adjusted(1, 0, -1, 0),
                             visualAlignment(cb->direction, Qt::AlignLeft | Qt::AlignVCenter),
                             pal, cb->state & State_Enabled, cb->currentText, QPalette::ButtonText);
            }

            painter->restore();
        }
        break;

#endif // QT_NO_COMBOBOX

    case CE_DockWidgetTitle:
        painter->save();
        if (const QStyleOptionDockWidget *dwOpt = qstyleoption_cast<const QStyleOptionDockWidget *>(option)) {
            const QStyleOptionDockWidgetV2 *v2
                = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(dwOpt);
            bool verticalTitleBar = v2 == 0 ? false : v2->verticalTitleBar;

            QRect rect = dwOpt->rect;
            QRect titleRect = subElementRect(SE_DockWidgetTitleBarText, option, widget).adjusted(-2, 0, -2, 0);
            QRect r = rect.adjusted(0, 0, -1, -1);
            if (verticalTitleBar)
                r.adjust(0, 0, 0, -1);

            if (verticalTitleBar) {
                QRect r = rect;
                QSize s = r.size();
                s.transpose();
                r.setSize(s);

                titleRect = QRect(r.left() + rect.bottom()
                                    - titleRect.bottom(),
                                r.top() + titleRect.left() - rect.left(),
                                titleRect.height(), titleRect.width());

                painter->translate(r.left(), r.top() + r.width());
                painter->rotate(-90);
                painter->translate(-r.left(), -r.top());

                rect = r;
            }

            if (!dwOpt->title.isEmpty()) {
                QString titleText
                    = painter->fontMetrics().elidedText(dwOpt->title,
                                            Qt::ElideRight, titleRect.width());
                drawItemText(painter,
                             titleRect,
                             Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, dwOpt->palette,
                             dwOpt->state & State_Enabled, titleText,
                             QPalette::WindowText);
                }
        }
        painter->restore();
        break;



    case CE_HeaderSection:
        painter->save();

        // Draws the header in tables.
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            Q_UNUSED(header);
            GtkWidget *gtkTreeView = QGtk::gtkWidget(QLS("GtkTreeView"));
            // Get the middle column
            GtkTreeViewColumn *column = QGtk::gtk_tree_view_get_column((GtkTreeView*)gtkTreeView, 1);
            Q_ASSERT(column);

            GtkWidget *gtkTreeHeader = column->button;
            GtkStateType state = gtkPainter.gtkState(option);
            GtkShadowType shadow = GTK_SHADOW_OUT;

            if (option->state & State_Sunken)
                shadow = GTK_SHADOW_IN;
            
            gtkPainter.paintBox(gtkTreeHeader, "button",  option->rect.adjusted(-1, 0, 0, 0), state, shadow, gtkTreeHeader->style);
        }

        painter->restore();
        break;

#ifndef QT_NO_SIZEGRIP

    case CE_SizeGrip: {
        GtkWidget *gtkStatusbar = QGtk::gtkWidget(QLS("GtkStatusbar.GtkFrame"));
        QRect gripRect = option->rect.adjusted(0, 0, -gtkStatusbar->style->xthickness, -gtkStatusbar->style->ythickness);
        gtkPainter.paintResizeGrip( gtkStatusbar, "window", gripRect, GTK_STATE_NORMAL,
                                    GTK_SHADOW_OUT, QApplication::isRightToLeft() ?
                                        GDK_WINDOW_EDGE_SOUTH_WEST : GDK_WINDOW_EDGE_SOUTH_EAST,
                                    gtkStatusbar->style);
    }
    break;

#endif // QT_NO_SIZEGRIP

    case CE_MenuBarEmptyArea: {
        GtkWidget *gtkMenubar = QGtk::gtkWidget(QLS("GtkMenuBar"));
        GdkColor gdkBg = gtkMenubar->style->bg[GTK_STATE_NORMAL]; // Theme can depend on transparency
        painter->fillRect(option->rect, QColor(gdkBg.red>>8, gdkBg.green>>8, gdkBg.blue>>8));
        if (widget) { // See CE_MenuBarItem
            QRect menuBarRect = widget->rect();
            QPixmap pixmap(menuBarRect.size());
            pixmap.fill(Qt::transparent);
            QPainter pmPainter(&pixmap);
            QGtkPainter gtkMenuBarPainter(&pmPainter);
            GtkShadowType shadow_type;
            QGtk::gtk_widget_style_get(gtkMenubar, "shadow-type", &shadow_type, NULL);
            gtkMenuBarPainter.paintBox( gtkMenubar, "menubar",  menuBarRect,
                                        GTK_STATE_NORMAL, shadow_type, gtkMenubar->style);
            pmPainter.end();
            painter->drawPixmap(option->rect, pixmap, option->rect);
        }
    }
    break;

    case CE_MenuBarItem:
        painter->save();

        if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            GtkWidget *gtkMenubarItem = QGtk::gtkWidget(QLS("GtkMenuBar.GtkMenuItem"));
            GtkWidget *gtkMenubar = QGtk::gtkWidget(QLS("GtkMenuBar"));

            style = gtkMenubarItem->style;

            if (widget) {
                // Since Qt does not currently allow filling the entire background
                // we use a hack for this by making a complete menubar each time and
                // paint with the correct offset inside it. Pixmap caching should resolve
                // most of the performance penalty.
                QRect menuBarRect = widget->rect();
                QPixmap pixmap(menuBarRect.size());
                pixmap.fill(Qt::transparent);
                QPainter pmPainter(&pixmap);
                QGtkPainter menubarPainter(&pmPainter);
                GtkShadowType shadow_type;
                QGtk::gtk_widget_style_get(gtkMenubar, "shadow-type", &shadow_type, NULL);
                GdkColor gdkBg = gtkMenubar->style->bg[GTK_STATE_NORMAL]; // Theme can depend on transparency
                painter->fillRect(option->rect, QColor(gdkBg.red>>8, gdkBg.green>>8, gdkBg.blue>>8));
                menubarPainter.paintBox(gtkMenubar, "menubar",  menuBarRect,
                                        GTK_STATE_NORMAL, shadow_type, gtkMenubar->style);
                pmPainter.end();
                painter->drawPixmap(option->rect, pixmap, option->rect);
            }

            QStyleOptionMenuItem item = *mbi;
            bool act = mbi->state & State_Selected && mbi->state & State_Sunken;
            bool dis = !(mbi->state & State_Enabled);
            item.rect = mbi->rect;
            GdkColor gdkText = gtkMenubarItem->style->fg[dis ? GTK_STATE_INSENSITIVE : GTK_STATE_NORMAL];
            GdkColor gdkHText = gtkMenubarItem->style->fg[GTK_STATE_PRELIGHT];
            QColor normalTextColor = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
            QColor highlightedTextColor = QColor(gdkHText.red>>8, gdkHText.green>>8, gdkHText.blue>>8);
            item.palette.setBrush(QPalette::HighlightedText, highlightedTextColor);
            item.palette.setBrush(QPalette::Text, normalTextColor);
            item.palette.setBrush(QPalette::ButtonText, normalTextColor);
            QCommonStyle::drawControl(element, &item, painter, widget);

            if (act) {
                GtkShadowType shadowType = GTK_SHADOW_NONE;
                QGtk::gtk_widget_style_get (gtkMenubarItem, "selected-shadow-type", &shadowType, NULL);
                gtkPainter.paintBox(gtkMenubarItem, "menuitem",  option->rect.adjusted(0, 0, 0, 3),
                                    GTK_STATE_PRELIGHT, shadowType, gtkMenubarItem->style);
                //draw text
                QPalette::ColorRole textRole = dis ? QPalette::Text : QPalette::HighlightedText;
                uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

                if (!QCleanlooksStyle::styleHint(SH_UnderlineShortcut, mbi, widget))
                    alignment |= Qt::TextHideMnemonic;

                drawItemText(painter, item.rect, alignment, item.palette, mbi->state & State_Enabled, mbi->text, textRole);
            }
        }
        painter->restore();
        break;

    case CE_Splitter: {
        GtkWidget *gtkWindow = QGtk::gtkWidget(QLS("GtkWindow")); // The Murrine Engine currently assumes a widget is passed
        gtkPainter.paintHandle(gtkWindow, "splitter", option->rect, gtkPainter.gtkState(option), GTK_SHADOW_NONE,
                                !(option->state & State_Horizontal) ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL,
                                style);
    }
    break;

#ifndef QT_NO_TOOLBAR

    case CE_ToolBar:
        if (const QStyleOptionToolBar *toolbar = qstyleoption_cast<const QStyleOptionToolBar *>(option)) {
            // Reserve the beveled appearance only for mainwindow toolbars
            if (!(widget && qobject_cast<const QMainWindow*> (widget->parentWidget())))
                break;

            QRect rect = option->rect;
            // There is a 1 pixel gap between toolbar lines in some styles (i.e Human)
            if (toolbar->positionWithinLine != QStyleOptionToolBar::End)
                rect.adjust(0, 0, 1, 0);

            GtkWidget *gtkToolbar = QGtk::gtkWidget(QLS("GtkToolbar"));
            GtkShadowType shadow_type = GTK_SHADOW_NONE;
            QGtk::gtk_widget_style_get(gtkToolbar, "shadow-type", &shadow_type, NULL);
            gtkPainter.paintBox( gtkToolbar, "toolbar",  rect,
                                 GTK_STATE_NORMAL, shadow_type, gtkToolbar->style);
        }
        break;

#endif // QT_NO_TOOLBAR

    case CE_MenuItem:
        painter->save();

        // Draws one item in a popup menu.
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            const int windowsItemFrame        =  2; // menu item frame width
            const int windowsItemHMargin      =  3; // menu item hor text margin
            const int windowsItemVMargin      = 26; // menu item ver text margin
            const int windowsRightBorder      = 15; // right border on windows
            GtkWidget *gtkMenu = QGtk::gtkWidget(QLS("GtkMenu"));
            GtkWidget *gtkMenuItem = menuItem->checked ? QGtk::gtkWidget(QLS("GtkMenu.GtkCheckMenuItem")) :
                                     QGtk::gtkWidget(QLS("GtkMenu.GtkMenuItem"));

            style = gtkPainter.getStyle(gtkMenuItem);
            QColor borderColor = option->palette.background().color().darker(160);
            QColor shadow = option->palette.dark().color();

            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
                GtkWidget *gtkMenuSeparator = QGtk::gtkWidget(QLS("GtkMenu.GtkSeparatorMenuItem"));
                painter->setPen(shadow.lighter(106));
                gboolean wide_separators = 0;
                gint     separator_height = 0;
                guint    horizontal_padding = 3;
                if (!QGtk::gtk_check_version(2, 10, 0)) {
                    QGtk::gtk_widget_style_get(gtkMenuSeparator,
                                           "wide-separators",    &wide_separators,
                                           "separator-height",   &separator_height,
                                           "horizontal-padding", &horizontal_padding,
                                           NULL);
                }
                if (wide_separators)
                    gtkPainter.paintBox( gtkMenuSeparator, "hseparator",
                                         option->rect.adjusted(0, 0, 0, -1), GTK_STATE_NORMAL, GTK_SHADOW_NONE, gtkMenu->style);
                else
                    gtkPainter.paintHline( gtkMenuSeparator, "hseparator",
                                           menuItem->rect, GTK_STATE_NORMAL, gtkMenu->style,
                                           option->rect.left() + horizontal_padding, option->rect.width() - 2*horizontal_padding, 2);
                painter->restore();
                break;
            }

            bool selected = menuItem->state & State_Selected && menuItem->state & State_Enabled;

            if (selected) {
                QRect rect = option->rect.adjusted(0, 0, -1, -1);
                gtkPainter.paintBox( gtkMenuItem, "menuitem", rect, GTK_STATE_PRELIGHT, GTK_SHADOW_OUT, style);
            }

            bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
            bool checked = menuItem->checked;
            bool enabled = menuItem->state & State_Enabled;
            bool ignoreCheckMark = false;

            gint checkSize;
            QGtk::gtk_widget_style_get(QGtk::gtkWidget(QLS("GtkMenu.GtkCheckMenuItem")), "indicator-size", &checkSize, NULL);

            int checkcol = qMax(menuItem->maxIconWidth, qMax(20, checkSize));

#ifndef QT_NO_COMBOBOX

            if (qobject_cast<const QComboBox*>(widget))
                ignoreCheckMark = true; // Ignore the checkmarks provided by the QComboMenuDelegate

#endif
            if (!ignoreCheckMark) {
                // Check
                QRect checkRect(option->rect.left() + 7, option->rect.center().y() - checkSize/2, checkSize, checkSize);
                checkRect = visualRect(menuItem->direction, menuItem->rect, checkRect);

                if (checkable && menuItem->icon.isNull()) {
                    // Some themes such as aero-clone draw slightly outside the paint rect
                    int spacing = 1; // ### Consider using gtkCheckBox : "indicator-spacing" instead

                    if (menuItem->checkType & QStyleOptionMenuItem::Exclusive) {
                        // Radio button
                        GtkShadowType shadow = GTK_SHADOW_OUT;
                        GtkStateType state = gtkPainter.gtkState(option);

                        if (selected)
                            state = GTK_STATE_PRELIGHT;
                        if (checked)
                            shadow = GTK_SHADOW_IN;

                        gtkPainter.setClipRect(checkRect.adjusted(-spacing, -spacing, spacing, spacing));
                        gtkPainter.paintOption(gtkMenuItem, checkRect.translated(-spacing, -spacing), state, shadow,
                                               gtkMenuItem->style, QLS("option"));
                        gtkPainter.setClipRect(QRect());

                    } else {
                        // Check box
                        if (menuItem->icon.isNull()) {
                            GtkShadowType shadow = GTK_SHADOW_OUT;
                            GtkStateType state = gtkPainter.gtkState(option);

                            if (selected)
                                state = GTK_STATE_PRELIGHT;
                            if (checked)
                                shadow = GTK_SHADOW_IN;

                            gtkPainter.setClipRect(checkRect.adjusted(-spacing, -spacing, -spacing, -spacing));
                            gtkPainter.paintCheckbox(gtkMenuItem, checkRect.translated(-spacing, -spacing), state, shadow,
                                                     gtkMenuItem->style, QLS("check"));
                           gtkPainter.setClipRect(QRect());
                        }
                    }
                }

            } else {
                // Ignore checkmark
                if (menuItem->icon.isNull())
                    checkcol = 0;
                else
                    checkcol = menuItem->maxIconWidth;
            }

            bool dis = !(menuItem->state & State_Enabled);
            bool act = menuItem->state & State_Selected;
            const QStyleOption *opt = option;
            const QStyleOptionMenuItem *menuitem = menuItem;
            QPainter *p = painter;
            QRect vCheckRect = visualRect(opt->direction, menuitem->rect,
                                          QRect(menuitem->rect.x() + 3, menuitem->rect.y(),
                                                checkcol, menuitem->rect.height()));

            if (!menuItem->icon.isNull()) {
                QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;

                if (act && !dis)
                    mode = QIcon::Active;

                QPixmap pixmap;
                int smallIconSize = pixelMetric(PM_SmallIconSize, option, widget);
                QSize iconSize(smallIconSize, smallIconSize);

#ifndef QT_NO_COMBOBOX
                if (const QComboBox *combo = qobject_cast<const QComboBox*>(widget))
                    iconSize = combo->iconSize();

#endif // QT_NO_COMBOBOX
                if (checked)
                    pixmap = menuItem->icon.pixmap(iconSize, mode, QIcon::On);
                else
                    pixmap = menuItem->icon.pixmap(iconSize, mode);

                int pixw = pixmap.width();
                int pixh = pixmap.height();
                QRect pmr(0, 0, pixw, pixh);
                pmr.moveCenter(vCheckRect.center());
                painter->setPen(menuItem->palette.text().color());
                if (!ignoreCheckMark && checkable && checked) {
                    QStyleOption opt = *option;

                    if (act) {
                        QColor activeColor = mergedColors(option->palette.background().color(),
                                                          option->palette.highlight().color());
                        opt.palette.setBrush(QPalette::Button, activeColor);
                    }
                    opt.state |= State_Sunken;
                    opt.rect = vCheckRect;
                    drawPrimitive(PE_PanelButtonCommand, &opt, painter, widget);
                }

                painter->drawPixmap(pmr.topLeft(), pixmap);
            }

            GdkColor gdkText = gtkMenuItem->style->fg[GTK_STATE_NORMAL];
            GdkColor gdkDText = gtkMenuItem->style->fg[GTK_STATE_INSENSITIVE];
            GdkColor gdkHText = gtkMenuItem->style->fg[GTK_STATE_PRELIGHT];
            QColor textColor = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
            QColor disabledTextColor = QColor(gdkDText.red>>8, gdkDText.green>>8, gdkDText.blue>>8);
            QColor highlightedTextColor = QColor(gdkHText.red>>8, gdkHText.green>>8, gdkHText.blue>>8);

            if (selected)
                painter->setPen(highlightedTextColor);
            else
                painter->setPen(textColor);

            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            int tab = menuitem->tabWidth;
            int xm = windowsItemFrame + checkcol + windowsItemHMargin;
            int xpos = menuitem->rect.x() + xm;
            QRect textRect(xpos, y + windowsItemVMargin, w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
            QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);
            QString s = menuitem->text;

            if (!s.isEmpty()) { // Draw text
                p->save();
                int t = s.indexOf(QLatin1Char('\t'));
                int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

                if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
                    text_flags |= Qt::TextHideMnemonic;

                // Draw shortcut right aligned
                text_flags |= Qt::AlignRight;

                if (t >= 0) {
                    int rightMargin = 12; // Hardcode for now
                    QRect vShortcutRect = visualRect(opt->direction, menuitem->rect,
                                                     QRect(textRect.topRight(), QPoint(menuitem->rect.right() - rightMargin, textRect.bottom())));

                    if (dis)
                        p->setPen(disabledTextColor);
                    p->drawText(vShortcutRect, text_flags , s.mid(t + 1));
                    s = s.left(t);
                }

                text_flags &= ~Qt::AlignRight;
                text_flags |= Qt::AlignLeft;
                QFont font = menuitem->font;
                if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                    font.setBold(true);
                p->setFont(font);

                if (dis)
                    p->setPen(disabledTextColor);
                p->drawText(vTextRect, text_flags, s.left(t));
                p->restore();
            }

            // Arrow
            if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {// draw sub menu arrow
                QPoint buttonShift(pixelMetric(PM_ButtonShiftHorizontal, option, widget),
                                   pixelMetric(PM_ButtonShiftVertical, option, widget));

                QFontMetrics fm(menuitem->font);
                int arrow_size = fm.ascent() + fm.descent() - 2 * gtkMenuItem->style->ythickness;
                gfloat arrow_scaling = 0.8;

                // "arrow-scaling" is actually hardcoded and fails on hardy (see gtk+-2.12/gtkmenuitem.c)
                // though the current documentation states otherwise
                int horizontal_padding;
                QGtk::gtk_widget_style_get(gtkMenuItem, "horizontal-padding", &horizontal_padding, NULL);

                const int dim = static_cast<int>(arrow_size * arrow_scaling);
                int xpos = menuItem->rect.left() + menuItem->rect.width() - horizontal_padding - dim;
                QRect  vSubMenuRect = visualRect(option->direction, menuItem->rect,
                                                 QRect(xpos, menuItem->rect.top() +
                                                       menuItem->rect.height() / 2 - dim / 2, dim, dim));
                GtkStateType state = enabled ? (act ? GTK_STATE_PRELIGHT: GTK_STATE_NORMAL) : GTK_STATE_INSENSITIVE;
                GtkShadowType shadowType = (state == GTK_STATE_PRELIGHT) ? GTK_SHADOW_OUT : GTK_SHADOW_IN;
                gtkPainter.paintArrow(gtkMenuItem, "menuitem", vSubMenuRect, QApplication::isRightToLeft() ? GTK_ARROW_LEFT : GTK_ARROW_RIGHT, state,
                                      shadowType, FALSE, style);
            }
        }
        painter->restore();
        break;

    case CE_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
            drawControl(CE_PushButtonBevel, btn, painter, widget);
            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(SE_PushButtonContents, btn, widget);
            gint interiorFocus = true;
            QGtk::gtk_widget_style_get(gtkButton, "interior-focus", &interiorFocus, NULL);
            int xt = interiorFocus ? gtkButton->style->xthickness : 0;
            int yt = interiorFocus ? gtkButton->style->ythickness : 0;

            if (btn->features & QStyleOptionButton::Flat && btn->state & State_HasFocus)
                // The normal button focus rect does not work well for flat buttons in Clearlooks
                drawPrimitive(PE_FrameFocusRect, option, painter, widget);
            else if (btn->state & State_HasFocus)
                gtkPainter.paintFocus(gtkButton, "button",
                                      option->rect.adjusted(xt, yt, -xt, -yt),
                                      btn->state & State_Sunken ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL,
                                      gtkButton->style);

            drawControl(CE_PushButtonLabel, &subopt, painter, widget);
        }
        break;

#ifndef QT_NO_TABBAR

    case CE_TabBarTabShape:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            GtkWidget *gtkNotebook = QGtk::gtkWidget(QLS("GtkNotebook"));
            style = gtkPainter.getStyle(gtkNotebook);

            QRect rect = option->rect;
            GtkShadowType shadow = GTK_SHADOW_OUT;
            GtkStateType state = GTK_STATE_ACTIVE;
            if (tab->state & State_Selected)
                state = GTK_STATE_NORMAL;

            bool first = tab->position == QStyleOptionTab::Beginning || tab->position == QStyleOptionTab::OnlyOneTab;
            bool last = tab->position == QStyleOptionTab::End || tab->position == QStyleOptionTab::OnlyOneTab;
            bool selected = (tab->state & State_Selected);
            if (option->direction == Qt::RightToLeft) {
                bool tmp = first;
                first = last;
                last = tmp;
            }
            int topIndent = 3;
            int bottomIndent = 1;
            int tabOverlap = 1;
            painter->save();

            switch (tab->shape) {
            case QTabBar::RoundedNorth:
                if (!selected)
                    rect.adjust(first ? 0 : -tabOverlap, topIndent, last ? 0 : tabOverlap, -bottomIndent);
                gtkPainter.paintExtention( gtkNotebook, "tab", rect,
                                           state, shadow, GTK_POS_BOTTOM, style);
                break;

            case QTabBar::RoundedSouth:
                if (!selected)
                    rect.adjust(first ? 0 : -tabOverlap, 0, last ? 0 : tabOverlap, -topIndent);
                gtkPainter.paintExtention( gtkNotebook, "tab", rect.adjusted(0, 1, 0, 0),
                                           state, shadow, GTK_POS_TOP, style);
                break;

            case QTabBar::RoundedWest:
                if (!selected)
                    rect.adjust(topIndent, 0, -bottomIndent, 0);
                gtkPainter.paintExtention( gtkNotebook, "tab", rect, state, shadow, GTK_POS_RIGHT, style);
                break;

            case QTabBar::RoundedEast:
                if (!selected)
                    rect.adjust(bottomIndent, 0, -topIndent, 0);
                gtkPainter.paintExtention( gtkNotebook, "tab", rect, state, shadow, GTK_POS_LEFT, style);
                break;

            default:
                QCleanlooksStyle::drawControl(element, option, painter, widget);
                break;
            }

            painter->restore();
        }

        break;

#endif //QT_NO_TABBAR

    case CE_ProgressBarGroove:
        if (const QStyleOptionProgressBar *bar = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
            Q_UNUSED(bar);
            GtkWidget *gtkProgressBar = QGtk::gtkWidget(QLS("GtkProgressBar"));
            GtkStateType state = gtkPainter.gtkState(option);
            gtkPainter.paintBox( gtkProgressBar, "trough",  option->rect, state, GTK_SHADOW_IN, gtkProgressBar->style);
        }

        break;

    case CE_ProgressBarContents:
        if (const QStyleOptionProgressBar *bar = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
            GtkStateType state = option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE;
            GtkWidget *gtkProgressBar = QGtk::gtkWidget(QLS("GtkProgressBar"));
            style = gtkProgressBar->style;
            gtkPainter.paintBox( gtkProgressBar, "trough",  option->rect, state, GTK_SHADOW_IN, style);
            int xt = style->xthickness;
            int yt = style->ythickness;
            QRect rect = bar->rect.adjusted(xt, yt, -xt, -yt);
            bool vertical = false;
            bool inverted = false;
            bool indeterminate = (bar->minimum == 0 && bar->maximum == 0);
            // Get extra style options if version 2

            if (const QStyleOptionProgressBarV2 *bar2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(option)) {
                vertical = (bar2->orientation == Qt::Vertical);
                inverted = bar2->invertedAppearance;
            }

            // If the orientation is vertical, we use a transform to rotate
            // the progress bar 90 degrees clockwise.  This way we can use the
            // same rendering code for both orientations.
            if (vertical) {
                rect.translate(xt, -yt * 2);
                rect = QRect(rect.left(), rect.top(), rect.height(), rect.width()); // Flip width and height
                QTransform m;
                m.translate(rect.height(), 0);
                m.rotate(90.0);
                painter->setTransform(m);
            }

            int maxWidth = rect.width();
            int minWidth = 4;

            qint64 progress = (qint64)qMax(bar->progress, bar->minimum); // Workaround for bug in QProgressBar
            double vc6_workaround = ((progress - qint64(bar->minimum)) / double(qint64(bar->maximum) - qint64(bar->minimum))) * maxWidth;
            int progressBarWidth = (int(vc6_workaround) > minWidth ) ? int(vc6_workaround) : minWidth;
            int width = indeterminate ? maxWidth : progressBarWidth;
            bool reverse = (!vertical && (bar->direction == Qt::RightToLeft)) || vertical;

            if (inverted)
                reverse = !reverse;

            int maximum = 2;
            int fakePos = 0;
            if (bar->minimum == bar->maximum)
                maximum = 0;
            if (bar->progress == bar->maximum)
                fakePos = maximum;
            else if (bar->progress > bar->minimum)
                fakePos = maximum - 1;

            GtkObject *adjustment =  QGtk::gtk_adjustment_new(fakePos, 0, maximum, 0, 0, 0);
            QGtk::gtk_progress_set_adjustment((GtkProgress*)(gtkProgressBar), (GtkAdjustment*)(adjustment));

            QRect progressBar;

            if (!indeterminate) {
                if (!reverse)
                    progressBar.setRect(rect.left(), rect.top(), width, rect.height());
                else
                    progressBar.setRect(rect.right() - width, rect.top(), width, rect.height());

            } else {
                Q_D(const QGtkStyle);
                int slideWidth = ((rect.width() - 4) * 2) / 3;
                int step = ((d->animateStep * slideWidth) / d->animationFps) % slideWidth;
                if ((((d->animateStep * slideWidth) / d->animationFps) % (2 * slideWidth)) >= slideWidth)
                    step = slideWidth - step;
                progressBar.setRect(rect.left() + step, rect.top(), slideWidth / 2, rect.height());
            }

            QString key = QString(QLS("%0")).arg(fakePos);
            if (inverted) {
                key += QLatin1String("inv");
                gtkPainter.setFlipHorizontal(true);
            }
            gtkPainter.paintBox( gtkProgressBar, "bar",  progressBar, GTK_STATE_SELECTED, GTK_SHADOW_OUT, style, key);
        }

        break;

    default:
        QCleanlooksStyle::drawControl(element, option, painter, widget);
    }
}

/*!
  \reimp
*/
QRect QGtkStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                SubControl subControl, const QWidget *widget) const
{
    QRect rect = QWindowsStyle::subControlRect(control, option, subControl, widget);
    if (!QGtk::isThemeAvailable())
        return QCleanlooksStyle::subControlRect(control, option, subControl, widget);

    switch (control) {
    case CC_TitleBar:
        return QCleanlooksStyle::subControlRect(control, option, subControl, widget);

    case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            // Reserve space for outside focus rect
            QStyleOptionSlider sliderCopy = *slider;
            sliderCopy.rect = option->rect.adjusted(2, 2, -2, -2);
            return QCleanlooksStyle::subControlRect(control, &sliderCopy, subControl, widget);
        }

        break;

#ifndef QT_NO_GROUPBOX

    case CC_GroupBox:
        if (qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            rect = option->rect.adjusted(0, groupBoxTopMargin, 0, -groupBoxBottomMargin);
            int topMargin = 0;
            int topHeight = 0;
            topHeight = 10;
            QRect frameRect = rect;
            frameRect.setTop(topMargin);

            if (subControl == SC_GroupBoxFrame)
                return rect;
            else if (subControl == SC_GroupBoxContents) {
                int margin = 0;
                int leftMarginExtension = 8;
                return frameRect.adjusted(leftMarginExtension + margin, margin + topHeight + groupBoxTitleMargin, -margin, -margin);
            }

            if (const QGroupBox *groupBoxWidget = qobject_cast<const QGroupBox *>(widget)) {
                //Prepare metrics for a bold font
                QFont font = widget->font();
                font.setBold(true);
                QFontMetrics fontMetrics(font);
                QSize textRect = fontMetrics.boundingRect(groupBoxWidget->title()).size() + QSize(4, 4);
                int indicatorWidth = pixelMetric(PM_IndicatorWidth, option, widget);
                int indicatorHeight = pixelMetric(PM_IndicatorHeight, option, widget);

                if (subControl == SC_GroupBoxCheckBox) {
                    rect.setWidth(indicatorWidth);
                    rect.setHeight(indicatorHeight);
                    rect.moveTop((textRect.height() - indicatorHeight) / 2);

                } else if (subControl == SC_GroupBoxLabel) {
                    if (groupBoxWidget->isCheckable())
                        rect.adjust(indicatorWidth + 4, 0, 0, 0);
                    rect.setSize(textRect);
                }
                rect = visualRect(option->direction, option->rect, rect);
            }
        }

        return rect;

#endif
#ifndef QT_NO_SPINBOX

    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            GtkWidget *gtkSpinButton = QGtk::gtkWidget(QLS("GtkSpinButton"));
            int center = spinbox->rect.height() / 2;
            int xt = spinbox->frame ? gtkSpinButton->style->xthickness : 0;
            int yt = spinbox->frame ? gtkSpinButton->style->ythickness : 0;
            int y = yt;

            QSize bs;
            bs.setHeight(qMax(8, spinbox->rect.height()/2 - y));
            bs.setWidth(spinboxArrowSize());
            int x, lx, rx;
            x = spinbox->rect.width() - y - bs.width() + 2;
            lx = xt;
            rx = x - xt;

            switch (subControl) {

            case SC_SpinBoxUp:
                if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                    return QRect();
                rect = QRect(x, xt, bs.width(), center - yt);
                break;

            case SC_SpinBoxDown:
                if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                    return QRect();
                rect = QRect(x, center, bs.width(), spinbox->rect.bottom() - center - yt + 1);
                break;

            case SC_SpinBoxEditField:
                if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                    rect = QRect(lx, yt, spinbox->rect.width() - 2*xt, spinbox->rect.height() - 2*yt);
                else
                    rect = QRect(lx, yt, rx - qMax(xt - 1, 0), spinbox->rect.height() - 2*yt);
                break;

            case SC_SpinBoxFrame:
                rect = spinbox->rect;

            default:
                break;
            }

            rect = visualRect(spinbox->direction, spinbox->rect, rect);
        }

        break;

#endif // Qt_NO_SPINBOX
#ifndef QT_NO_COMBOBOX

    case CC_ComboBox:
        if (const QStyleOptionComboBox *box = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            // We employ the gtk widget to position arrows and separators for us
            QString comboBoxPath = box->editable ? QLS("GtkComboBoxEntry") : QLS("GtkComboBox");
            GtkWidget *gtkCombo = QGtk::gtkWidget(comboBoxPath);
            QGtk::gtk_widget_set_direction(gtkCombo, (option->direction == Qt::RightToLeft) ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
            GtkAllocation geometry = {0, 0, qMax(0, option->rect.width()), qMax(0, option->rect.height())};
            QGtk::gtk_widget_size_allocate(gtkCombo, &geometry);
            int appears_as_list = !styleHint(QStyle::SH_ComboBox_Popup, option, widget);
            QString arrowPath = comboBoxPath + QLS(".GtkToggleButton");

            if (!box->editable && !appears_as_list)
                arrowPath += QLS(".GtkHBox.GtkArrow");

            GtkWidget *arrowWidget = QGtk::gtkWidget(arrowPath);
            if (!arrowWidget)
                return QCleanlooksStyle::subControlRect(control, option, subControl, widget);

            QRect buttonRect(arrowWidget->allocation.x, arrowWidget->allocation.y,
                             arrowWidget->allocation.width, arrowWidget->allocation.height);

            switch (subControl) {

            case SC_ComboBoxArrow: // Note: this indicates the arrowbutton for editable combos
                rect = buttonRect;

                break;

            case SC_ComboBoxEditField: {
                rect = visualRect(option->direction, option->rect, rect);
                int xMargin = box->editable ? 1 : 4, yMargin = 2;
                rect.setRect(option->rect.left() + gtkCombo->style->xthickness + xMargin,
                             option->rect.top()  + gtkCombo->style->ythickness + yMargin,
                             option->rect.width() - buttonRect.width() - 2*(gtkCombo->style->xthickness + xMargin),
                             option->rect.height() - 2*(gtkCombo->style->ythickness + yMargin));
                rect = visualRect(option->direction, option->rect, rect);
                break;
            }

            default:
                break;
            }
        }

        break;

#endif // QT_NO_COMBOBOX

    default:
        break;
    }

    return rect;
}

/*!
  \reimp
*/
QSize QGtkStyle::sizeFromContents(ContentsType type, const QStyleOption *option,

                                  const QSize &size, const QWidget *widget) const
{

    QSize newSize = QCleanlooksStyle::sizeFromContents(type, option, size, widget);
    if (!QGtk::isThemeAvailable())
        return newSize;

    switch (type) {

    case CT_ToolButton:
        if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
            newSize = size + QSize(2 * gtkButton->style->xthickness, 1 + 2 * gtkButton->style->ythickness);
            if (widget && qobject_cast<QToolBar *>(widget->parentWidget())) {
                QSize minSize(0, 25);
                if (toolbutton->toolButtonStyle != Qt::ToolButtonTextOnly)
                    minSize = toolbutton->iconSize + QSize(12, 12);
                newSize = newSize.expandedTo(minSize);
            }
            
            if (toolbutton->features & QStyleOptionToolButton::HasMenu)
                newSize += QSize(6, 0);
        }
        break;

    case CT_MenuItem:
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            int textMargin = 8;

            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
                GtkWidget *gtkMenuSeparator = QGtk::gtkWidget(QLS("GtkMenu.GtkSeparatorMenuItem"));
                gboolean wide_separators;
                gint     separator_height;
                QGtk::gtk_widget_style_get(gtkMenuSeparator,
                                       "wide-separators",    &wide_separators,
                                       "separator-height",   &separator_height,
                                       NULL);
                newSize = QSize(size.width(), wide_separators ? separator_height - 1 : 7 );

                break;
            }

            GtkWidget *gtkMenuItem = QGtk::gtkWidget(QLS("GtkMenu.GtkMenuItem"));
            GtkStyle* style = gtkMenuItem->style;
            newSize += QSize(textMargin + style->xthickness - 2, style->ythickness - 4);

            // Cleanlooks assumes a check column of 20 pixels so we need to
            // expand it a bit
            gint checkSize;
            QGtk::gtk_widget_style_get(QGtk::gtkWidget(QLS("GtkMenu.GtkCheckMenuItem")), "indicator-size", &checkSize, NULL);
            newSize.setHeight(qMax(newSize.height(), checkSize + 2));
            newSize.setWidth(newSize.width() + qMax(0, checkSize - 20));
        }

        break;

    case CT_Menu:
        // This is evil, but QMenu adds 1 pixel too much
        newSize -= QSize(0, 1);

        break;

    case CT_SpinBox:
        // QSpinBox does some nasty things that depends on CT_LineEdit
        newSize = size + QSize(0, -QGtk::gtkWidget(QLS("GtkSpinButton"))->style->ythickness * 2 + 2);
        break;

    case CT_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            GtkWidget *gtkButton = QGtk::gtkWidget(QLS("GtkButton"));
            gint focusPadding, focusWidth;
            QGtk::gtk_widget_style_get(gtkButton, "focus-padding", &focusPadding, NULL);
            QGtk::gtk_widget_style_get(gtkButton, "focus-line-width", &focusWidth, NULL);
            newSize = size;
            newSize += QSize(2*gtkButton->style->xthickness + 4, 2*gtkButton->style->ythickness);
            newSize += QSize(2*(focusWidth + focusPadding + 2), 2*(focusWidth + focusPadding));

            GtkWidget *gtkButtonBox = QGtk::gtkWidget(QLS("GtkHButtonBox"));
            gint minWidth = 85, minHeight = 0;
            QGtk::gtk_widget_style_get(gtkButtonBox, "child-min-width", &minWidth,
                                   "child-min-height", &minHeight, NULL);
            if (!btn->text.isEmpty() && newSize.width() < minWidth)
                newSize.setWidth(minWidth);
            if (newSize.height() < minHeight)
                newSize.setHeight(minHeight);
        }

        break;

    case CT_Slider: {
        GtkWidget *gtkSlider = QGtk::gtkWidget(QLS("GtkHScale"));
        newSize = size + QSize(2*gtkSlider->style->xthickness, 2*gtkSlider->style->ythickness);
    }
    break;

    case CT_MenuBarItem://cleanlooks adds 2 pixels
        newSize = QWindowsStyle::sizeFromContents(type, option, size, widget) + QSize(0, 1);
        break;

    case CT_LineEdit: {
        GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("GtkEntry"));
        newSize = size + QSize(2*gtkEntry->style->xthickness, 2*gtkEntry->style->ythickness);
    }
    break;

    case CT_ItemViewItem:
        newSize += QSize(0, 2);
        break;

    case CT_ComboBox:
        if (const QStyleOptionComboBox *combo = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            GtkWidget *gtkCombo = QGtk::gtkWidget(QLS("GtkComboBox"));
            QRect arrowButtonRect = subControlRect(CC_ComboBox, combo, SC_ComboBoxArrow, widget);
            newSize = size + QSize(12 + arrowButtonRect.width() + 2*gtkCombo->style->xthickness, 4 + 2*gtkCombo->style->ythickness);

            if (!(widget && qobject_cast<QToolBar *>(widget->parentWidget())))
                newSize += QSize(0, 3);
        }
        break;

    case CT_GroupBox:
        newSize += QSize(4, groupBoxBottomMargin + groupBoxTopMargin + groupBoxTitleMargin); // Add some space below the groupbox
        break;

    case CT_TabBarTab:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            if (!tab->icon.isNull())
                newSize += QSize(6, 0);
        }
        newSize += QSize(1, 0);
        break;

    default:
        break;
    }

    return newSize;
}

/*! \reimp */
QPixmap QGtkStyle::standardPixmap(StandardPixmap sp, const QStyleOption *option,
                                  const QWidget *widget) const
{
    if (!QGtk::isThemeAvailable())
        return QCleanlooksStyle::standardPixmap(sp, option, widget);

    QPixmap pixmap;
    switch (sp) {

    case SP_TitleBarNormalButton: {
        QImage restoreButton((const char **)dock_widget_restore_xpm);
        QColor alphaCorner = restoreButton.color(2);
        alphaCorner.setAlpha(80);
        restoreButton.setColor(2, alphaCorner.rgba());
        alphaCorner.setAlpha(180);
        restoreButton.setColor(4, alphaCorner.rgba());
        return QPixmap::fromImage(restoreButton);
    }
    break;

    case SP_TitleBarCloseButton: // Fall through
    case SP_DockWidgetCloseButton: {

        QImage closeButton((const char **)dock_widget_close_xpm);
        QColor alphaCorner = closeButton.color(2);
        alphaCorner.setAlpha(80);
        closeButton.setColor(2, alphaCorner.rgba());
        return QPixmap::fromImage(closeButton);
    }
    break;

    case SP_DialogDiscardButton: {
        return QGtkPainter::getIcon(GTK_STOCK_DELETE);
    }

    case SP_DialogOkButton: {
        return QGtkPainter::getIcon(GTK_STOCK_OK);
    }

    case SP_DialogCancelButton: {
        return QGtkPainter::getIcon(GTK_STOCK_CANCEL);
    }

    case SP_DialogYesButton: {
        return QGtkPainter::getIcon(GTK_STOCK_YES);
    }

    case SP_DialogNoButton: {
        return QGtkPainter::getIcon(GTK_STOCK_NO);
    }

    case SP_DialogOpenButton: {
        return QGtkPainter::getIcon(GTK_STOCK_OPEN);
    }

    case SP_DialogCloseButton: {
        return QGtkPainter::getIcon(GTK_STOCK_CLOSE);
    }

    case SP_DialogApplyButton: {
        return QGtkPainter::getIcon(GTK_STOCK_APPLY);
    }

    case SP_DialogSaveButton: {
        return QGtkPainter::getIcon(GTK_STOCK_SAVE);
    }

    case SP_MessageBoxWarning: {
        return QGtkPainter::getIcon(GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
    }

    case SP_MessageBoxQuestion: {
        return QGtkPainter::getIcon(GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
    }

    case SP_MessageBoxInformation: {
        return QGtkPainter::getIcon(GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
    }

    case SP_MessageBoxCritical: {
        return QGtkPainter::getIcon(GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);
    }

    default:
        return QCleanlooksStyle::standardPixmap(sp, option, widget);
    }

    return QPixmap();
}

/*! \reimp */
QRect QGtkStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    QRect r = QCleanlooksStyle::subElementRect(element, option, widget);
    switch (element) {
    case SE_ProgressBarLabel:
    case SE_ProgressBarContents:
    case SE_ProgressBarGroove:
        return option->rect;
    default:
        break;
    }

    return r;
}

/*!
  \reimp
*/
QRect QGtkStyle::itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const
{
    return QCleanlooksStyle::itemPixmapRect(r, flags, pixmap);
}

/*!
  \reimp
*/
void QGtkStyle::drawItemPixmap(QPainter *painter, const QRect &rect,
                            int alignment, const QPixmap &pixmap) const
{
    QCleanlooksStyle::drawItemPixmap(painter, rect, alignment, pixmap);
}

/*!
  \reimp
*/
QStyle::SubControl QGtkStyle::hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                              const QPoint &pt, const QWidget *w) const
{
    return QCleanlooksStyle::hitTestComplexControl(cc, opt, pt, w);
}

/*!
  \reimp
*/
QPixmap QGtkStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                        const QStyleOption *opt) const
{
    return QCleanlooksStyle::generatedIconPixmap(iconMode, pixmap, opt);
}

/*!
  \reimp
*/
void QGtkStyle::drawItemText(QPainter *painter, const QRect &rect, int alignment, const QPalette &pal,
                                    bool enabled, const QString& text, QPalette::ColorRole textRole) const
{
    return QCleanlooksStyle::drawItemText(painter, rect, alignment, pal, enabled, text, textRole);
}

QT_END_NAMESPACE

#endif //!defined(QT_NO_STYLE_QGTK)
