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

#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "metatranslator.h"
#include <QAbstractItemModel>
#include <QList>
#include <QHash>
#include <QtCore/QLocale>
#include <QtXml/QXmlDefaultHandler>

QT_BEGIN_NAMESPACE

class ContextItem;
class QIODevice;

class MessageItem
{
public:
    MessageItem(const TranslatorMessage &message,
                const QString &text, const QString &comment,
                const QString &fileName, const int lineNumber, ContextItem *ctxtI);
    inline virtual ~MessageItem() {}

    inline virtual bool danger() const {return m_danger;}

    void setTranslation(const QString &translation);
    void setFinished(bool finished);
    void setDanger(bool danger);

    inline void setContextItem(ContextItem *ctxtI) {cntxtItem = ctxtI;}
    inline ContextItem *contextItem() const {return cntxtItem;}

    inline QString context() const {return QLatin1String(m.context());}
    inline QString sourceText() const {return tx;}
    inline QString comment() const {return com;}
    inline QString fileName() const { return m_fileName; }
    inline int lineNumber() const { return m_lineNumber; }
    inline QString translation() const {return m.translation();}
    inline QStringList translations() const { return m.translations(); }
    inline void setTranslations(const QStringList &translations) { m.setTranslations(translations); }

    inline bool finished() const {return m.type() == TranslatorMessage::Finished;}
    inline bool obsolete() const { return m.type() == TranslatorMessage::Obsolete; }
    inline TranslatorMessage message() const {return m;}
    bool compare(const QString &findText, bool matchSubstring, Qt::CaseSensitivity cs)
    {
        bool found = false;
        QString searchText = tx;
        if (matchSubstring) {
            if (searchText.indexOf(findText,0, cs) >= 0) {
                found = true;
            }
        } else {
            if (cs == Qt::CaseInsensitive) {
                if (findText.toLower() == searchText.toLower()) {
                    found = true;
                }
            } else {
                if ( findText == searchText ) {
                    found = true;
                }
            }
        }
        return found;
    }

private:
    TranslatorMessage m;
    QString tx;
    QString com;
    QString m_fileName;
    int m_lineNumber;
    bool m_danger;
    ContextItem *cntxtItem;
};


class MessageModel;

typedef QList<TranslatorMessage> TML;

class ContextItem
{
public:
    ContextItem(QString c, MessageModel *model);
    ~ContextItem();

    inline bool danger() const { return m_dangerCount > 0;}

    void appendToComment(const QString& x);
    void incrementFinishedCount();
    void decrementFinishedCount();
    inline void incrementDangerCount() { ++m_dangerCount; }
    inline void decrementDangerCount() { --m_dangerCount; }
    inline void incrementObsoleteCount() { ++m_obsoleteCount; }

    inline int finishedCount() const { return m_finishedCount; }
    inline int unFinishedCount() const {
        return msgItemList.count() - m_finishedCount - m_obsoleteCount;
    }
    inline int obsoleteCount() const { return m_obsoleteCount; }

    inline QString context() const { return ctxt; }
    inline QString comment() const { return com; }
    inline QString fullContext() const { return com.trimmed(); }

    inline bool isObsolete() const { return m_obsoleteCount == msgItemList.count(); }
    inline bool isFinished() const { return unFinishedCount() == 0; }

    MessageItem *messageItem(int i);
    inline const QList<MessageItem *> messageItemList() const { return msgItemList; }
    inline void appendMessageItem(MessageItem *msgItem) { msgItemList.append(msgItem); }
    inline int messageItemsInList() const { return msgItemList.count(); }

    MessageModel *model() { return m_model; }

private:
    QString com;
    QString ctxt;
    int m_finishedCount;
    int m_dangerCount;
    int m_obsoleteCount;
    QList<MessageItem *> msgItemList;
    MessageModel *m_model;
};

class ContextList : public QObject, public QList<ContextItem *>
{
    Q_OBJECT

public:
    ContextList();
    MessageItem *messageItem(int context, int message) const;
    ContextItem *contextItem(int context) const;

    bool isModified() const { return m_modified; }
    void setModified(bool isModified);

signals:
    void modifiedChanged(bool);

private:
    bool m_modified;
};


class MessageModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    MessageModel(QObject *parent = 0);

    class iterator
    {
    public:
        iterator(MessageModel *model, int contextNo = 0, int itemNo = -1);
        iterator(ContextList *contextlist, int contextNo = 0, int itemNo = -1);
        iterator(const iterator& other);
        MessageItem *current() const;
        MessageItem *operator*() const;

        iterator &operator=(const iterator& other);
        bool operator==(const iterator& other) const
        {
            return (other.m_contextList == m_contextList && other.m_contextNo == m_contextNo && other.m_itemNo == m_itemNo);
        }

        bool operator!=(const iterator& other) const
        {
            return !(*this==other);
        }
        iterator &operator++();
        void reset();
        int contextNo() const { return m_contextNo; }
        int messageNo() const { return m_itemNo; }
    private:
        iterator() {}
        ContextList *m_contextList;
        int m_contextNo;
        int m_itemNo;
    };

    iterator begin()
    {
        return Iterator(0, 0);
    }

    iterator end()
    {
        // both contextNo and itemNo points to the item behind the last item for end()
        // remember to make sure that begin() == end() for an empty model
        return Iterator(cntxtList.count(), cntxtList.count() ? cntxtList.last()->messageItemsInList() : 0);
    }

    iterator Iterator(int contextNo = 0, int itemNo = 0)
    {
        return iterator(&cntxtList, contextNo, itemNo);
    }

    enum { SortRole = Qt::UserRole };

    enum { SourceText = 0x1, Translations = 0x2, Comments = 0x4 };

    // model index functions
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    // Specializations
    inline const QList<ContextItem *> contextList() const {return cntxtList;}
    inline void appendContextItem(ContextItem *cntxtItem) {cntxtList.append(cntxtItem);}
    inline int contextsInList() const {return cntxtList.count();}
    bool findMessage(int *contextNo, int *itemNo, const QString &findItem, int where,
        bool matchSubstring, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    MessageItem *findMessage(const char *context, const char *sourcetext, const char *comment = 0) const;

    ContextItem *createContextItem(const QString &c)
    {
        return new ContextItem(c, this);
    }
    bool finished(const QModelIndex &index);
    void updateItem(QModelIndex indx);
    void updateAll();
    void clearContextList();

    ContextItem *contextItem(const QModelIndex &indx) const;
    MessageItem *messageItem(const QModelIndex &indx) const;

    ContextItem *contextItem(int context) const;
    MessageItem *messageItem(int context, int message) const;

    QModelIndex modelIndex(int context, int message, int column = 0);
    void setTranslation(const iterator &it, const QString &translation);

    int getMessageCount() const { return m_numMessages; }
    bool isEmpty() const { return m_numMessages == 0; }
    bool isModified() const { return cntxtList.isModified(); }
    void setModified(bool dirty) { cntxtList.setModified(dirty); }

    bool load(const QString &fileName);
    bool save(const QString &fileName);
    bool release(const QString& fileName,
                    bool verbose = false, bool ignoreUnfinished = false,
                    Translator::SaveMode mode = Translator::Stripped );
    bool release(QIODevice *iod,
                    bool verbose = false, bool ignoreUnfinished = false,
                    Translator::SaveMode mode = Translator::Stripped );
    QString srcFileName() { return m_srcFileName; }

    QTranslator *translator();

    QLocale::Language language() const;
    void setLanguage(QLocale::Language lang);
    QLocale::Country country() const;
    void setCountry(QLocale::Country country);

    QStringList normalizedTranslations(const MessageItem &m) const;
    void doCharCounting(const QString& text, int& trW, int& trC, int& trCS);
    void updateStatistics();

    int getNumFinished() const { return m_numFinished; }
    int getNumNonobsolete() const { return m_numNonobsolete; }
    int getSrcWords() const { return m_srcWords; }
    int getSrcChars() const { return m_srcChars; }
    int getSrcCharsSpc() const { return m_srcCharsSpc; }

    void incrementFinishedCount() { ++m_numFinished; }
    void decrementFinishedCount()
    {
        if (m_numFinished > 0)
            --m_numFinished;
    }

    static QPixmap *pxOn;
    static QPixmap *pxOff;
    static QPixmap *pxObsolete;
    static QPixmap *pxDanger;
    static QPixmap *pxWarning;
    static QPixmap *pxEmpty;

signals:
    void statsChanged(int words, int characters, int cs, int words2, int characters2, int cs2);
    void progressChanged(int finishedCount, int oldFinishedCount);
    void languageChanged(QLocale::Language lang);
    void modifiedChanged(bool);

private:
    QTranslator *m_translator;
    ContextList cntxtList;

    int m_numFinished;
    int m_numNonobsolete;
    int m_numMessages;

    QString m_srcFileName;

    // For statistics
    int m_srcWords;
    int m_srcChars;
    int m_srcCharsSpc;

    QLocale::Language m_language;
    QLocale::Country m_country;
    QByteArray m_codecForTr;

    class SimpleXmlErrorHandler : public QXmlDefaultHandler
    {
    public:
        virtual bool fatalError(const QXmlParseException& exception);
    };

    SimpleXmlErrorHandler xmlErrorHandler;

    friend class iterator;
};

QT_END_NAMESPACE

#endif //MESSAGEMODEL_H
