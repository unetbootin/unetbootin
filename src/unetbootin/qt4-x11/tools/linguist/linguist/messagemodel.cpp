/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "messagemodel.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QTextCodec>

#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QTextDocument>

#include <private/qtranslator_p.h>

#include <limits.h>

QT_BEGIN_NAMESPACE

/******************************************************************************
 *
 * MessageItem
 *
 *****************************************************************************/

MessageItem::MessageItem(const TranslatorMessage &message)
  : m_message(message),
    m_danger(false)
{
    if (m_message.translation().isEmpty())
        m_message.setTranslation(QString());
}


bool MessageItem::compare(const QString &findText, bool matchSubstring,
    Qt::CaseSensitivity cs) const
{
    return matchSubstring
        ? text().indexOf(findText, 0, cs) >= 0
        : text().compare(findText, cs) == 0;
}

/******************************************************************************
 *
 * ContextItem
 *
 *****************************************************************************/

ContextItem::ContextItem(const QString &context)
  : m_context(context),
    m_finishedCount(0),
    m_finishedDangerCount(0),
    m_unfinishedDangerCount(0),
    m_nonobsoleteCount(0)
{}

void ContextItem::appendToComment(const QString &str)
{
    if (!m_comment.isEmpty())
        m_comment += QLatin1String("\n\n");
    m_comment += str;
}

MessageItem *ContextItem::messageItem(int i) const
{
    if (i >= 0 && i < msgItemList.count())
        return const_cast<MessageItem *>(&msgItemList[i]);
    Q_ASSERT(i >= 0 && i < msgItemList.count());
    return 0;
}

MessageItem *ContextItem::findMessage(const QString &sourcetext, const QString &comment) const
{
    for (int i = 0; i < messageCount(); ++i) {
        MessageItem *mi = messageItem(i);
        if (mi->text() == sourcetext && mi->comment() == comment)
            return mi;
    }
    return 0;
}

/******************************************************************************
 *
 * DataModel
 *
 *****************************************************************************/

DataModel::DataModel(QObject *parent)
  : QObject(parent),
    m_modified(false),
    m_numMessages(0),
    m_srcWords(0),
    m_srcChars(0),
    m_srcCharsSpc(0),
    m_language(QLocale::Language(-1)),
    m_sourceLanguage(QLocale::Language(-1)),
    m_country(QLocale::Country(-1)),
    m_sourceCountry(QLocale::Country(-1))
{}

QStringList DataModel::normalizedTranslations(const MessageItem &m) const
{
    return Translator::normalizedTranslations(m.message(), m_language, m_country);
}

ContextItem *DataModel::contextItem(int context) const
{
    if (context >= 0 && context < m_contextList.count())
        return const_cast<ContextItem *>(&m_contextList[context]);
    Q_ASSERT(context >= 0 && context < m_contextList.count());
    return 0;
}

MessageItem *DataModel::messageItem(const DataIndex &index) const
{
    if (ContextItem *c = contextItem(index.context()))
        return c->messageItem(index.message());
    return 0;
}

ContextItem *DataModel::findContext(const QString &context) const
{
    for (int c = 0; c < m_contextList.count(); ++c) {
        ContextItem *ctx = contextItem(c);
        if (ctx->context() == context)
            return ctx;
    }
    return 0;
}

MessageItem *DataModel::findMessage(const QString &context,
    const QString &sourcetext, const QString &comment) const
{
    if (ContextItem *ctx = findContext(context))
        return ctx->findMessage(sourcetext, comment);
    return 0;
}

static int calcMergeScore(const DataModel *one, const DataModel *two)
{
    int inBoth = 0;
    for (int i = 0; i < two->contextCount(); ++i) {
        ContextItem *oc = two->contextItem(i);
        if (ContextItem *c = one->findContext(oc->context())) {
            for (int j = 0; j < oc->messageCount(); ++j) {
                MessageItem *m = oc->messageItem(j);
                if (c->findMessage(m->text(), m->comment()) >= 0)
                    ++inBoth;
            }
        }
    }
    return inBoth * 100 / two->messageCount();
}

bool DataModel::isWellMergeable(const DataModel *other) const
{
    if (!other->messageCount() || !messageCount())
        return true;

    return calcMergeScore(this, other) + calcMergeScore(other, this) > 90;
}

