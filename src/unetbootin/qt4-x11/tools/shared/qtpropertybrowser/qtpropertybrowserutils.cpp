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

#include "qtpropertybrowserutils_p.h"
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

QtCursorDatabase::QtCursorDatabase()
{
    appendCursor(Qt::ArrowCursor, QApplication::translate("QtCursorDatabase", "Arrow", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-arrow.png")));
    appendCursor(Qt::UpArrowCursor, QApplication::translate("QtCursorDatabase", "Up Arrow", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-uparrow.png")));
    appendCursor(Qt::CrossCursor, QApplication::translate("QtCursorDatabase", "Cross", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-cross.png")));
    appendCursor(Qt::WaitCursor, QApplication::translate("QtCursorDatabase", "Wait", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-wait.png")));
    appendCursor(Qt::IBeamCursor, QApplication::translate("QtCursorDatabase", "IBeam", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-ibeam.png")));
    appendCursor(Qt::SizeVerCursor, QApplication::translate("QtCursorDatabase", "Size Vertical", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-sizev.png")));
    appendCursor(Qt::SizeHorCursor, QApplication::translate("QtCursorDatabase", "Size Horizontal", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-sizeh.png")));
    appendCursor(Qt::SizeFDiagCursor, QApplication::translate("QtCursorDatabase", "Size Backslash", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-sizef.png")));
    appendCursor(Qt::SizeBDiagCursor, QApplication::translate("QtCursorDatabase", "Size Slash", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-sizeb.png")));
    appendCursor(Qt::SizeAllCursor, QApplication::translate("QtCursorDatabase", "Size All", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-sizeall.png")));
    appendCursor(Qt::BlankCursor, QApplication::translate("QtCursorDatabase", "Blank", 0,
                        QApplication::UnicodeUTF8), QIcon());
    appendCursor(Qt::SplitVCursor, QApplication::translate("QtCursorDatabase", "Split Vertical", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-vsplit.png")));
    appendCursor(Qt::SplitHCursor, QApplication::translate("QtCursorDatabase", "Split Horizontal", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-hsplit.png")));
    appendCursor(Qt::PointingHandCursor, QApplication::translate("QtCursorDatabase", "Pointing Hand", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-hand.png")));
    appendCursor(Qt::ForbiddenCursor, QApplication::translate("QtCursorDatabase", "Forbidden", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-forbidden.png")));
    appendCursor(Qt::OpenHandCursor, QApplication::translate("QtCursorDatabase", "Open Hand", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-openhand.png")));
    appendCursor(Qt::ClosedHandCursor, QApplication::translate("QtCursorDatabase", "Closed Hand", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-closedhand.png")));
    appendCursor(Qt::WhatsThisCursor, QApplication::translate("QtCursorDatabase", "What's This", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-whatsthis.png")));
    appendCursor(Qt::BusyCursor, QApplication::translate("QtCursorDatabase", "Busy", 0,
                        QApplication::UnicodeUTF8), QIcon(QLatin1String(":/trolltech/qtpropertybrowser/images/cursor-busy.png")));
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
#ifndef QT_NO_CURSOR
    Qt::CursorShape shape = cursor.shape();
    if (m_cursorShapeToValue.contains(shape))
        return m_cursorShapeToValue[shape];
#endif
    return -1;
}

#ifndef QT_NO_CURSOR
QCursor QtCursorDatabase::valueToCursor(int value) const
{
    if (m_valueToCursorShape.contains(value))
        return QCursor(m_valueToCursorShape[value]);
    return QCursor();
}
#endif

QPixmap QtPropertyBrowserUtils::brushValuePixmap(const QBrush &b)
{
    QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);

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
    return QApplication::translate("QtPropertyBrowserUtils", "[%1, %2, %3] (%4)", 0, QApplication::UnicodeUTF8)
                                  .arg(QString::number(c.red()))
                                  .arg(QString::number(c.green()))
                                  .arg(QString::number(c.blue()))
                                  .arg(QString::number(c.alpha()));
}

QPixmap QtPropertyBrowserUtils::fontValuePixmap(const QFont &font)
{
    QFont f = font;
    QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
    QPainter p(&img);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.setRenderHint(QPainter::Antialiasing, true);
    f.setPointSize(13);
    p.setFont(f);
    QTextOption t;
    t.setAlignment(Qt::AlignCenter);
    p.drawText(QRect(0, 0, 16, 16), QString(QLatin1Char('A')), t);
    return QPixmap::fromImage(img);
}

QIcon QtPropertyBrowserUtils::fontValueIcon(const QFont &f)
{
    return QIcon(fontValuePixmap(f));
}

QString QtPropertyBrowserUtils::fontValueText(const QFont &f)
{
    return QApplication::translate("QtPropertyBrowserUtils", "[%1, %2]", 0, QApplication::UnicodeUTF8)
                                  .arg(f.family())
                                  .arg(f.pointSize());
}


QtBoolEdit::QtBoolEdit(QWidget *parent) :
    QWidget(parent),
    m_checkBox(new QCheckBox(this)),
    m_textVisible(true)
{
    QHBoxLayout *lt = new QHBoxLayout;
    if (QApplication::layoutDirection() == Qt::LeftToRight)
        lt->setContentsMargins(4, 0, 0, 0);
    else
        lt->setContentsMargins(0, 0, 4, 0);
    lt->addWidget(m_checkBox);
    setLayout(lt);
    connect(m_checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    setFocusProxy(m_checkBox);
    m_checkBox->setText(tr("True"));
}

void QtBoolEdit::setTextVisible(bool textVisible)
{
    if (m_textVisible == textVisible)
        return;

    m_textVisible = textVisible;
    if (m_textVisible)
        m_checkBox->setText(isChecked() ? tr("True") : tr("False"));
    else
        m_checkBox->setText(QString());
}

Qt::CheckState QtBoolEdit::checkState() const
{
    return m_checkBox->checkState();
}

void QtBoolEdit::setCheckState(Qt::CheckState state)
{
    m_checkBox->setCheckState(state);
}

bool QtBoolEdit::isChecked() const
{
    return m_checkBox->isChecked();
}

void QtBoolEdit::setChecked(bool c)
{
    m_checkBox->setChecked(c);
    if (!m_textVisible)
        return;
    m_checkBox->setText(isChecked() ? tr("True") : tr("False"));
}

bool QtBoolEdit::blockCheckBoxSignals(bool block)
{
    return m_checkBox->blockSignals(block);
}

void QtBoolEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        m_checkBox->click();
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}


QtKeySequenceEdit::QtKeySequenceEdit(QWidget *parent)
    : QWidget(parent), m_num(0), m_lineEdit(new QLineEdit(this))
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->setMargin(0);
    m_lineEdit->installEventFilter(this);
    m_lineEdit->setReadOnly(true);
    m_lineEdit->setFocusProxy(this);
    setFocusPolicy(m_lineEdit->focusPolicy());
    setAttribute(Qt::WA_InputMethodEnabled);
}

bool QtKeySequenceEdit::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_lineEdit && e->type() == QEvent::ContextMenu) {
        QContextMenuEvent *c = static_cast<QContextMenuEvent *>(e);
        QMenu *menu = m_lineEdit->createStandardContextMenu();
        const QList<QAction *> actions = menu->actions();
        QListIterator<QAction *> itAction(actions);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            action->setShortcut(QKeySequence());
            QString actionString = action->text();
            const int pos = actionString.lastIndexOf(QLatin1Char('\t'));
            if (pos > 0)
                actionString.remove(pos, actionString.length() - pos);
            action->setText(actionString);
        }
        QAction *actionBefore = 0;
        if (actions.count() > 0)
            actionBefore = actions[0];
        QAction *clearAction = new QAction(tr("Clear Shortcut"), menu);
        menu->insertAction(actionBefore, clearAction);
        menu->insertSeparator(actionBefore);
        clearAction->setEnabled(!m_keySequence.isEmpty());
        connect(clearAction, SIGNAL(triggered()), this, SLOT(slotClearShortcut()));
        menu->exec(c->globalPos());
        delete menu;
        e->accept();
        return true;
    }

    return QWidget::eventFilter(o, e);
}

void QtKeySequenceEdit::slotClearShortcut()
{
    if (m_keySequence.isEmpty())
        return;
    setKeySequence(QKeySequence());
    emit keySequenceChanged(m_keySequence);
}

void QtKeySequenceEdit::handleKeyEvent(QKeyEvent *e)
{
    int nextKey = e->key();
    if (nextKey == Qt::Key_Control || nextKey == Qt::Key_Shift ||
            nextKey == Qt::Key_Meta || nextKey == Qt::Key_Alt ||
            nextKey == Qt::Key_Super_L || nextKey == Qt::Key_AltGr)
        return;

    nextKey |= translateModifiers(e->modifiers(), e->text());
    int k0 = m_keySequence[0];
    int k1 = m_keySequence[1];
    int k2 = m_keySequence[2];
    int k3 = m_keySequence[3];
    switch (m_num) {
        case 0: k0 = nextKey; k1 = 0; k2 = 0; k3 = 0; break;
        case 1: k1 = nextKey; k2 = 0; k3 = 0; break;
        case 2: k2 = nextKey; k3 = 0; break;
        case 3: k3 = nextKey; break;
        default: break;
    }
    ++m_num;
    if (m_num > 3)
        m_num = 0;
    m_keySequence = QKeySequence(k0, k1, k2, k3);
    m_lineEdit->setText(m_keySequence.toString(QKeySequence::NativeText));
    e->accept();
    emit keySequenceChanged(m_keySequence);
}

void QtKeySequenceEdit::setKeySequence(const QKeySequence &sequence)
{
    if (sequence == m_keySequence)
        return;
    m_num = 0;
    m_keySequence = sequence;
    m_lineEdit->setText(m_keySequence.toString(QKeySequence::NativeText));
}

QKeySequence QtKeySequenceEdit::keySequence() const
{
    return m_keySequence;
}

int QtKeySequenceEdit::translateModifiers(Qt::KeyboardModifiers state, const QString &text) const
{
    int result = 0;
    if ((state & Qt::ShiftModifier) && (text.size() == 0 || !text.at(0).isPrint() || text.at(0).isLetter() || text.at(0).isSpace()))
        result |= Qt::SHIFT;
    if (state & Qt::ControlModifier)
        result |= Qt::CTRL;
    if (state & Qt::MetaModifier)
        result |= Qt::META;
    if (state & Qt::AltModifier)
        result |= Qt::ALT;
    return result;
}

void QtKeySequenceEdit::focusInEvent(QFocusEvent *e)
{
    m_lineEdit->event(e);
    m_lineEdit->selectAll();
    QWidget::focusInEvent(e);
}

void QtKeySequenceEdit::focusOutEvent(QFocusEvent *e)
{
    m_num = 0;
    m_lineEdit->event(e);
    QWidget::focusOutEvent(e);
}

void QtKeySequenceEdit::keyPressEvent(QKeyEvent *e)
{
    handleKeyEvent(e);
    e->accept();
}

void QtKeySequenceEdit::keyReleaseEvent(QKeyEvent *e)
{
    m_lineEdit->event(e);
}

bool QtKeySequenceEdit::event(QEvent *e)
{
    if (e->type() == QEvent::Shortcut ||
            e->type() == QEvent::ShortcutOverride  ||
            e->type() == QEvent::KeyRelease) {
        e->accept();
        return true;
    }
    return QWidget::event(e);
}




#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif
