/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <qabstracttextdocumentlayout.h>
#include <qtextformat.h>
#include "qtextdocument_p.h"
#include "qtextengine_p.h"

#include "qabstracttextdocumentlayout_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QAbstractTextDocumentLayout
    \reentrant

    \brief The QAbstractTextDocumentLayout class is an abstract base
    class used to implement custom layouts for QTextDocuments.

    \ingroup text

    The standard layout provided by Qt can handle simple word
    processing including inline layouts, lists and tables.

    Some applications (e.g. a word processor or a DTP application)
    might need more features than the ones provided by Qt's layout
    engine, in which case you can subclass QAbstractTextDocumentLayout
    to provide your own custom layout behavior for your text
    documents.
*/

/*!
    \fn void QAbstractTextDocumentLayout::update(const QRectF &rect)

    This signal is emitted when the rectangle \a rect has been
    updated.
*/

/*!
   \fn void QAbstractTextDocumentLayout::updateBlock(const QTextBlock &block)
   \since 4.4

   This signal is emitted when the specified \a block has been updated.
*/

/*!
    \fn void QAbstractTextDocumentLayout::documentSizeChanged(const QSizeF &newSize)

    This signal is emitted when the size of the document changes. The new
    size is specified by \a newSize.

    This information is useful to widgets that display text documents
    since it enables them to update their scroll bars correctly.

    \sa documentSize()
*/

/*!
    \fn void QAbstractTextDocumentLayout::pageCountChanged(int newPages)

    This signal is emitted when the number of pages in the layout
    changes; \a newPages is the updated page count.

    Changes to the page count are due to the changes to the layout or
    the document content itself.

    \sa pageCount()
*/

/*!
    \fn int QAbstractTextDocumentLayout::pageCount() const

    Returns the number of pages required by the layout.

    \sa pageCountChanged()
*/

/*!
    \fn QSizeF QAbstractTextDocumentLayout::documentSize() const

    Returns the total size of the document. This is useful to display widgets
    since they can use to information to update their scroll bars correctly

    \sa documentSizeChanged(), QTextDocument::pageSize
*/

/*!
    \fn void QAbstractTextDocumentLayout::draw(QPainter *painter, const PaintContext &context)

    Draws the layout on the given \a painter with the given \a
    context.
*/

/*!
    \fn int QAbstractTextDocumentLayout::hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const

    Returns the cursor postion for the given \a point with the \a accuracy specified.
    Returns -1 to indicate failure if no valid cursor position was found.
*/

/*!
    \fn void QAbstractTextDocumentLayout::documentChanged(int position, int charsRemoved, int charsAdded)

    This function is called whenever the contents of the document change.
    A change occurs when text is inserted, removed, or a combination of
    the two types of operation. The change is specified by \a position,
    \a charsRemoved, and \a charsAdded corresponding to the starting
    character position of the change, the number of character removed from
    the document, and the number of characters added.

    For example, when inserting the text "Hello" into an empty document,
    \a charsRemoved would be 0 and \a charsAdded would be 5 (the length of
    the string).

    Replacing text is the combination of removal and insertion. For example,
    if the text "Hello" gets replaced by "Hi", \a charsRemoved would be 5
    and \a charsAdded would be 2.
*/

/*!
    \class QAbstractTextDocumentLayout::PaintContext
    \reentrant

    \brief The QAbstractTextDocumentLayout::PaintContext class is a
    convenience class defining the parameters of a painter context.

    A painter context is used when rendering custom layouts for
    QTextDocuments with the QAbstractTextDocumentLayout::draw()
    function, and is specified by a \l {cursorPosition}{cursor
    position}, a \l {palette}{default text color}, a \l clip rectangle
    and a collection of \l selections.

    \sa QAbstractTextDocumentLayout
*/

/*!
    \fn QAbstractTextDocumentLayout::PaintContext::PaintContext()
    \internal
*/

/*!
    \variable QAbstractTextDocumentLayout::PaintContext::cursorPosition
    \brief the position within the document, where the cursor
    line should be drawn.

    The default value is -1.
*/

/*!
    \variable QAbstractTextDocumentLayout::PaintContext::palette
    \brief the default color that is used for the text, when no color
    is specified in the text.

    The default value of this variable uses the application's default
    palette.
*/

/*!
    \variable QAbstractTextDocumentLayout::PaintContext::clip

    \brief a hint to the layout avoiding that paragraphs, frames or
    text that are clearly outside the specified rectangle, are drawn

    Specifying a clip rectangle can speed up drawing of large
    documents significantly. Note that the clip rectangle is in
    document coordinates (i.e., not in viewport coordinates), and that
    it is not a substitute for a clip region set on the painter.

    The default value is a null rectangle.
*/

/*!
    \variable QAbstractTextDocumentLayout::PaintContext::selections

    \brief the collection of selections that will be rendered when
    passing this painter context to QAbstractTextDocumentLayout's
    draw() function.

    The default value of this variable is an empty vector.
*/

/*!
    \class QAbstractTextDocumentLayout::Selection
    \reentrant

    \brief The QAbstractTextDocumentLayout::Selection class is a
    convenience class defining the parameters of a selection.

    A selection can be used to specify a part of a document that
    should be rendere when drawing custom layouts for QTextDocuments
    with the QAbstractTextDocumentLayout::draw() function, and is
    specified by a \l cursor and a \l format.

    \sa QAbstractTextDocumentLayout, PaintContext
*/

/*!
    \variable QAbstractTextDocumentLayout::Selection::format
    \brief the format of the selection

    The default value is QTextFormat::InvalidFormat.
*/

/*!
    \variable QAbstractTextDocumentLayout::Selection::cursor
    \brief the selection's cursor

    The default value is a null cursor.
*/

/*!
    Creates a new text document layout for the given \a document.
*/
QAbstractTextDocumentLayout::QAbstractTextDocumentLayout(QTextDocument *document)
    : QObject(*new QAbstractTextDocumentLayoutPrivate, document)
{
    Q_D(QAbstractTextDocumentLayout);
    d->setDocument(document);
}

/*!
    \internal
*/
QAbstractTextDocumentLayout::QAbstractTextDocumentLayout(QAbstractTextDocumentLayoutPrivate &p, QTextDocument *document)
    :QObject(p, document)
{
    Q_D(QAbstractTextDocumentLayout);
    d->setDocument(document);
}

/*!
    \internal
*/
QAbstractTextDocumentLayout::~QAbstractTextDocumentLayout()
{
}

/*!
    Registers the given \a component as a handler for items of the
    given \a formatType.
*/
void QAbstractTextDocumentLayout::registerHandler(int formatType, QObject *component)
{
    Q_D(QAbstractTextDocumentLayout);

    QTextObjectInterface *iface = qobject_cast<QTextObjectInterface *>(component);
    if (!iface)
        return; // ### print error message on terminal?

    connect(component, SIGNAL(destroyed(QObject*)), this, SLOT(_q_handlerDestroyed(QObject*)));

    QTextObjectHandler h;
    h.iface = iface;
    h.component = component;
    d->handlers.insert(formatType, h);
}

/*!
    Returns a handler for objects of the given \a objectType.
*/
QTextObjectInterface *QAbstractTextDocumentLayout::handlerForObject(int objectType) const
{
    Q_D(const QAbstractTextDocumentLayout);

    QTextObjectHandler handler = d->handlers.value(objectType);
    if (!handler.component)
        return 0;

    return handler.iface;
}

/*!
    Sets the size of the inline object \a item in accordance with the
    text \a format.
    \a posInDocument specifies the position of the object within the document.
*/
void QAbstractTextDocumentLayout::resizeInlineObject(QTextInlineObject item, int posInDocument, const QTextFormat &format)
{
    Q_D(QAbstractTextDocumentLayout);

    QTextCharFormat f = format.toCharFormat();
    Q_ASSERT(f.isValid());
    QTextObjectHandler handler = d->handlers.value(f.objectType());
    if (!handler.component)
        return;

    QSizeF s = handler.iface->intrinsicSize(document(), posInDocument, format);
    item.setWidth(s.width());
    item.setAscent(s.height());
    item.setDescent(0);
}

