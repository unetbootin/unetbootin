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

/*
  doc.h
*/

#ifndef DOC_H
#define DOC_H

#include <QSet>
#include <QString>

#include "location.h"

QT_BEGIN_NAMESPACE

class Atom;
class CodeMarker;
class Config;
class DocPrivate;
class Quoter;
class Text;
class FakeNode;

typedef QMap<QString, QStringList> QCommandMap;
typedef QMap<QString, QString> QStringMap;
typedef QStringMap::const_iterator QStringMapEntry; 
typedef QMultiMap<QString, QString> QStringMultiMap;

class Doc
{
 public:
    // the order is important
    enum SectioningUnit { 
        Book = -2, 
        Part, 
        Chapter, 
        Section1, 
        Section2, 
        Section3, 
        Section4 
    };

    Doc() : priv(0) {}
    Doc(const Location &start_loc,
        const Location &end_loc, 
        const QString &source, 
        const QSet<QString> &metaCommandSet);
    Doc(const Doc &doc);
    ~Doc();

    Doc& operator=( const Doc& doc );

    void renameParameters(const QStringList &oldNames, 
                          const QStringList &newNames);
    void simplifyEnumDoc();
    void setBody(const Text &body);

    const Location &location() const;
    bool isEmpty() const;
    const QString& source() const;
    const Text& body() const;
    Text briefText() const;
    Text trimmedBriefText(const QString &className) const;
    Text legaleseText() const;
    const QString& baseName() const;
    SectioningUnit granularity() const;
    const QSet<QString> &parameterNames() const;
    const QStringList &enumItemNames() const;
    const QStringList &omitEnumItemNames() const;
    const QSet<QString> &metaCommandsUsed() const;
    QStringList metaCommandArgs( const QString& metaCommand ) const;
    const QList<Text> &alsoList() const;
    bool hasTableOfContents() const;
    bool hasKeywords() const;
    bool hasTargets() const;
    const QList<Atom *> &tableOfContents() const;
    const QList<int> &tableOfContentsLevels() const;
    const QList<Atom *> &keywords() const;
    const QList<Atom *> &targets() const;
    const QStringMap &metaTagMap() const;

    static void initialize( const Config &config );
    static void terminate();
    static QString alias( const QString &english );
    static void trimCStyleComment( Location& location, QString& str );
    static CodeMarker *quoteFromFile(const Location &location, 
                                     Quoter &quoter,
                                     const QString &fileName);
    static QString canonicalTitle(const QString &title);

 private:
    void detach();
    DocPrivate *priv;
};

class DoxWriter
{
  public:
    DoxWriter(const QString& source, DocPrivate* docPrivate)
        : commentType(OtherComment),
          lineLength(0),
          lineCount(0),
          priv(docPrivate),
          oldText(source) {}
    ~DoxWriter() {}

    void pass1();
    void pass2();

    static void setDoxPass(int pass);
    static bool isDoxPass(int pass);
    static bool isDoxPass();
    static void insertTitle(FakeNode* node, const QString& title);
    static void writeTitles();
    static void readTitles();
    static void writeMembers();
    static void readMembers();
    static void writeAnchors();
    static void readAnchors();

  private:
    void indentLine();
    void newLine();
    void concatenate(QString text);
    void wrap(QString text);
    bool conversionRequired() const;
    void convertMetaCommands();
    void convertText();
    const Atom* link(const Atom* atom);
    void formattingLeft(const Atom* atom, const Atom* next);
    void formattingRight(const Atom* atom, const Atom* prev);
    void tt(const Atom* atom);
    void formatIf(const Atom* atom);
    void formatEndif();
    void formatElse();
    const Atom* sectionHeading(const Atom* atom);
    void unhandled(const Atom* atom);
    void code(const Atom* atom);
    const Atom* codeQuoteCommand(const Atom* atom);
    void writeCode(QString text);
    void writeCommand(QCommandMap::const_iterator cmd);

    static void insertAnchor(const QString& text);
    static bool isAnchor(const QString& text);

    static const QString* getPageFile(const QString& title);
    static const QString* getFile(const QString& title);
    static const QString* getExamplePath(const QString& title);
    static const QString* getHeaderFile(const QString& title);
    static const QString* getGroup(const QString& title);
    static const QString* getModule(const QString& title);
    static const QString* getExternalPage(const QString& title);
    static const QString* getPageTitle(const QString& text);
    static const QString* getFileTitle(const QString& text);
    static const QString* getExampleTitle(const QString& text);
    static const QString* getHeaderFileTitle(const QString& text);
    static const QString* getGroupTitle(const QString& text);
    static const QString* getModuleTitle(const QString& text);
    static const QString* getExternalPageTitle(const QString& text);
    
    static bool isProperty(const QString& title, QStringList& classes);
    static bool isVariable(const QString& title, QStringList& classes);
    static bool isEnum(const QString& title, QStringList& classes);

 private:
    static void writeMap(const QStringMap& map, const QString& name);
    static void readMap(QStringMap& map, 
                        QStringMap& inverseMap, 
                        const QString& name);
    static void writeMultiMap(const QStringMultiMap& map, const QString& name);
    static void readMultiMap(QStringMultiMap& map, const QString& name);

 public: // VS 6, SunCC need this to be public
    enum StructType { BulletList, NumericList, ValueList, Table };
 private:
    struct StructDesc {
        StructType      structType;
        int             count;
        bool            nested;
        bool            inTableHeader;
        bool            inTableRow;
        bool            inTableItem;
        bool            odd;

        StructDesc()
            : structType(BulletList),
              count(0),
              nested(false),
              inTableHeader(false),
              inTableRow(false),
              inTableItem(false),
              odd(true) { }

        StructDesc(StructType t, bool n)
            : structType(t),
              count(0),
              nested(n),
              inTableHeader(false),
              inTableRow(false),
              inTableItem(false),
              odd(true) { }
    };

    typedef QStack<StructDesc> StructStack;

    enum CommentType {
        ClassComment,
        EnumComment,
        ExampleComment,
        FnComment,
        GroupComment,
        HeaderFileComment,
        MacroComment,
        ModuleComment,
        PageComment,
        PropertyComment,
        ServiceComment,
        TypedefComment,
        VariableComment,
        OtherComment
    };

  private:
    CommentType         commentType;
    int                 lineLength;
    int                 lineCount;
    DocPrivate*         priv;
    QString             oldText;
    QString             newText;
    StructStack         structs;

    QString             currentPage;
    QString             currentFn;
    QString             currentTitle;
    QString             currentEnum;
    QString             currentProperty;
    QString             currentVariable;
    QString             currentExample;
    QString             currentGroup;
    QString             currentModule;
    QString             currentMacro;
    QString             currentService;
    QString             currentTypedef;
    QString             currentHeaderFile;
    static QString      currentClass;

    static int          doxPass;
    static QSet<QString> anchors;
    static QStringMap   exampleTitles;
    static QStringMap   headerFileTitles;
    static QStringMap   fileTitles;
    static QStringMap   groupTitles;
    static QStringMap   moduleTitles;
    static QStringMap   pageTitles;
    static QStringMap   externalPageTitles;
    static QStringMap   exampleTitlesInverse;
    static QStringMap   headerFileTitlesInverse;
    static QStringMap   fileTitlesInverse;
    static QStringMap   groupTitlesInverse;
    static QStringMap   moduleTitlesInverse;
    static QStringMap   pageTitlesInverse;
    static QStringMap   externalPageTitlesInverse;

    static QStringMultiMap variables;
    static QStringMultiMap properties;
    static QStringMultiMap enums;
};

QT_END_NAMESPACE

#endif