bool DataModel::load(const QString &fileName, bool *langGuessed, QWidget *parent)
{
    Translator tor;
    ConversionData cd;
    bool ok = tor.load(fileName, cd, QLatin1String("auto"));
    if (!ok) {
        QMessageBox::warning(parent, QObject::tr("Qt Linguist"), cd.error());
        return false;
    }

    QList<TranslatorMessage> dupes = tor.findDuplicates();
    if (!dupes.isEmpty()) {
        QString err = tr("<qt>Duplicate messages found in '%1':").arg(Qt::escape(fileName));
        int numdups = 0;
        foreach (const TranslatorMessage &msg, dupes) {
            if (++numdups >= 5) {
                err += tr("<p>[more duplicates omitted]");
                break;
            }
            err += tr("<p>* Context: %1<br>* Source: %2")
                    .arg(Qt::escape(msg.context()), Qt::escape(msg.sourceText()));
            if (!msg.comment().isEmpty())
                err += tr("<br>* Comment: %3").arg(Qt::escape(msg.comment()));
        }
        QMessageBox::warning(parent, QObject::tr("Qt Linguist"), err);
    }

    m_srcFileName = fileName;
    m_codecName = tor.codecName();
    m_relativeLocations = (tor.locationsType() == Translator::RelativeLocations);
    m_extra = tor.extras();
    m_contextList.clear();
    m_numMessages = 0;

    QHash<QString, int> contexts;

    m_srcWords = 0;
    m_srcChars = 0;
    m_srcCharsSpc = 0;

    foreach (const TranslatorMessage &msg, tor.messages()) {
        if (!contexts.contains(msg.context())) {
            contexts.insert(msg.context(), m_contextList.size());
            m_contextList.append(ContextItem(msg.context()));
        }

        ContextItem *c = contextItem(contexts.value(msg.context()));
        if (msg.sourceText() == QLatin1String(ContextComment)) {
            c->appendToComment(msg.comment());
        } else {
            MessageItem tmp(msg);
            if (msg.type() == TranslatorMessage::Finished)
                c->incrementFinishedCount();
            if (msg.type() != TranslatorMessage::Obsolete) {
                doCharCounting(tmp.text(), m_srcWords, m_srcChars, m_srcCharsSpc);
                doCharCounting(tmp.pluralText(), m_srcWords, m_srcChars, m_srcCharsSpc);
                c->incrementNonobsoleteCount();
            }
            c->appendMessage(tmp);
            ++m_numMessages;
        }
    }

    // Try to detect the correct language in the following order
    // 1. Look for the language attribute in the ts
    //   if that fails
    // 2. Guestimate the language from the filename
    //   (expecting the qt_{en,de}.ts convention)
    //   if that fails
    // 3. Retrieve the locale from the system.
    *langGuessed = false;
    QString lang = tor.languageCode();
    if (lang.isEmpty()) {
        lang = QFileInfo(fileName).baseName();
        int pos = lang.indexOf(QLatin1Char('_'));
        if (pos != -1 && pos + 3 == lang.length())
            lang = fileName.mid(pos + 1);
        else
            lang.clear();
        *langGuessed = true;
    }
    QLocale::Language l;
    QLocale::Country c;
    Translator::languageAndCountry(lang, &l, &c);
    if (l == QLocale::C) {
        QLocale sys;
        l = sys.language();
        c = sys.country();
        *langGuessed = true;
    }
    setLanguageAndCountry(l, c);
    // Try to detect the correct source language in the following order
    // 1. Look for the language attribute in the ts
    //   if that fails
    // 2. Assume English
    lang = tor.sourceLanguageCode();
    if (lang.isEmpty()) {
        l = QLocale::C;
        c = QLocale::AnyCountry;
    } else {
        Translator::languageAndCountry(lang, &l, &c);
    }
    setSourceLanguageAndCountry(l, c);

    setModified(false);

    return true;
}

bool DataModel::save(const QString &fileName, QWidget *parent)
{
    Translator tor;
    for (DataModelIterator it(this); it.isValid(); ++it)
        tor.append(it.current()->message());

    tor.setLanguageCode(Translator::makeLanguageCode(m_language, m_country));
    tor.setSourceLanguageCode(Translator::makeLanguageCode(m_sourceLanguage, m_sourceCountry));
    tor.setCodecName(m_codecName);
    tor.setLocationsType(m_relativeLocations ? Translator::RelativeLocations
                                             : Translator::AbsoluteLocations);
    tor.setExtras(m_extra);
    ConversionData cd;
    bool ok = tor.save(fileName, cd, QLatin1String("auto"));
    if (ok)
        setModified(false);
    else
        QMessageBox::warning(parent, QObject::tr("Qt Linguist"), cd.error());
    return ok;
}

bool DataModel::saveAs(const QString &newFileName, QWidget *parent)
{
    if (!save(newFileName, parent))
        return false;
    m_srcFileName = newFileName;
    return true;
}

bool DataModel::release(const QString &fileName, bool verbose, bool ignoreUnfinished,
    TranslatorSaveMode mode, QWidget *parent)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(parent, QObject::tr("Qt Linguist"),
            tr("Cannot create '%2': %1").arg(file.errorString()).arg(fileName));
        return false;
    }
    Translator tor;
    QLocale locale(m_language, m_country);
    tor.setLanguageCode(locale.name());
    for (DataModelIterator it(this); it.isValid(); ++it)
        tor.append(it.current()->message());
    ConversionData cd;
    cd.m_verbose = verbose;
    cd.m_ignoreUnfinished = ignoreUnfinished;
    cd.m_saveMode = mode;
    bool ok = tor.release(&file, cd);
    if (!ok)
        QMessageBox::warning(parent, QObject::tr("Qt Linguist"), cd.error());
    return ok;
}

