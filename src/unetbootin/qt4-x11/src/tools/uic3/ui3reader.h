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

#ifndef UI3READER_H
#define UI3READER_H

#include <QDomDocument>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QTextStream>
#include <QVariant>
#include <QByteArray>
#include <QPair>

QT_BEGIN_NAMESPACE

class DomUI;
class DomFont;
class DomWidget;
class DomProperty;
class DomLayout;
class DomLayoutItem;
class DomActionGroup;
class Porting;
struct Color;

typedef QList<QPair<int, Color> > ColorGroup;

class Ui3Reader
{
public:
    Ui3Reader(QTextStream &stream);
    ~Ui3Reader();

    void computeDeps(const QDomElement &e, QStringList &globalIncludes, QStringList &localIncludes, bool impl = false);
    void generateUi4(const QString &fn, const QString &outputFn, QDomDocument doc, bool implicitIncludes);

    void generate(const QString &fn, const QString &outputFn,
         QDomDocument doc, bool decl, bool subcl, const QString &trm,
         const QString& subclname, bool omitForwardDecls, bool implicitIncludes, const QString &convertedUiFile);

    void embed(const char *project, const QStringList &images);

    void setTrMacro(const QString &trmacro);
    void setForwardDeclarationsEnabled(bool b);
    void setOutputFileName(const QString &fileName);

    void createFormDecl(const QDomElement &e, bool implicitIncludes);
    void createFormImpl(const QDomElement &e);

    void createWrapperDecl(const QDomElement &e, const QString &convertedUiFile);

    void createSubDecl(const QDomElement &e, const QString& subclname);
    void createSubImpl(const QDomElement &e, const QString& subclname);

    void createColorGroupImpl(const QString& cg, const QDomElement& e);
    ColorGroup loadColorGroup(const QDomElement &e);

    QDomElement getObjectProperty(const QDomElement& e, const QString& name);
    QString getPixmapLoaderFunction(const QDomElement& e);
    QString getFormClassName(const QDomElement& e);
    QString getClassName(const QDomElement& e);
    QString getObjectName(const QDomElement& e);
    QString getLayoutName(const QDomElement& e);

    QString registerObject(const QString& name);
    QString registeredName(const QString& name);
    bool isObjectRegistered(const QString& name);
    QStringList unique(const QStringList&);

    QString trcall(const QString& sourceText, const QString& comment = QString());

    QDomElement parse(const QDomDocument &doc);

    void setExtractImages(bool extract, const QString &qrcOutputFile);

private:
    void init();

    void createWrapperDeclContents(const QDomElement &e);

    void errorInvalidProperty(const QString &propertyName, const QString &widgetName, const QString &widgetClass,
                              int line, int col);
    void errorInvalidSignal(const QString &signal, const QString &widgetName, const QString &widgetClass,
                            int line, int col);
    void errorInvalidSlot(const QString &slot, const QString &widgetName, const QString &widgetClass,
                          int line, int col);

    DomUI *generateUi4(const QDomElement &e, bool implicitIncludes);
    DomWidget *createWidget(const QDomElement &w, const QString &widgetClass = QString());
    void createProperties(const QDomElement &e, QList<DomProperty*> *properties, const QString &className);
    void createAttributes(const QDomElement &e, QList<DomProperty*> *properties, const QString &className);
    DomLayout *createLayout(const QDomElement &e);
    DomLayoutItem *createLayoutItem(const QDomElement &e);
    DomProperty *readProperty(const QDomElement &e);
    void fixActionGroup(DomActionGroup *g);
    QString fixActionProperties(QList<DomProperty*> &properties, bool isActionGroup = false);

    QString fixHeaderName(const QString &headerName) const;
    QString fixClassName(const QString &className) const;
    QString fixDeclaration(const QString &declaration) const;
    QString fixType(const QString &type) const;
    QString fixMethod(const QString &method) const;

    QDomElement findDerivedFontProperties(const QDomElement &n) const;

    void fixLayoutMargin(DomLayout *ui_layout);

    QTextStream &out;
    QTextOStream trout;
    QString languageChangeBody;
    QString outputFileName;
    QStringList objectNames;
    QMap<QString,QString> objectMapper;
    QString indent;
    QStringList tags;
    QStringList layouts;
    QString formName;
    QString lastItem;
    QString trmacro;
    bool nofwd;

    struct Buddy
    {
        Buddy(const QString& k, const QString& b)
            : key(k), buddy(b) {}
        Buddy(){} // for valuelist
        QString key;
        QString buddy;
        bool operator==(const Buddy& other) const
            { return (key == other.key); }
    };
    struct CustomInclude
    {
        QString header;
        QString location;
        Q_DUMMY_COMPARISON_OPERATOR(CustomInclude)
    };
    QList<Buddy> buddies;

    QStringList layoutObjects;
    bool isLayout(const QString& name) const;

    uint item_used : 1;
    uint cg_used : 1;
    uint pal_used : 1;
    uint stdsetdef : 1;
    uint externPixmaps : 1;

    QString uiFileVersion;
    QString nameOfClass;
    QStringList namespaces;
    QString bareNameOfClass;
    QString pixmapLoaderFunction;

    void registerDatabases(const QDomElement& e);
    bool isWidgetInTable(const QDomElement& e, const QString& connection, const QString& table);
    bool isFrameworkCodeGenerated(const QDomElement& e);
    QString getDatabaseInfo(const QDomElement& e, const QString& tag);
    void createFormImpl(const QDomElement& e, const QString& form, const QString& connection, const QString& table);
    void writeFunctionsDecl(const QStringList &fuLst, const QStringList &typLst, const QStringList &specLst);
    void writeFunctionsSubDecl(const QStringList &fuLst, const QStringList &typLst, const QStringList &specLst);
    void writeFunctionsSubImpl(const QStringList &fuLst, const QStringList &typLst, const QStringList &specLst,
                                const QString &subClass, const QString &descr);
    QStringList dbConnections;
    QMap<QString, QStringList> dbCursors;
    QMap<QString, QStringList> dbForms;

    static bool isMainWindow;
    static QString mkBool(bool b);
    static QString mkBool(const QString& s);
    bool toBool(const QString& s);
    static QString fixString(const QString &str, bool encode = false);
    static bool onlyAscii;
    static QString mkStdSet(const QString& prop);
    static QString getComment(const QDomNode& n);
    QVariant defSpacing, defMargin;
    QString fileName;
    bool writeFunctImpl;

    QDomElement root;
    QDomElement widget;

    QMap<QString, bool> candidateCustomWidgets;
    Porting *m_porting;

    bool m_extractImages;
    QString m_qrcOutputFile;
    QMap<QString, QString> m_imageMap;
};

QT_END_NAMESPACE

#endif // UI3READER_H