/*!
    Lays out the inline object \a item using the given text \a format.
    The base class implementation does nothing.
    \a posInDocument specifies the position of the object within the document.

    \sa drawInlineObject()
*/
void QAbstractTextDocumentLayout::positionInlineObject(QTextInlineObject item, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(item);
    Q_UNUSED(posInDocument);
    Q_UNUSED(format);
}

/*!
    \fn void QAbstractTextDocumentLayout::drawInlineObject(QPainter *painter, const QRectF &rect, QTextInlineObject object, int posInDocument, const QTextFormat &format)

    Called to draw the inline object \a object on the given \a painter within
    the rectangle specified by \a rect using the text format specified by
    \a format.
    \a posInDocument specifies the position of the object within the document.

    \sa draw()
*/
void QAbstractTextDocumentLayout::drawInlineObject(QPainter *p, const QRectF &rect, QTextInlineObject item,
                                                   int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(item);
    Q_D(QAbstractTextDocumentLayout);

    QTextCharFormat f = format.toCharFormat();
    Q_ASSERT(f.isValid());
    QTextObjectHandler handler = d->handlers.value(f.objectType());
    if (!handler.component)
        return;

    handler.iface->drawObject(p, rect, document(), posInDocument, format);
}

void QAbstractTextDocumentLayoutPrivate::_q_handlerDestroyed(QObject *obj)
{
    HandlerHash::Iterator it = handlers.begin();
    while (it != handlers.end())
        if ((*it).component == obj)
            it = handlers.erase(it);
        else
            ++it;
}

/*!
    \internal

    Returns the index of the format at position \a pos.
*/
int QAbstractTextDocumentLayout::formatIndex(int pos)
{
    QTextDocumentPrivate *pieceTable = qobject_cast<QTextDocument *>(parent())->docHandle();
    return pieceTable->find(pos).value()->format;
}

/*!
    \fn QTextCharFormat QAbstractTextDocumentLayout::format(int position)

    Returns the character format that is applicable at the given \a position.
*/
QTextCharFormat QAbstractTextDocumentLayout::format(int pos)
{
    QTextDocumentPrivate *pieceTable = qobject_cast<QTextDocument *>(parent())->docHandle();
    int idx = pieceTable->find(pos).value()->format;
    return pieceTable->formatCollection()->charFormat(idx);
}



/*!
    Returns the text document that this layout is operating on.
*/
QTextDocument *QAbstractTextDocumentLayout::document() const
{
    Q_D(const QAbstractTextDocumentLayout);
    return d->document;
}

/*!
    \fn QString QAbstractTextDocumentLayout::anchorAt(const QPointF &position) const

    Returns the reference of the anchor at the given \a position, or an empty
    string if no anchor exists at that point.
*/
QString QAbstractTextDocumentLayout::anchorAt(const QPointF& pos) const
{
    int cursorPos = hitTest(pos, Qt::ExactHit);
    if (cursorPos == -1)
        return QString();

    QTextDocumentPrivate *pieceTable = qobject_cast<const QTextDocument *>(parent())->docHandle();
    QTextDocumentPrivate::FragmentIterator it = pieceTable->find(cursorPos);
    QTextCharFormat fmt = pieceTable->formatCollection()->charFormat(it->format);
    return fmt.anchorHref();
}

/*!
    Returns the bounding rectacle of \a frame.
    \fn QRectF QAbstractTextDocumentLayout::frameBoundingRect(QTextFrame *frame) const
    Returns the bounding rectangle of \a frame.
*/

/*!
    \fn QRectF QAbstractTextDocumentLayout::blockBoundingRect(const QTextBlock &block) const
    Returns the bounding rectangle of \a block.
*/

/*!
    Sets the paint device used for rendering the document's layout to the
    given \a device.

    \sa paintDevice()
*/
void QAbstractTextDocumentLayout::setPaintDevice(QPaintDevice *device)
{
    Q_D(QAbstractTextDocumentLayout);
    d->paintDevice = device;
}

/*!
    Returns the paint device used to render the document's layout.

    \sa setPaintDevice()
*/
QPaintDevice *QAbstractTextDocumentLayout::paintDevice() const
{
    Q_D(const QAbstractTextDocumentLayout);
    return d->paintDevice;
}

QT_END_NAMESPACE

#include "moc_qabstracttextdocumentlayout.cpp"