void DataModel::doCharCounting(const QString &text, int &trW, int &trC, int &trCS)
{
    trCS += text.length();
    bool inWord = false;
    for (int i = 0; i < text.length(); ++i) {
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

void DataModel::setLanguageAndCountry(QLocale::Language lang, QLocale::Country country)
{
    if (m_language == lang && m_country == country)
        return;
    m_language = lang;
    m_country = country;

    if (lang == QLocale::C || uint(lang) > uint(QLocale::LastLanguage)) // XXX does this make any sense?
        lang = QLocale::English;
    QByteArray rules;
    getNumerusInfo(lang, country, &rules, &m_numerusForms);
    m_localizedLanguage = QCoreApplication::translate("MessageEditor", QLocale::languageToString(lang).toAscii());
    m_countRefNeeds.clear();
    for (int i = 0; i < rules.size(); ++i) {
        m_countRefNeeds.append(!(rules.at(i) == Q_EQ && (i == (rules.size() - 2) || rules.at(i + 2) == (char)Q_NEWRULE)));
        while (++i < rules.size() && rules.at(i) != (char)Q_NEWRULE) {}
    }
    m_countRefNeeds.append(true);
    emit languageChanged();
    setModified(true);
}

void DataModel::setSourceLanguageAndCountry(QLocale::Language lang, QLocale::Country country)
{
    if (m_sourceLanguage == lang && m_sourceCountry == country)
        return;
    m_sourceLanguage = lang;
    m_sourceCountry = country;
    setModified(true);
}

void DataModel::updateStatistics()
{
    int trW = 0;
    int trC = 0;
    int trCS = 0;

    for (DataModelIterator it(this); it.isValid(); ++it) {
        const MessageItem *mi = it.current();
        if (mi->isFinished())
            foreach (const QString &trnsl, mi->translations())
                doCharCounting(trnsl, trW, trC, trCS);
    }

    emit statsChanged(m_srcWords, m_srcChars, m_srcCharsSpc, trW, trC, trCS);
}

void DataModel::setModified(bool isModified)
{
    if (m_modified == isModified)
        return;
    m_modified = isModified;
    emit modifiedChanged();
}

QString DataModel::prettifyPlainFileName(const QString &fn)
{
    static QString workdir = QDir::currentPath() + QLatin1Char('/');

    return QDir::toNativeSeparators(fn.startsWith(workdir) ? fn.mid(workdir.length()) : fn);
}

QString DataModel::prettifyFileName(const QString &fn)
{
    if (fn.startsWith(QLatin1Char('=')))
        return QLatin1Char('=') + prettifyPlainFileName(fn.mid(1));
    else
        return prettifyPlainFileName(fn);
}

/******************************************************************************
 *
 * DataModelIterator 
 *
 *****************************************************************************/

DataModelIterator::DataModelIterator(DataModel *model, int context, int message)
  : DataIndex(context, message), m_model(model)
{
}

bool DataModelIterator::isValid() const
{
    return m_context < m_model->m_contextList.count();
}

void DataModelIterator::operator++()
{
    ++m_message;
    if (m_message >= m_model->m_contextList.at(m_context).messageCount()) {
        ++m_context;
        m_message = 0;
    }
}

MessageItem *DataModelIterator::current() const
{
    return m_model->messageItem(*this);
}


/******************************************************************************
 *
 * MultiMessageItem
 *
 *****************************************************************************/

MultiMessageItem::MultiMessageItem(const MessageItem *m)
    : m_text(m->text()),
      m_pluralText(m->pluralText()),
      m_comment(m->comment()),
      m_nonnullCount(0),
      m_nonobsoleteCount(0),
      m_editableCount(0),
      m_unfinishedCount(0)
{
}

/******************************************************************************
 *
 * MultiContextItem
 *
 *****************************************************************************/

MultiContextItem::MultiContextItem(int oldCount, ContextItem *ctx, bool writable)
    : m_context(ctx->context()),
      m_comment(ctx->comment()),
      m_finishedCount(0),
      m_editableCount(0),
      m_nonobsoleteCount(0)
{
    QList<MessageItem *> mList;
    QList<MessageItem *> eList;
    for (int j = 0; j < ctx->messageCount(); ++j) {
        MessageItem *m = ctx->messageItem(j);
        mList.append(m);
        eList.append(0);
        m_multiMessageList.append(MultiMessageItem(m));
    }
    for (int i = 0; i < oldCount; ++i) {
        m_messageLists.append(eList);
        m_writableMessageLists.append(0);
        m_contextList.append(0);
    }
    m_messageLists.append(mList);
    m_writableMessageLists.append(writable ? &m_messageLists.last() : 0);
    m_contextList.append(ctx);
}

void MultiContextItem::appendEmptyModel()
{
    QList<MessageItem *> eList;
    for (int j = 0; j < messageCount(); ++j)
        eList.append(0);
    m_messageLists.append(eList);
    m_writableMessageLists.append(0);
    m_contextList.append(0);
}

void MultiContextItem::assignLastModel(ContextItem *ctx, bool writable)
{
    if (writable)
        m_writableMessageLists.last() = &m_messageLists.last();
    m_contextList.last() = ctx;
}

// XXX this is not needed, yet
void MultiContextItem::moveModel(int oldPos, int newPos)
{
    m_contextList.insert(newPos, m_contextList[oldPos]);
    m_messageLists.insert(newPos, m_messageLists[oldPos]);
    m_writableMessageLists.insert(newPos, m_writableMessageLists[oldPos]);
    removeModel(oldPos < newPos ? oldPos : oldPos + 1);
}

void MultiContextItem::removeModel(int pos)
{
    m_contextList.removeAt(pos);
    m_messageLists.removeAt(pos);
    m_writableMessageLists.removeAt(pos);
}

void MultiContextItem::putMessageItem(int pos, MessageItem *m)
{
    m_messageLists.last()[pos] = m;
}

void MultiContextItem::appendMessageItem(MessageItem *m)
{
    for (int i = 0; i < m_messageLists.count() - 1; ++i)
        m_messageLists[i].append(0);
    m_messageLists.last().append(m);
    m_multiMessageList.append(MultiMessageItem(m));
}

void MultiContextItem::removeMultiMessageItem(int pos)
{
    for (int i = 0; i < m_messageLists.count(); ++i)
        m_messageLists[i].removeAt(pos);
    m_multiMessageList.removeAt(pos);
}

int MultiContextItem::firstNonobsoleteMessageIndex(int msgIdx) const
{
    for (int i = 0; i < m_messageLists.size(); ++i)
        if (m_messageLists[i][msgIdx] && !m_messageLists[i][msgIdx]->isObsolete())
            return i;
    return -1;
}

int MultiContextItem::findMessage(const QString &sourcetext, const QString &comment) const
{
    for (int i = 0, cnt = messageCount(); i < cnt; ++i) {
        MultiMessageItem *m = multiMessageItem(i);
        if (m->text() == sourcetext && m->comment() == comment)
            return i;
    }
    return -1;
}

/******************************************************************************
 *
 * MultiDataModel
 *
 *****************************************************************************/

static const uchar paletteRGBs[7][3] = {
    { 236, 244, 255 }, // blue
    { 236, 255, 255 }, // cyan
    { 236, 255, 232 }, // green
    { 255, 255, 230 }, // yellow
    { 255, 242, 222 }, // orange
    { 255, 236, 236 }, // red
    { 252, 236, 255 }  // purple
};

MultiDataModel::MultiDataModel(QObject *parent) :
    QObject(parent),
    m_numFinished(0),
    m_numEditable(0),
    m_numMessages(0),
    m_modified(false)
{
    for (int i = 0; i < 7; ++i)
        m_colors[i] = QColor(paletteRGBs[i][0], paletteRGBs[i][1], paletteRGBs[i][2]);

    m_bitmap = QBitmap(8, 8);
    m_bitmap.clear();
    QPainter p(&m_bitmap);
    for (int j = 0; j < 8; ++j)
        for (int k = 0; k < 8; ++k)
            if ((j + k) & 4)
                p.drawPoint(j, k);
}

MultiDataModel::~MultiDataModel()
{
    qDeleteAll(m_dataModels);
}

QBrush MultiDataModel::brushForModel(int model) const
{
    QBrush brush(m_colors[model % 7]);
    if (!isModelWritable(model))
        brush.setTexture(m_bitmap);
    return brush;
}

bool MultiDataModel::isWellMergeable(const DataModel *dm) const
{
    if (!dm->messageCount() || !messageCount())
        return true;

    int inBothNew = 0;
    for (int i = 0; i < dm->contextCount(); ++i) {
        ContextItem *c = dm->contextItem(i);
        if (MultiContextItem *mc = findContext(c->context())) {
            for (int j = 0; j < c->messageCount(); ++j) {
                MessageItem *m = c->messageItem(j);
                if (mc->findMessage(m->text(), m->comment()) >= 0)
                    ++inBothNew;
            }
        }
    }
    int newRatio = inBothNew * 100 / dm->messageCount();

    int inBothOld = 0;
    for (int k = 0; k < contextCount(); ++k) {
        MultiContextItem *mc = multiContextItem(k);
        if (ContextItem *c = dm->findContext(mc->context())) {
            for (int j = 0; j < mc->messageCount(); ++j) {
                MultiMessageItem *m = mc->multiMessageItem(j);
                if (c->findMessage(m->text(), m->comment()))
                    ++inBothOld;
            }
        }
    }
    int oldRatio = inBothOld * 100 / messageCount();

    return newRatio + oldRatio > 90;
}

void MultiDataModel::append(DataModel *dm, bool readWrite)
{
    int insCol = modelCount() + 1;
    m_msgModel->beginInsertColumns(QModelIndex(), insCol, insCol);
    m_dataModels.append(dm);
    for (int j = 0; j < contextCount(); ++j) {
        m_msgModel->beginInsertColumns(m_msgModel->createIndex(j, 0, 0), insCol, insCol);
        m_multiContextList[j].appendEmptyModel();
        m_msgModel->endInsertColumns();
    }
    m_msgModel->endInsertColumns();
    for (int i = 0; i < dm->contextCount(); ++i) {
        ContextItem *c = dm->contextItem(i);
        int mcx = findContextIndex(c->context());
        if (mcx >= 0) {
            MultiContextItem *mc = multiContextItem(mcx);
            mc->assignLastModel(c, readWrite);
            for (int j = 0; j < c->messageCount(); ++j) {
                MessageItem *m = c->messageItem(j);
                int msgIdx = mc->findMessage(m->text(), m->comment());
                if (msgIdx >= 0) {
                    mc->putMessageItem(msgIdx, m);
                } else {
                    int msgCnt = mc->messageCount();
                    m_msgModel->beginInsertRows(m_msgModel->createIndex(mcx, 0, 0), msgCnt, msgCnt);
                    mc->appendMessageItem(m);
                    m_msgModel->endInsertRows();
                    ++m_numMessages;
                }
            }
        } else {
            MultiContextItem item(modelCount() - 1, c, readWrite);
            m_msgModel->beginInsertRows(QModelIndex(), contextCount(), contextCount());
            m_multiContextList.append(item);
            m_msgModel->endInsertRows();
            m_numMessages += item.messageCount();
        }
    }
    dm->setWritable(readWrite);
    updateCountsOnAdd(modelCount() - 1, readWrite);
    connect(dm, SIGNAL(modifiedChanged()), SLOT(onModifiedChanged()));
    connect(dm, SIGNAL(languageChanged()), SLOT(onLanguageChanged()));
    connect(dm, SIGNAL(statsChanged(int,int,int,int,int,int)), SIGNAL(statsChanged(int,int,int,int,int,int)));
    emit modelAppended();
}

void MultiDataModel::close(int model)
{
    if (m_dataModels.count() == 1) {
        closeAll();
    } else {
        updateCountsOnRemove(model, isModelWritable(model));
        int delCol = model + 1;
        m_msgModel->beginRemoveColumns(QModelIndex(), delCol, delCol);
        for (int i = m_multiContextList.size(); --i >= 0;) {
            m_msgModel->beginRemoveColumns(m_msgModel->createIndex(i, 0, 0), delCol, delCol);
            m_multiContextList[i].removeModel(model);
            m_msgModel->endRemoveColumns();
        }
        delete m_dataModels.takeAt(model);
        m_msgModel->endRemoveColumns();
        emit modelDeleted(model);
        for (int i = m_multiContextList.size(); --i >= 0;) {
            MultiContextItem &mc = m_multiContextList[i];
            QModelIndex contextIdx = m_msgModel->createIndex(i, 0, 0);
            for (int j = mc.messageCount(); --j >= 0;)
                if (mc.multiMessageItem(j)->isEmpty()) {
                    m_msgModel->beginRemoveRows(contextIdx, j, j);
                    mc.removeMultiMessageItem(j);
                    m_msgModel->endRemoveRows();
                    --m_numMessages;
                }
            if (!mc.messageCount()) {
                m_msgModel->beginRemoveRows(QModelIndex(), i, i);
                m_multiContextList.removeAt(i);
                m_msgModel->endRemoveRows();
            }
        }
        onModifiedChanged();
    }
}

void MultiDataModel::closeAll()
{
    m_numFinished = 0;
    m_numEditable = 0;
    m_numMessages = 0;
    int delCol = m_dataModels.count();
    m_msgModel->beginRemoveColumns(QModelIndex(), 1, delCol);
    for (int i = m_multiContextList.size(); --i >= 0;) {
        m_msgModel->beginRemoveColumns(m_msgModel->createIndex(i, 0, 0), 1, delCol);
        m_msgModel->endRemoveColumns();
    }
    qDeleteAll(m_dataModels);
    m_dataModels.clear();
    m_multiContextList.clear();
    m_msgModel->endRemoveColumns();
    m_msgModel->reset();
    emit allModelsDeleted();
    onModifiedChanged();
}

// XXX this is not needed, yet
void MultiDataModel::moveModel(int oldPos, int newPos)
{
    int delPos = oldPos < newPos ? oldPos : oldPos + 1;
    m_dataModels.insert(newPos, m_dataModels[oldPos]);
    m_dataModels.removeAt(delPos);
    for (int i = 0; i < m_multiContextList.size(); ++i)
        m_multiContextList[i].moveModel(oldPos, newPos);
}

QStringList MultiDataModel::prettifyFileNames(const QStringList &names)
{
    QStringList out;

    foreach (const QString &name, names)
        out << DataModel::prettifyFileName(name);
    return out;
}

QString MultiDataModel::condenseFileNames(const QStringList &names)
{
    if (names.isEmpty())
        return QString();

    if (names.count() < 2)
        return names.first();

    QString prefix = names.first();
    if (prefix.startsWith(QLatin1Char('=')))
        prefix.remove(0, 1);
    QString suffix = prefix;
    for (int i = 1; i < names.count(); ++i) {
        QString fn = names[i];
        if (fn.startsWith(QLatin1Char('=')))
            fn.remove(0, 1);
        for (int j = 0; j < prefix.length(); ++j)
            if (fn[j] != prefix[j]) {
                if (j < prefix.length()) {
                    while (j > 0 && prefix[j - 1].isLetterOrNumber())
                        --j;
                    prefix.truncate(j);
                }
                break;
            }
        int fnl = fn.length() - 1;
        int sxl = suffix.length() - 1;
        for (int k = 0; k <= sxl; ++k)
            if (fn[fnl - k] != suffix[sxl - k]) {
                if (k < sxl) {
                    while (k > 0 && suffix[sxl - k + 1].isLetterOrNumber())
                        --k;
                    if (prefix.length() + k > fnl)
                        --k;
                    suffix.remove(0, sxl - k + 1);
                }
                break;
            }
    }
    QString ret = prefix + QLatin1Char('{');
    int pxl = prefix.length();
    int sxl = suffix.length();
    for (int j = 0; j < names.count(); ++j) {
        if (j)
            ret += QLatin1Char(',');
        int off = pxl;
        QString fn = names[j];
        if (fn.startsWith(QLatin1Char('='))) {
            ret += QLatin1Char('=');
            ++off;
        }
        ret += fn.mid(off, fn.length() - sxl - off);
    }
    ret += QLatin1Char('}') + suffix;
    return ret;
}

QStringList MultiDataModel::srcFileNames(bool pretty) const
{
    QStringList names;
    foreach (DataModel *dm, m_dataModels)
        names << (dm->isWritable() ? QString() : QString::fromLatin1("=")) + dm->srcFileName(pretty);
    return names;
}

QString MultiDataModel::condensedSrcFileNames(bool pretty) const
{
    return condenseFileNames(srcFileNames(pretty));
}

bool MultiDataModel::isModified() const
{
    foreach (const DataModel *mdl, m_dataModels)
        if (mdl->isModified())
            return true;
    return false;
}

void MultiDataModel::onModifiedChanged()
{
    bool modified = isModified();
    if (modified != m_modified) {
        emit modifiedChanged(modified);
        m_modified = modified;
    }
}

void MultiDataModel::onLanguageChanged()
{
    int i = 0;
    while (sender() != m_dataModels[i])
        ++i;
    emit languageChanged(i);
}

int MultiDataModel::isFileLoaded(const QString &name) const
{
    for (int i = 0; i < m_dataModels.size(); ++i)
        if (m_dataModels[i]->srcFileName() == name)
            return i;
    return -1;
}

int MultiDataModel::findContextIndex(const QString &context) const
{
    for (int i = 0; i < m_multiContextList.size(); ++i) {
        const MultiContextItem &mc = m_multiContextList[i];
        if (mc.context() == context)
            return i;
    }
    return -1;
}

MultiContextItem *MultiDataModel::findContext(const QString &context) const
{
    for (int i = 0; i < m_multiContextList.size(); ++i) {
        const MultiContextItem &mc = m_multiContextList[i];
        if (mc.context() == context)
            return const_cast<MultiContextItem *>(&mc);
    }
    return 0;
}

MessageItem *MultiDataModel::messageItem(const MultiDataIndex &index, int model) const
{
    if (index.context() < contextCount() && model >= 0 && model < modelCount()) {
        MultiContextItem *mc = multiContextItem(index.context());
        if (index.message() < mc->messageCount())
            return mc->messageItem(model, index.message());
    }
    Q_ASSERT(model >= 0 && model < modelCount());
    Q_ASSERT(index.context() < contextCount());
    return 0;
}

void MultiDataModel::setTranslation(const MultiDataIndex &index, const QString &translation)
{
    MessageItem *m = messageItem(index);
    if (translation == m->translation())
        return;
    m->setTranslation(translation);
    setModified(index.model(), true);
    emit translationChanged(index);
}

void MultiDataModel::setFinished(const MultiDataIndex &index, bool finished)
{
    MultiContextItem *mc = multiContextItem(index.context());
    MultiMessageItem *mm = mc->multiMessageItem(index.message());
    ContextItem *c = contextItem(index);
    MessageItem *m = messageItem(index);
    TranslatorMessage::Type type = m->type();
    if (type == TranslatorMessage::Unfinished && finished) {
        m->setType(TranslatorMessage::Finished);
        mm->decrementUnfinishedCount();
        if (!mm->countUnfinished()) {
            incrementFinishedCount();
            mc->incrementFinishedCount();
            emit multiContextDataChanged(index);
        }
        c->incrementFinishedCount();
        if (m->danger()) {
            c->incrementFinishedDangerCount();
            c->decrementUnfinishedDangerCount();
            if (!c->unfinishedDangerCount()
                || c->finishedCount() == c->nonobsoleteCount())
                emit contextDataChanged(index);
        } else if (c->finishedCount() == c->nonobsoleteCount()) {
            emit contextDataChanged(index);
        }
        emit messageDataChanged(index);
        setModified(index.model(), true);
    } else if (type == TranslatorMessage::Finished && !finished) {
        m->setType(TranslatorMessage::Unfinished);
        mm->incrementUnfinishedCount();
        if (mm->countUnfinished() == 1) {
            decrementFinishedCount();
            mc->decrementFinishedCount();
            emit multiContextDataChanged(index);
        }
        c->decrementFinishedCount();
        if (m->danger()) {
            c->decrementFinishedDangerCount();
            c->incrementUnfinishedDangerCount();
            if (c->unfinishedDangerCount() == 1
                || c->finishedCount() + 1 == c->nonobsoleteCount())
                emit contextDataChanged(index);
        } else if (c->finishedCount() + 1 == c->nonobsoleteCount()) {
            emit contextDataChanged(index);
        }
        emit messageDataChanged(index);
        setModified(index.model(), true);
    }
}

void MultiDataModel::setDanger(const MultiDataIndex &index, bool danger)
{
    ContextItem *c = contextItem(index);
    MessageItem *m = messageItem(index);
    if (!m->danger() && danger) {
        if (m->isFinished()) {
            c->incrementFinishedDangerCount();
            if (c->finishedDangerCount() == 1)
                emit contextDataChanged(index);
        } else {
            c->incrementUnfinishedDangerCount();
            if (c->unfinishedDangerCount() == 1)
                emit contextDataChanged(index);
        }
        emit messageDataChanged(index);
        m->setDanger(danger);
    } else if (m->danger() && !danger) {
        if (m->isFinished()) {
            c->decrementFinishedDangerCount();
            if (!c->finishedDangerCount())
                emit contextDataChanged(index);
        } else {
            c->decrementUnfinishedDangerCount();
            if (!c->unfinishedDangerCount())
                emit contextDataChanged(index);
        }
        emit messageDataChanged(index);
        m->setDanger(danger);
    }
}

void MultiDataModel::updateCountsOnAdd(int model, bool writable)
{
    for (int i = 0; i < m_multiContextList.size(); ++i) {
        MultiContextItem &mc = m_multiContextList[i];
        for (int j = 0; j < mc.messageCount(); ++j)
            if (MessageItem *m = mc.messageItem(model, j)) {
                MultiMessageItem *mm = mc.multiMessageItem(j);
                mm->incrementNonnullCount();
                if (!m->isObsolete()) {
                    if (writable) {
                        if (!mm->countEditable()) {
                            mc.incrementEditableCount();
                            incrementEditableCount();
                            if (m->isFinished()) {
                                mc.incrementFinishedCount();
                                incrementFinishedCount();
                            } else {
                                mm->incrementUnfinishedCount();
                            }
                        } else if (!m->isFinished()) {
                            if (!mm->isUnfinished()) {
                                mc.decrementFinishedCount();
                                decrementFinishedCount();
                            }
                            mm->incrementUnfinishedCount();
                        }
                        mm->incrementEditableCount();
                    }
                    mc.incrementNonobsoleteCount();
                    mm->incrementNonobsoleteCount();
                }
            }
    }
}

void MultiDataModel::updateCountsOnRemove(int model, bool writable)
{
    for (int i = 0; i < m_multiContextList.size(); ++i) {
        MultiContextItem &mc = m_multiContextList[i];
        for (int j = 0; j < mc.messageCount(); ++j)
            if (MessageItem *m = mc.messageItem(model, j)) {
                MultiMessageItem *mm = mc.multiMessageItem(j);
                mm->decrementNonnullCount();
                if (!m->isObsolete()) {
                    mm->decrementNonobsoleteCount();
                    mc.decrementNonobsoleteCount();
                    if (writable) {
                        mm->decrementEditableCount();
                        if (!mm->countEditable()) {
                            mc.decrementEditableCount();
                            decrementEditableCount();
                            if (m->isFinished()) {
                                mc.decrementFinishedCount();
                                decrementFinishedCount();
                            } else {
                                mm->decrementUnfinishedCount();
                            }
                        } else if (!m->isFinished()) {
                            mm->decrementUnfinishedCount();
                            if (!mm->isUnfinished()) {
                                mc.incrementFinishedCount();
                                incrementFinishedCount();
                            }
                        }
                    }
                }
            }
    }
}

/******************************************************************************
 *
 * MultiDataModelIterator 
 *
 *****************************************************************************/

MultiDataModelIterator::MultiDataModelIterator(MultiDataModel *dataModel, int model, int context, int message)
  : MultiDataIndex(model, context, message), m_dataModel(dataModel)
{
}

void MultiDataModelIterator::operator++()
{
    Q_ASSERT(isValid());
    ++m_message;
    if (m_message >= m_dataModel->m_multiContextList.at(m_context).messageCount()) {
        ++m_context;
        m_message = 0;
    }
}

bool MultiDataModelIterator::isValid() const
{
    return m_context < m_dataModel->m_multiContextList.count();
}

MessageItem *MultiDataModelIterator::current() const
{
    return m_dataModel->messageItem(*this);
}


/******************************************************************************
 *
 * MessageModel
 *
 *****************************************************************************/

MessageModel::MessageModel(QObject *parent, MultiDataModel *data)
  : QAbstractItemModel(parent), m_data(data)
{
    data->m_msgModel = this;
    connect(m_data, SIGNAL(multiContextDataChanged(MultiDataIndex)),
                    SLOT(multiContextItemChanged(MultiDataIndex)));
    connect(m_data, SIGNAL(contextDataChanged(MultiDataIndex)),
                    SLOT(contextItemChanged(MultiDataIndex)));
    connect(m_data, SIGNAL(messageDataChanged(MultiDataIndex)),
                    SLOT(messageItemChanged(MultiDataIndex)));
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, 0);
    if (!parent.internalId())
        return createIndex(row, column, parent.row() + 1);
    return QModelIndex();
}

QModelIndex MessageModel::parent(const QModelIndex& index) const
{
    if (index.internalId())
        return createIndex(index.internalId() - 1, 0, 0);
    return QModelIndex();
}

void MessageModel::multiContextItemChanged(const MultiDataIndex &index)
{
    QModelIndex idx = createIndex(index.context(), m_data->modelCount() + 2, 0);
    emit dataChanged(idx, idx);
}

void MessageModel::contextItemChanged(const MultiDataIndex &index)
{
    QModelIndex idx = createIndex(index.context(), index.model() + 1, 0);
    emit dataChanged(idx, idx);
}

void MessageModel::messageItemChanged(const MultiDataIndex &index)
{
    QModelIndex idx = createIndex(index.message(), index.model() + 1, index.context() + 1);
    emit dataChanged(idx, idx);
}

QModelIndex MessageModel::modelIndex(const MultiDataIndex &index)
{
    if (index.message() < 0) // Should be unused case
        return createIndex(index.context(), index.model() + 1, 0);
    return createIndex(index.message(), index.model() + 1, index.context() + 1);
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_data->contextCount(); // contexts
    if (!parent.internalId()) // messages
        return m_data->multiContextItem(parent.row())->messageCount();
    return 0;
}

int MessageModel::columnCount(const QModelIndex &) const
{
    return m_data->modelCount() + 3;
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    static QVariant pxOn  = 
        qVariantFromValue(QPixmap(QLatin1String(":/images/s_check_on.png")));
    static QVariant pxOff = 
        qVariantFromValue(QPixmap(QLatin1String(":/images/s_check_off.png")));
    static QVariant pxObsolete =    
        qVariantFromValue(QPixmap(QLatin1String(":/images/s_check_obsolete.png")));
    static QVariant pxDanger =
        qVariantFromValue(QPixmap(QLatin1String(":/images/s_check_danger.png")));
    static QVariant pxWarning =
        qVariantFromValue(QPixmap(QLatin1String(":/images/s_check_warning.png")));
    static QVariant pxEmpty = 
        qVariantFromValue(QPixmap(QLatin1String(":/images/s_check_empty.png")));

    int row = index.row();
    int column = index.column() - 1;
    if (column < 0)
        return QVariant();

    int numLangs = m_data->modelCount();

    if (role == Qt::ToolTipRole && column < numLangs) {
        return tr("Completion status for %1").arg(m_data->model(column)->localizedLanguage());
    } else if (index.internalId()) {
        // this is a message
        int crow = index.internalId() - 1;
        MultiContextItem *mci = m_data->multiContextItem(crow);
        if (row >= mci->messageCount() || !index.isValid())
            return QVariant();

        if (role == Qt::DisplayRole || (role == Qt::ToolTipRole && column == numLangs)) {
            switch (column - numLangs) {
            case 0: // Source text
                {
                    MultiMessageItem *msgItem = mci->multiMessageItem(row);
                    if (msgItem->text().isEmpty()) {
                        if (mci->context().isEmpty())
                            return tr("<file header>");
                        else
                            return tr("<context comment>");
                    }
                    return msgItem->text().simplified();
                }
            default: // Status or dummy column => no text
                return QVariant();
            }
        }
        else if (role == Qt::DecorationRole && column < numLangs) {
            if (MessageItem *msgItem = mci->messageItem(column, row)) {
                switch (msgItem->message().type()) {
                case TranslatorMessage::Unfinished:
                    if (msgItem->translation().isEmpty())
                        return pxEmpty;
                    if (msgItem->danger())
                        return pxDanger;
                    return pxOff;
                case TranslatorMessage::Finished:
                    if (msgItem->danger())
                        return pxWarning;
                    return pxOn;
                default:
                    return pxObsolete;
                }
            }
            return QVariant();
        }
        else if (role == SortRole) {
            switch (column - numLangs) {
            case 0: // Source text
                return mci->multiMessageItem(row)->text().simplified().remove(QLatin1Char('&'));
            case 1: // Dummy column
                return QVariant();
            default:
                if (MessageItem *msgItem = mci->messageItem(column, row)) {
                    int rslt = !msgItem->translation().isEmpty();
                    if (!msgItem->danger())
                        rslt |= 2;
                    if (msgItem->isObsolete())
                        rslt |= 8;
                    else if (msgItem->isFinished())
                        rslt |= 4;
                    return rslt;
                }
                return INT_MAX;
            }
        }
        else if (role == Qt::ForegroundRole && column > 0
                 && mci->multiMessageItem(row)->isObsolete()) {
            return QBrush(Qt::darkGray);
        }
        else if (role == Qt::ForegroundRole && column == numLangs
                 && mci->multiMessageItem(row)->text().isEmpty()) {
            return QBrush(QColor(0, 0xa0, 0xa0));
        }
        else if (role == Qt::BackgroundRole) {
            if (column < numLangs && numLangs != 1)
                return m_data->brushForModel(column);
        }
    } else {
        // this is a context
        if (row >= m_data->contextCount() || !index.isValid())
            return QVariant();

        MultiContextItem *mci = m_data->multiContextItem(row);

        if (role == Qt::DisplayRole || (role == Qt::ToolTipRole && column == numLangs)) {
            switch (column - numLangs) {
            case 0: // Context
                {
                    if (mci->context().isEmpty())
                        return tr("<unnamed context>");
                    return mci->context().simplified();
                }
            case 1:
                {
                    QString s;
                    s.sprintf("%d/%d", mci->getNumFinished(), mci->getNumEditable());
                    return s;
                }
            default:
                return QVariant(); // Status => no text
            }
        }
        else if (role == Qt::DecorationRole && column < numLangs) {
            if (ContextItem *contextItem = mci->contextItem(column)) {
                if (contextItem->isObsolete())
                    return pxObsolete;
                if (contextItem->isFinished())
                    return contextItem->finishedDangerCount() > 0 ? pxWarning : pxOn;
                return contextItem->unfinishedDangerCount() > 0 ? pxDanger : pxOff;
            }
            return QVariant();
        }
        else if (role == SortRole) {
            switch (column - numLangs) {
            case 0: // Context (same as display role)
                return mci->context().simplified();
            case 1: // Items
                return mci->getNumEditable();
            default: // Percent
                if (ContextItem *contextItem = mci->contextItem(column)) {
                    int totalItems = contextItem->nonobsoleteCount();
                    int percent = totalItems ? (100 * contextItem->finishedCount()) / totalItems : 100;
                    int rslt = percent * (((1 << 28) - 1) / 100) + totalItems;
                    if (contextItem->isObsolete()) {
                        rslt |= (1 << 30);
                    } else if (contextItem->isFinished()) {
                        rslt |= (1 << 29);
                        if (!contextItem->finishedDangerCount())
                            rslt |= (1 << 28);
                    } else {
                        if (!contextItem->unfinishedDangerCount())
                            rslt |= (1 << 28);
                    }
                    return rslt;
                }
                return INT_MAX;
            }
        }
        else if (role == Qt::ForegroundRole && column >= numLangs
                 && m_data->multiContextItem(row)->isObsolete()) {
            return QBrush(Qt::darkGray);
        }
        else if (role == Qt::ForegroundRole && column == numLangs
                 && m_data->multiContextItem(row)->context().isEmpty()) {
            return QBrush(QColor(0, 0xa0, 0xa0));
        }
        else if (role == Qt::BackgroundRole) {
            if (column < numLangs && numLangs != 1) {
                QBrush brush = m_data->brushForModel(column);
                if (row & 1) {
                    brush.setColor(brush.color().darker(108));
                }
                return brush;
            }
        }
    }
    return QVariant();
}

MultiDataIndex MessageModel::dataIndex(const QModelIndex &index, int model) const
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.internalId());
    return MultiDataIndex(model, index.internalId() - 1, index.row());
}

QT_END_NAMESPACE
