/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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
****************************************************************************/

#include "qtpropertybrowserutils_p.h"
#include <QtGui/QApplication>
#include <QtGui/QPainter>

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

QtCursorDatabase::QtCursorDatabase()
{
    appendCursor(Qt::ArrowCursor, QApplication::translate("QtCursorDatabase", "Arrow", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-arrow.png"));
    appendCursor(Qt::UpArrowCursor, QApplication::translate("QtCursorDatabase", "Up Arrow", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-uparrow.png"));
    appendCursor(Qt::CrossCursor, QApplication::translate("QtCursorDatabase", "Cross", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-cross.png"));
    appendCursor(Qt::WaitCursor, QApplication::translate("QtCursorDatabase", "Wait", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-wait.png"));
    appendCursor(Qt::IBeamCursor, QApplication::translate("QtCursorDatabase", "IBeam", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-ibeam.png"));
    appendCursor(Qt::SizeVerCursor, QApplication::translate("QtCursorDatabase", "Size Vertical", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-sizev.png"));
    appendCursor(Qt::SizeHorCursor, QApplication::translate("QtCursorDatabase", "Size Horizontal", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-sizeh.png"));
    appendCursor(Qt::SizeFDiagCursor, QApplication::translate("QtCursorDatabase", "Size Backslash", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-sizef.png"));
    appendCursor(Qt::SizeBDiagCursor, QApplication::translate("QtCursorDatabase", "Size Slash", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-sizeb.png"));
    appendCursor(Qt::SizeAllCursor, QApplication::translate("QtCursorDatabase", "Size All", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-sizeall.png"));
    appendCursor(Qt::BlankCursor, QApplication::translate("QtCursorDatabase", "Blank", 0,
                        QApplication::UnicodeUTF8), QIcon());
    appendCursor(Qt::SplitVCursor, QApplication::translate("QtCursorDatabase", "Split Vertical", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-vsplit.png"));
    appendCursor(Qt::SplitHCursor, QApplication::translate("QtCursorDatabase", "Split Horizontal", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-hsplit.png"));
    appendCursor(Qt::PointingHandCursor, QApplication::translate("QtCursorDatabase", "Pointing Hand", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-hand.png"));
    appendCursor(Qt::ForbiddenCursor, QApplication::translate("QtCursorDatabase", "Forbidden", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-forbidden.png"));
    appendCursor(Qt::OpenHandCursor, QApplication::translate("QtCursorDatabase", "Open Hand", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-openhand.png"));
    appendCursor(Qt::ClosedHandCursor, QApplication::translate("QtCursorDatabase", "Closed Hand", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-closedhand.png"));
    appendCursor(Qt::WhatsThisCursor, QApplication::translate("QtCursorDatabase", "What's This", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-whatsthis.png"));
    appendCursor(Qt::BusyCursor, QApplication::translate("QtCursorDatabase", "Busy", 0,
                        QApplication::UnicodeUTF8), QIcon(":/trolltech/qtpropertybrowser/images/cursor-busy.png"));
}

void QtCursorDatabase::appendCursor(Qt::CursorShape shape, const QString &name, const QIcon &icon)
{
    if (m_cursorShapeToValue.contains(shape))
        return;
    int value = m_cursorNames.count();
    m_cursorNames.append(name);
    m_cursorIcons[value] = icon;
    m_valueToCursorShape[value] = shape;
    m_cursorShapeToValue[shape] = value;
}

QStringList QtCursorDatabase::cursorShapeNames() const
{
    return m_cursorNames;
}

QMap<int, QIcon> QtCursorDatabase::cursorShapeIcons() const
{
    return m_cursorIcons;
}

QString QtCursorDatabase::cursorToShapeName(const QCursor &cursor) const
{
    int val = cursorToValue(cursor);
    if (val >= 0)
        return m_cursorNames.at(val);
    return QString();
}

QIcon QtCursorDatabase::cursorToShapeIcon(const QCursor &cursor) const
{
    int val = cursorToValue(cursor);
    return m_cursorIcons.value(val);
}

int QtCursorDatabase::cursorToValue(const QCursor &cursor) const
{
    Qt::CursorShape shape = cursor.shape();
    if (m_cursorShapeToValue.contains(shape))
        return m_cursorShapeToValue[shape];
    return -1;
}

QCursor QtCursorDatabase::valueToCursor(int value) const
{
    if (m_valueToCursorShape.contains(value))
        return QCursor(m_valueToCursorShape[value]);
    return QCursor();
}

QPixmap QtPropertyBrowserUtils::brushValuePixmap(const QBrush &b)
{
    QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&img);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(0, 0, img.width(), img.height(), b);
    QColor color = b.color();
    if (color.alpha() != 255) { // indicate alpha by an inset
        QBrush  opaqueBrush = b;
        color.setAlpha(255);
        opaqueBrush.setColor(color);
        painter.fillRect(img.width() / 4, img.height() / 4,
                         img.width() / 2, img.height() / 2, opaqueBrush);
    }
    painter.end();
    return QPixmap::fromImage(img);
}

QIcon QtPropertyBrowserUtils::brushValueIcon(const QBrush &b)
{
    return QIcon(brushValuePixmap(b));
}

QString QtPropertyBrowserUtils::colorValueText(const QColor &c)
{
    return QApplication::translate("QtCursorDatabase", "[%1, %2, %3] (%4)", 0, QApplication::UnicodeUTF8)
                                  .arg(QString::number(c.red()))
                                  .arg(QString::number(c.green()))
                                  .arg(QString::number(c.blue()))
                                  .arg(QString::number(c.alpha()));
}

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif
