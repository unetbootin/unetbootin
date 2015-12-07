/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "messagemodel.h"
#include "trwindow.h"

#include <QMessageBox>
#include <QTextCodec>
#include <QPixmap>

QT_BEGIN_NAMESPACE

/******************************************************************************
 *
 * MessageItem IMPLEMENTATION
 *
 *****************************************************************************/
MessageItem::MessageItem(const TranslatorMessage &message,
                         const QString &text, const QString &comment,
                         const QString &fileName, const int lineNumber, ContextItem *ctxtI)
    : m(message),
      tx(text),
      com(comment),
      m_fileName(fileName),
      m_lineNumber(lineNumber),
      cntxtItem(ctxtI)
{
    if (m.translation().isEmpty()) {
        QString t = QLatin1String("");
        m.setTranslation(t);
    }

    if (m.type() == TranslatorMessage::Finished)
        cntxtItem->incrementFinishedCount();
    m_danger = false;
}

void MessageItem::setTranslation(const QString &translation)
{
    if (translation != m.translation()) {
        m.setTranslation(translation);
        cntxtItem->model()->setModified(true);
    }
}

void MessageItem::setFinished(bool finished)
{
    TranslatorMessage::Type ty = m.type();
    if (ty == TranslatorMessage::Unfinished && finished) {
        m.setType(TranslatorMessage::Finished);
        cntxtItem->incrementFinishedCount();
    } else if (ty == TranslatorMessage::Finished && !finished) {
        m.setType(TranslatorMessage::Unfinished);
        cntxtItem->decrementFinishedCount();
    }
    cntxtItem->model()->setModified(ty != m.type());
}

void MessageItem::setDanger(bool danger)
{
    if (!m_danger && danger) {
        cntxtItem->incrementDangerCount();
    } else if (m_danger && !danger) {
        cntxtItem->decrementDangerCount();
    }
    m_danger = danger;
}


/******************************************************************************
 *
 * ContextItem IMPLEMENTATION
 *
 *****************************************************************************/
ContextItem::ContextItem(QString c, MessageModel *model)
    : com(QLatin1String("")),
      ctxt(c),
      m_finishedCount(0),
      m_dangerCount(0),
      m_obsoleteCount(0),
      m_model(model)
{
}

ContextItem::~ContextItem()
{
    // delete all the message items
    qDeleteAll(msgItemList);
}

void ContextItem::appendToComment(const QString& x)
{
    if (!com.isEmpty())
        com += QString(QLatin1String("\n\n"));
    com += x;
}

void ContextItem::incrementFinishedCount()
{
    ++m_finishedCount;
    m_model->incrementFinishedCount();
}

void ContextItem::decrementFinishedCount()
{
    --m_finishedCount;
    m_model->decrementFinishedCount();
}

MessageItem *ContextItem::messageItem(int i)
{
    if (i >= msgItemList.count() || i < 0)
        return 0;

    return msgItemList[i];
}


/******************************************************************************
 *
 * MessageModel IMPLEMENTATION
 *
 *****************************************************************************/

QPixmap *MessageModel::pxOn = 0;
QPixmap *MessageModel::pxOff = 0;
QPixmap *MessageModel::pxObsolete = 0;
QPixmap *MessageModel::pxDanger = 0;
QPixmap *MessageModel::pxWarning = 0;
QPixmap *MessageModel::pxEmpty = 0;

MessageModel::MessageModel(QObject *parent)
    : QAbstractItemModel(parent),
      m_translator(0),
      m_numFinished(0),
      m_numNonobsolete(0),
      m_numMessages(0),
      m_srcWords(0),
      m_srcChars(0),
      m_srcCharsSpc(0)
{
    // If the icons haven't been loaded yet, load them now
    if (!pxOn) {
        pxOn  = new QPixmap(QLatin1String(":/images/s_check_on.png"));
        pxOff = new QPixmap(QLatin1String(":/images/s_check_off.png"));
        pxObsolete = new QPixmap(QLatin1String(":/images/s_check_obsolete.png"));
        pxDanger = new QPixmap(QLatin1String(":/images/s_check_danger.png"));
        pxWarning = new QPixmap(QLatin1String(":/images/s_check_warning.png"));
        pxEmpty = new QPixmap(QLatin1String(":/images/s_check_empty.png"));
    }

    // Forward modified changed signal
    connect(&cntxtList, SIGNAL(modifiedChanged(bool)), this, SIGNAL(modifiedChanged(bool)));
}

ContextItem *MessageModel::contextItem(const QModelIndex &indx) const
{
    if (indx.isValid()) {
        int row = indx.row();
        if (indx.internalPointer()) row = indx.parent().row();
        return cntxtList.at(row);
    }

    return 0;
}

MessageItem *MessageModel::messageItem(const QModelIndex &indx) const
{
    if (indx.isValid()) {
        ContextItem *c = static_cast<ContextItem *>(indx.internalPointer());
        if (c) return c->messageItem(indx.row());
    }

    return 0;
}

QStringList MessageModel::normalizedTranslations(const MessageItem &m) const
{
    return MetaTranslator::normalizedTranslations(m.message(), m_language, m_country);
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
    if (row < 0 || column < 0) return QModelIndex();
    if (!parent.isValid()) {
        // The context items
        if (row >= rowCount() || column >= columnCount()) {
            return QModelIndex();
        }
        return createIndex(row, column, 0);   //internalpointer points to the 'owner' object
    } else {
        // The message items
        if (parent.internalPointer()) return QModelIndex();
        int toprow = parent.row();
        ContextItem *c = cntxtList.at(toprow);
        if (row > c->messageItemsInList()) return QModelIndex();
        return createIndex(row, column, c);
    }
}

QModelIndex MessageModel::parent(const QModelIndex& index) const
{
    if (ContextItem *cntxtItem = static_cast<ContextItem *>(index.internalPointer())) {
        int idx = cntxtList.indexOf(cntxtItem);
        Q_ASSERT(idx != -1);
        return createIndex(idx, 0, 0);
    }
    return QModelIndex();
}

void MessageModel::updateItem(QModelIndex indx)
{
    QModelIndex top = indx.parent();

    /* Emit a separate dataChanged signal for each possibly changed cell.
       This is a workaround for an inefficiency in Qt which causes the
       whole tree view to be redrawn when the topLeft != bottomRight.

       21-10-2007
     */
    QModelIndex col1 = index(indx.row(), 0, top);
    QModelIndex col2 = index(indx.row(), 1, top);
    QModelIndex col3 = index(indx.row(), 2, top);
    emit dataChanged(col1, col1);
    emit dataChanged(col2, col2);
    emit dataChanged(col3, col3);

    /* This is the code we'd like to have here.
    QModelIndex tl = index(indx.row(), 0, top);
    QModelIndex br = index(indx.row(), 2, top);
    emit dataChanged(tl, br);
     */
}

void MessageModel::clearContextList()
{
    int r = cntxtList.count();

    if (r <= 0) // no items
        return;

    for (int i = 0; i < r; ++i)
        delete cntxtList[i];

    cntxtList.clear();

    reset();
}

// since we don't add or remove single rows, update all at once...
void MessageModel::updateAll()
{
    reset();
}

void MessageModel::setTranslation(const iterator &it, const QString &translation)
{
    int c = it.contextNo();
    int m = it.messageNo();
    QModelIndex idx = modelIndex(c, m, 2);
    MessageItem *msg = it.current();
    msg->setTranslation(translation);
    emit dataChanged(idx, idx);
}

QModelIndex MessageModel::modelIndex(int context, int message, int column)
{
    QModelIndex idx = index(context, column);
    if (idx.isValid()) idx = index(message, column, idx);
    return idx;
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        ContextItem *c = static_cast<ContextItem *>(parent.internalPointer());
        if (c) return -1;
        return cntxtList[parent.row()]->messageItemsInList();
    }

    return cntxtList.count();
}

int MessageModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QLatin1String("");
        case 1:
            return tr("Context");
        case 2:
            return tr("Items");
        case 3:
            return tr("Index");
        }

        return QLatin1String("Error");
    } else if (role == Qt::DecorationRole && orientation == Qt::Horizontal && section == 0) {
        return qVariantFromValue(*pxObsolete);
    }

    return QVariant();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    ContextItem *cntxtItem = static_cast<ContextItem *>(index.internalPointer());
    if (cntxtItem) {
        if (row >= cntxtItem->messageItemsInList() || !index.isValid())
            return QVariant();

        MessageItem *msgItem = cntxtItem->messageItem(row);

        if (role == Qt::DisplayRole || (role == Qt::ToolTipRole && column > 0 && column < 3)) {
            switch (column) {
            case 0: // Done
                return QVariant();
            case 1: // Source text
                return msgItem->sourceText().simplified();
            case 2: // Translation
                return msgItem->translation().simplified();
            case 3: // Index
                return row;
            }
        }
        else if (role == Qt::DecorationRole && column == 0) {
            if (msgItem->message().type() == TranslatorMessage::Unfinished && msgItem->translation().isEmpty())
                return qVariantFromValue(*pxEmpty);
            else if (msgItem->message().type() == TranslatorMessage::Unfinished && msgItem->danger())
                return qVariantFromValue(*pxDanger);
            else if (msgItem->message().type() == TranslatorMessage::Finished && msgItem->danger())
                return qVariantFromValue(*pxWarning);
            else if (msgItem->message().type() == TranslatorMessage::Finished)
                return qVariantFromValue(*pxOn);
            else if (msgItem->message().type() == TranslatorMessage::Unfinished)
                return qVariantFromValue(*pxOff);
            else if (msgItem->message().type() == TranslatorMessage::Obsolete)
                return qVariantFromValue(*pxObsolete);
        }
        else if (role == SortRole) {
            switch (column) {
            case 0: // Status
                return msgItem->message().type() * 100 + !msgItem->translation().isEmpty() * 10 + !msgItem->danger();
            case 1: // Source text without certain characters
                return msgItem->sourceText().simplified().remove(QLatin1Char('&'));
            case 2: // Translation without certain characters
                return msgItem->translation().simplified().remove(QLatin1Char('&'));
            case 3: // Index
                return row;
            }
        }
        else if (role == Qt::ForegroundRole && column > 0 && msgItem->obsolete()) {
            return QBrush(Qt::darkGray);
        }
    } else {
        if (row >= cntxtList.count() || !index.isValid())
            return QVariant();

        ContextItem *cntxtItem = cntxtList.at(row);

        if (role == Qt::DisplayRole || (role == Qt::ToolTipRole && column == 1)) {
            switch (column) {
            case 0: // Done
                return QVariant();
            case 1: // Context
                return cntxtItem->context().simplified();
            case 2: // Items
                {
                    QString s;
                    const int itemCount = cntxtItem->messageItemsInList();
                    s.sprintf("%d/%d", cntxtItem->finishedCount(),
                        itemCount - cntxtItem->obsoleteCount());
                    return s;
                }
            case 3: // Index
                return row;
            }
        }
        else if (role == Qt::DecorationRole && column == 0) {
            if (cntxtItem->isObsolete())
                return qVariantFromValue(*pxObsolete);
            else if (cntxtItem->isFinished())
                return qVariantFromValue(*pxOn);
            else
                return qVariantFromValue(*pxOff);
        }
        else if (role == SortRole) {
            switch (column) {
            case 0: // Percent
                {
                    int totalItems = cntxtItem->messageItemsInList() - cntxtItem->obsoleteCount();
                    int percent = totalItems ? (100 * cntxtItem->finishedCount()) / totalItems : 100;
                    return percent * 10000 + totalItems;
                }
            case 1: // Context (same as display role)
                return cntxtItem->context().simplified();
            case 2: // Items
                return cntxtItem->messageItemsInList() - cntxtItem->obsoleteCount();
            case 3: // Index
                return row;
            }
        }
        else if (role == Qt::ForegroundRole && column > 0 && cntxtItem->isObsolete()) {
            return QBrush(Qt::darkGray);
        }
    }
    return QVariant();
}

bool MessageModel::finished(const QModelIndex &index)
{
    MessageItem *m = messageItem(index);
    return m->finished();
}

ContextItem *MessageModel::contextItem(int context) const
{
    if (context >= 0 && context < cntxtList.count()) return cntxtList[context];
    return 0;
}

/**
 * class MessageModelTranslator
 *
 *
 */
class MessageModelTranslator : public QTranslator
{
public:
    MessageModelTranslator(MessageModel *msgmodel): QTranslator(msgmodel)
    {
        //m_msgModel = msgmodel;
    }
    virtual ~MessageModelTranslator() {}
    virtual bool isEmpty() const;
    virtual QString translate(const char *context, const char *sourcetext, const char *comment = 0) const;
private:
    MessageModel *messageModel() const { return static_cast<MessageModel*>(parent()); }
};

bool MessageModelTranslator::isEmpty() const
{
    return messageModel()->isEmpty();
}

QString MessageModelTranslator::translate(const char *context, const char *sourcetext, const char *comment /*= 0*/) const
{
    MessageItem *m = messageModel()->findMessage(context, sourcetext, comment);
    return m ? m->translation() : QString();
}



MessageItem *MessageModel::messageItem(int context, int message) const
{
    ContextItem *c = contextItem(context);
    if (c && message >= 0 && message < c->messageItemsInList()) return c->messageItem(message);
    return 0;
}

MessageItem *MessageModel::findMessage(const char *context, const char *sourcetext, const char *comment /*= 0*/) const
{
    for (int c = 0; c < cntxtList.count(); ++c) {
        ContextItem *ctx = cntxtList.at(c);
        if (ctx->context() == QLatin1String(context)) {
            QList<MessageItem*> items = ctx->messageItemList();
            for (int i = 0; i < items.count(); ++i) {
                MessageItem *mi = items.at(i);
                if (mi->sourceText() == QLatin1String(sourcetext)) {
                    if (comment) {
                        if (mi->comment() != QLatin1String(comment)) continue;
                    }
                    return mi;
                }
            }
            break;
        }
    }
    return 0;
}

bool MessageModel::findMessage(int *contextNo, int *itemNo,
        const QString &findText, int findWhere,
        bool matchSubstring, Qt::CaseSensitivity cs)
{
    bool found = false;
    if (contextsInList() <= 0)
        return false;

    int pass = 0;
    int scopeNum = *contextNo;
    int itemNum = *itemNo;

    MessageItem *m = 0;

    // We want to search the scope we started from *again*, since we did not
    // necessarily search that *completely* when we started.
    // (Problaby we started somewhere in the middle of it.)
    // Therefore, "pass <=" and not "pass < "
    while (!found && pass <= contextsInList()) {
        ContextItem *c = contextList().at(scopeNum);
        for (int mit = itemNum; mit < c->messageItemsInList() ; ++mit) {
            m = c->messageItem(mit);
            QString searchText;
            switch (findWhere) {
                case SourceText:
                    searchText = m->sourceText();
                    break;
                case Translations:
                    searchText = m->translation();
                    break;
                case Comments:
                    searchText = m->comment();
                    break;
            }
            if (matchSubstring) {
                if (searchText.indexOf(findText,0, cs) >= 0) {
                    found = true;
                    break;
                }
            } else {
                if (cs == Qt::CaseInsensitive) {
                    if (findText.toLower() == searchText.toLower()) {
                        found = true;
                        break;
                    }
                } else {
                    if ( findText == searchText ) {
                        found = true;
                        break;
                    }
                }
            }

        }
        itemNum = 0;
        ++pass;

        ++scopeNum;
        if (scopeNum >= contextsInList()) {
            scopeNum = 0;
            //delayedMsg = tr("Search wrapped.");
        }
    }

    if (found) {
        *itemNo = itemNum;
        *contextNo = scopeNum;
    }
    return found;
}

bool MessageModel::load(const QString &fileName)
{
    MetaTranslator tor;
    tor.setXmlErrorHandler(&xmlErrorHandler);
    bool ok = tor.load(fileName);
    if (ok) {
        m_srcFileName = fileName;

        if(tor.codecForTr())
            m_codecForTr = tor.codecForTr()->name();
        int messageCount = 0;
        clearContextList();
        m_numFinished = 0;
        m_numNonobsolete = 0;

        TML all = tor.messages();
        QHash<QString, ContextItem*> contexts;

        m_srcWords = 0;
        m_srcChars = 0;
        m_srcCharsSpc = 0;

        foreach(const TranslatorMessage &mtm, all) {
            QCoreApplication::processEvents();
            ContextItem *c;
            if (contexts.contains(QLatin1String(mtm.context()))) {
                c = contexts.value( QLatin1String(mtm.context()));
            }
            else {
                c = createContextItem(tor.toUnicode(mtm.context(), mtm.utf8()));;
                appendContextItem(c);
                contexts.insert(QLatin1String(mtm.context()), c);
            }
            if (QByteArray(mtm.sourceText()) == ContextComment) {
                c->appendToComment(tor.toUnicode(mtm.comment(), mtm.utf8()));
            }
            else {
                MessageItem *tmp = new MessageItem(mtm, tor.toUnicode(mtm.sourceText(),
                    mtm.utf8()), tor.toUnicode(mtm.comment(), mtm.utf8()), mtm.fileName(), mtm.lineNumber(), c);
                if (mtm.type() != TranslatorMessage::Obsolete) {
                    m_numNonobsolete++;
                    //if (mtm.type() == TranslatorMessage::Finished)
                        //tmp->setFinished(true);
                        //++m_numFinished;
                    doCharCounting(tmp->sourceText(), m_srcWords, m_srcChars, m_srcCharsSpc);
                }
                else {
                    c->incrementObsoleteCount();
                }
                c->appendMessageItem(tmp);
                ++messageCount;
            }
        }

        // Try to detect the correct language in the following order
        // 1. Look for the language attribute in the ts
        //   if that fails
        // 2. Guestimate the language from the filename (expecting the qt_{en,de}.ts convention)
        //   if that fails
        // 3. Retrieve the locale from the system.
        QString lang = tor.languageCode();
        if (lang.isEmpty()) {
            int pos_sep = fileName.indexOf(QLatin1Char('_'));
            if (pos_sep != -1 && pos_sep + 3 <= fileName.length()) {
                lang = fileName.mid(pos_sep + 1, 2);
            }
        }
        QLocale::Language l;
        QLocale::Country c;
        MetaTranslator::languageAndCountry(lang, &l, &c);
        if (l == QLocale::C) {
            QLocale sys;
            l = sys.language();
            c = sys.country();
        }
        setLanguage(l);
        setCountry(c);

        m_numMessages = messageCount;
        updateAll();
        setModified(false);
    }

    return ok;
}

bool MessageModel::save(const QString &fileName)
{
    MetaTranslator tor;
    tor.setXmlErrorHandler(&xmlErrorHandler);
    MessageItem *m;
    for (iterator it = begin(); (m = it.current()); ++it) {
        tor.insert(m->message());
    }
    QLocale locale(m_language, m_country);
    if (m_country == QLocale::AnyCountry) {
        QString languageCode = locale.name().section(QLatin1Char('_'), 0, 0);
        if (languageCode.length() <= 3) {
            tor.setLanguageCode(languageCode);
        }
    } else {
        QString languageCode = locale.name();
        tor.setLanguageCode(languageCode);
    }
    tor.setCodecForTr(m_codecForTr.constData());
    bool ok = tor.save(fileName);
    if (ok) setModified(false);
    return ok;
}

bool MessageModel::release(const QString& fileName,
                bool verbose /*= false*/, bool ignoreUnfinished /*= false*/,
                Translator::SaveMode mode /*= Translator::Stripped */)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        bool ok =  release(&file, verbose, ignoreUnfinished, mode);
        file.close();
        return ok;
    }
    return false;
}

bool MessageModel::release(QIODevice *iod,
                bool verbose /*= false*/, bool ignoreUnfinished /*= false*/,
                Translator::SaveMode mode  /*= Translator::Stripped */)
{
    MetaTranslator tor;
    tor.setXmlErrorHandler(&xmlErrorHandler);
    MessageItem *m;
    QLocale locale(m_language, m_country);
    tor.setLanguageCode(locale.name());
    for (MessageModel::iterator it = begin() ; (m = it.current()) ; ++it) {
        tor.insert(m->message());
    }
    return tor.release(iod, verbose, ignoreUnfinished, mode);
}


void MessageModel::doCharCounting(const QString& text, int& trW, int& trC, int& trCS)
{
    trCS += text.length();
    bool inWord = false;
    for (int i=0; i<(int)text.length(); ++i) {
        if (text[i].isLetterOrNumber() || text[i] == QLatin1Char('_')) {
            if (!inWord) {
                ++trW;
                inWord = true;
            }
        } else {
            inWord = false;
        }
        if (!text[i].isSpace())
            trC++;
    }
}

QLocale::Language MessageModel::language() const
{
    return m_language;
}

void MessageModel::setLanguage(QLocale::Language lang)
{
    if (m_language != lang) {
        m_language = lang;
        emit languageChanged(m_language);
        setModified(true);
    }
}

QLocale::Country MessageModel::country() const
{
    return m_country;
}

void MessageModel::setCountry(QLocale::Country country)
{
    if (m_country != country) {
        m_country = country;
        setModified(true);
    }
}

void MessageModel::updateStatistics()
{
    QList<ContextItem *> ctxtList;
    QList<MessageItem *> msgList;
    const MessageItem *mi;
    int trW = 0;
    int trC = 0;
    int trCS = 0;

    for (MessageModel::iterator it = begin(); (mi = it.current()); ++it) {
        if (mi->finished() && !(mi->message().type() == TranslatorMessage::Obsolete))
            doCharCounting(mi->translation(), trW, trC, trCS);
    }

    emit statsChanged(m_srcWords, m_srcChars, m_srcCharsSpc, trW, trC, trCS);
}

QTranslator *MessageModel::translator()
{
    if (!m_translator) m_translator = static_cast<QTranslator*>(new MessageModelTranslator(this));
    return m_translator;
}


/******************************************************************************
 *
 * MessageModel::iterator IMPLEMENTATION
 *
 *****************************************************************************/
MessageModel::iterator::iterator(ContextList *contextList, int contextNo /*= 0*/, int itemNo /*=0*/)
: m_contextList(contextList), m_contextNo(contextNo), m_itemNo(itemNo)
{
}


MessageModel::iterator::iterator(MessageModel *model, int contextNo /*= 0*/, int itemNo /*=0*/)
: m_contextList(&model->cntxtList), m_contextNo(contextNo), m_itemNo(itemNo)
{
}

MessageModel::iterator::iterator(const MessageModel::iterator& other)
{
    *this = other;
}

MessageModel::iterator &MessageModel::iterator::operator=(const MessageModel::iterator& other)
{
    m_contextList = other.m_contextList;
    m_contextNo = other.m_contextNo;
    m_itemNo = other.m_itemNo;
    return *this;
}

MessageModel::iterator &MessageModel::iterator::operator++()
{
    ++m_itemNo;
    if (m_itemNo >= m_contextList->at(m_contextNo)->messageItemsInList()) {
        ++m_contextNo;
        if (m_contextNo != m_contextList->count())
            m_itemNo = 0;
    }
    return *this;
}

MessageItem *MessageModel::iterator::current() const
{
    if (m_contextNo >= 0 && m_contextNo < m_contextList->count()) {
        if (m_itemNo >= 0 && m_itemNo < m_contextList->at(m_contextNo)->messageItemsInList()) {
            return m_contextList->at(m_contextNo)->messageItem(m_itemNo);
        }
    }
    return 0;
}

MessageItem *MessageModel::iterator::operator*() const
{
    return current();
}

void MessageModel::iterator::reset()
{
    m_contextNo = 0;
    m_itemNo = 0;
}

bool MessageModel::SimpleXmlErrorHandler::fatalError(const QXmlParseException& exception)
{
    QString msg;
    msg.sprintf( "Parse error at line %d, column %d (%s).",
                 exception.lineNumber(), exception.columnNumber(),
                 exception.message().toLatin1().data() );
    QMessageBox::information(0, QObject::tr("Qt Linguist"), msg );
    return false;
}

/******************************************************************************
 *
 * ContextList IMPLEMENTATION
 *
 *****************************************************************************/
ContextList::ContextList()
    : m_modified(false)
{
}

ContextItem *ContextList::contextItem(int context) const
{
    if (context >= 0 && context < count()) return at(context);
    return 0;
}

MessageItem *ContextList::messageItem(int context, int message) const
{
    ContextItem *c = contextItem(context);
    if (c && message >= 0 && message < c->messageItemsInList())
        return c->messageItem(message);
    return 0;
}

void ContextList::setModified(bool isModified)
{
    if (m_modified != isModified) {
        m_modified = isModified;
        emit modifiedChanged(m_modified);
    }
}

QT_END_NAMESPACE
