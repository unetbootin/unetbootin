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

#include "ui3reader.h"
#include "parser.h"
#include "domtool.h"
#include "globaldefs.h"

// uic4
#include "uic.h"
#include "ui4.h"
#include "driver.h"
#include "option.h"

#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QtDebug>

QT_BEGIN_NAMESPACE

QByteArray combinePath(const char *infile, const char *outfile)
{
    QFileInfo inFileInfo(QDir::current(), QFile::decodeName(infile));
    QFileInfo outFileInfo(QDir::current(), QFile::decodeName(outfile));
    int numCommonComponents = 0;

    QStringList inSplitted = inFileInfo.dir().canonicalPath().split(QLatin1Char('/'));
    QStringList outSplitted = outFileInfo.dir().canonicalPath().split(QLatin1Char('/'));

    while (!inSplitted.isEmpty() && !outSplitted.isEmpty() &&
            inSplitted.first() == outSplitted.first()) {
        inSplitted.erase(inSplitted.begin());
        outSplitted.erase(outSplitted.begin());
        numCommonComponents++;
    }

    if (numCommonComponents < 2) {
        /*
          The paths don't have the same drive, or they don't have the
          same root directory. Use an absolute path.
        */
        return QFile::encodeName(inFileInfo.absoluteFilePath());
    } else {
        /*
          The paths have something in common. Use a path relative to
          the output file.
        */
        while (!outSplitted.isEmpty()) {
            outSplitted.erase(outSplitted.begin());
            inSplitted.prepend(QLatin1String(".."));
        }
        inSplitted.append(inFileInfo.fileName());
        return QFile::encodeName(inSplitted.join(QLatin1String("/")));
    }
}

/*!
  Creates a declaration (header file) for the form given in \a e

  \sa createFormImpl()
*/
void Ui3Reader::createFormDecl(const QDomElement &e, bool implicitIncludes)
{
    QDomElement body = e;

    QDomElement n;
    QDomNodeList nl;
    int i;
    QString objClass = getClassName(e);
    if (objClass.isEmpty())
        return;
    QString objName = getObjectName(e);

    QStringList typeDefs;

    QMap<QString, CustomInclude> customWidgetIncludes;

    /*
      We are generating a few QImage members that are not strictly
      necessary in some cases. Ideally, we would use requiredImage,
      which is computed elsewhere, to keep the generated .h and .cpp
      files synchronized.
    */

    // at first the images
    QMap<QString, int> customWidgets;
    QStringList forwardDecl;
    QStringList forwardDecl2;
    for (n = e; !n.isNull(); n = n.nextSibling().toElement()) {
        if (n.tagName().toLower() == QLatin1String("customwidgets")) {
            QDomElement n2 = n.firstChild().toElement();
            while (!n2.isNull()) {
                if (n2.tagName().toLower() == QLatin1String("customwidget")) {
                    QDomElement n3 = n2.firstChild().toElement();
                    QString cl;
                    while (!n3.isNull()) {
                        QString tagName = n3.tagName().toLower();
                        if (tagName == QLatin1String("class")) {
                            cl = n3.firstChild().toText().data();
                            if (!nofwd)
                                forwardDecl << cl;
                            customWidgets.insert(cl, 0);
                        } else if (tagName == QLatin1String("header")) {
                            CustomInclude ci;
                            ci.header = n3.firstChild().toText().data();
                            ci.location = n3.attribute(QLatin1String("location"), QLatin1String("global"));
                            if (!ci.header.isEmpty())
                                forwardDecl.removeAll(cl);
                            customWidgetIncludes.insert(cl, ci);
                        }
                        n3 = n3.nextSibling().toElement();
                    }
                }
                n2 = n2.nextSibling().toElement();
            }
        }
    }

    // register the object and unify its name
    objName = registerObject(objName);
    QString protector = objName.toUpper() + QLatin1String("_H");
    protector.replace(QLatin1String("::"), QLatin1String("_"));
    out << "#ifndef " << protector << endl;
    out << "#define " << protector << endl;
    out << endl;

    out << "#include <qvariant.h>" << endl; // for broken HP-UX compilers

    QStringList globalIncludes, localIncludes;

    {
        QMap<QString, CustomInclude>::Iterator it = customWidgetIncludes.find(objClass);
        if (it != customWidgetIncludes.end()) {
            if ((*it).location == QLatin1String("global"))
                globalIncludes += (*it).header;
            else
                localIncludes += (*it).header;
        }
    }

    QStringList::ConstIterator it;

    globalIncludes = unique(globalIncludes);
    for (it = globalIncludes.constBegin(); it != globalIncludes.constEnd(); ++it) {
        if (!(*it).isEmpty()) {
            QString header = fixHeaderName(*it);
            out << "#include <" << header << ">" << endl;
        }
    }
    localIncludes = unique(localIncludes);
    for (it = localIncludes.constBegin(); it != localIncludes.constEnd(); ++it) {
        if (!(*it).isEmpty()) {
            QString header = fixHeaderName(*it);
            out << "#include \"" << header << "\"" << endl;
        }
    }
    out << endl;

    bool dbForm = false;
    registerDatabases(e);
    dbConnections = unique(dbConnections);
    if (dbForms[QLatin1String("(default)")].count())
        dbForm = true;
    bool subDbForms = false;
    for (it = dbConnections.constBegin(); it != dbConnections.constEnd(); ++it) {
        if (!(*it).isEmpty() && (*it) != QLatin1String("(default)")) {
            if (dbForms[(*it)].count()) {
                subDbForms = true;
                break;
            }
        }
    }

    // some typedefs, maybe
    typeDefs = unique(typeDefs);
    for (it = typeDefs.constBegin(); it != typeDefs.constEnd(); ++it) {
        if (!(*it).isEmpty())
            out << "typedef " << *it << ";" << endl;
    }

    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("forward"));
    for (i = 0; i < (int) nl.length(); i++)
        forwardDecl2 << fixDeclaration(nl.item(i).toElement().firstChild().toText().data());

    forwardDecl = unique(forwardDecl);
    for (it = forwardDecl.constBegin(); it != forwardDecl.constEnd(); ++it) {
        if (!(*it).isEmpty() && (*it) != objClass) {
            QString forwardName = *it;
            QStringList forwardNamespaces = forwardName.split(QLatin1String("::"));
            forwardName = forwardNamespaces.last();
            forwardNamespaces.removeAt(forwardNamespaces.size()-1);

            QStringList::ConstIterator ns = forwardNamespaces.constBegin();
            while (ns != forwardNamespaces.constEnd()) {
                out << "namespace " << *ns << " {" << endl;
                ++ns;
            }
            out << "class " << forwardName << ";" << endl;
            for (int i = 0; i < (int) forwardNamespaces.count(); i++)
                out << "}" << endl;
        }
    }

    for (it = forwardDecl2.constBegin(); it != forwardDecl2.constEnd(); ++it) {
        QString fd = *it;
        fd = fd.trimmed();
        if (!fd.endsWith(QLatin1String(";")))
            fd += QLatin1String(";");
        out << fd << endl;
    }

    out << endl;

    Driver d;
    d.option().headerProtection = false;
    d.option().copyrightHeader = false;
    d.option().extractImages = m_extractImages;
    d.option().qrcOutputFile = m_qrcOutputFile;
    d.option().implicitIncludes = implicitIncludes;
    if (trmacro.size())
        d.option().translateFunction = trmacro;
    DomUI *ui = generateUi4(e, implicitIncludes);
    d.uic(fileName, ui, &out);
    delete ui;

    createWrapperDeclContents(e);

    out << "#endif // " << protector << endl;
}

void Ui3Reader::createWrapperDecl(const QDomElement &e, const QString &convertedUiFile)
{
    QString objName = getObjectName(e);

    objName = registerObject(objName);
    QString protector = objName.toUpper() + QLatin1String("_H");
    protector.replace(QLatin1String("::"), QLatin1String("_"));
    out << "#ifndef " << protector << endl;
    out << "#define " << protector << endl;
    out << endl;
    out << "#include \"" << convertedUiFile << "\"" << endl;

    createWrapperDeclContents(e);
    out << endl;
    out << "#endif // " << protector << endl;
}

void Ui3Reader::createWrapperDeclContents(const QDomElement &e)
{
    QString objClass = getClassName(e);
    if (objClass.isEmpty())
        return;

    QDomNodeList nl;
    QString exportMacro;
    int i;
    QDomElement n;
    QStringList::ConstIterator it;
    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("exportmacro"));
    if (nl.length() == 1)
        exportMacro = nl.item(0).firstChild().toText().data();

    QStringList::ConstIterator ns = namespaces.constBegin();
    while (ns != namespaces.constEnd()) {
        out << "namespace " << *ns << " {" << endl;
        ++ns;
    }

    out << "class ";
    if (!exportMacro.isEmpty())
        out << exportMacro << " ";
    out << bareNameOfClass << " : public " << objClass << ", public Ui::" << bareNameOfClass << endl << "{" << endl;

    /* qmake ignore Q_OBJECT */
    out << "    Q_OBJECT" << endl;
    out << endl;
    out << "public:" << endl;

    // constructor
    if (objClass == QLatin1String("QDialog") || objClass == QLatin1String("QWizard")) {
        out << "    " << bareNameOfClass << "(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);" << endl;
    } else if (objClass == QLatin1String("QWidget")) {
        out << "    " << bareNameOfClass << "(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0);" << endl;
    } else if (objClass == QLatin1String("QMainWindow") || objClass == QLatin1String("Q3MainWindow")) {
        out << "    " << bareNameOfClass << "(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::WType_TopLevel);" << endl;
        isMainWindow = true;
    } else {
        out << "    " << bareNameOfClass << "(QWidget* parent = 0, const char* name = 0);" << endl;
    }

    // destructor
    out << "    ~" << bareNameOfClass << "();" << endl;
    out << endl;

    // database connections
    dbConnections = unique(dbConnections);
    bool hadOutput = false;
    for (it = dbConnections.constBegin(); it != dbConnections.constEnd(); ++it) {
        if (!(*it).isEmpty()) {
            // only need pointers to non-default connections
            if ((*it) != QLatin1String("(default)") && !(*it).isEmpty()) {
                out << indent << "QSqlDatabase* " << *it << "Connection;" << endl;
                hadOutput = true;
            }
        }
    }
    if (hadOutput)
        out << endl;

    QStringList publicSlots, protectedSlots, privateSlots;
    QStringList publicSlotTypes, protectedSlotTypes, privateSlotTypes;
    QStringList publicSlotSpecifier, protectedSlotSpecifier, privateSlotSpecifier;

    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("slot"));
    for (i = 0; i < (int) nl.length(); i++) {
        n = nl.item(i).toElement();
        if (n.parentNode().toElement().tagName() != QLatin1String("slots")
             && n.parentNode().toElement().tagName() != QLatin1String("connections"))
            continue;
        if (n.attribute(QLatin1String("language"), QLatin1String("C++")) != QLatin1String("C++"))
            continue;
        QString returnType = n.attribute(QLatin1String("returnType"), QLatin1String("void"));
        QString functionName = n.firstChild().toText().data().trimmed();
        if (functionName.endsWith(QLatin1String(";")))
            functionName = functionName.left(functionName.length() - 1);
        QString specifier = n.attribute(QLatin1String("specifier"));
        QString access = n.attribute(QLatin1String("access"));
        if (access == QLatin1String(QLatin1String("protected"))) {
            protectedSlots += functionName;
            protectedSlotTypes += returnType;
            protectedSlotSpecifier += specifier;
        } else if (access == QLatin1String("private")) {
            privateSlots += functionName;
            privateSlotTypes += returnType;
            privateSlotSpecifier += specifier;
        } else {
            publicSlots += functionName;
            publicSlotTypes += returnType;
            publicSlotSpecifier += specifier;
        }
    }

    QStringList publicFuncts, protectedFuncts, privateFuncts;
    QStringList publicFunctRetTyp, protectedFunctRetTyp, privateFunctRetTyp;
    QStringList publicFunctSpec, protectedFunctSpec, privateFunctSpec;

    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("function"));
    for (i = 0; i < (int) nl.length(); i++) {
        n = nl.item(i).toElement();
        if (n.parentNode().toElement().tagName() != QLatin1String("functions"))
            continue;
        if (n.attribute(QLatin1String("language"), QLatin1String("C++")) != QLatin1String("C++"))
            continue;
        QString returnType = n.attribute(QLatin1String("returnType"), QLatin1String("void"));
        QString functionName = n.firstChild().toText().data().trimmed();
        if (functionName.endsWith(QLatin1String(";")))
            functionName = functionName.left(functionName.length() - 1);
        QString specifier = n.attribute(QLatin1String("specifier"));
        QString access = n.attribute(QLatin1String("access"));
        if (access == QLatin1String("protected")) {
            protectedFuncts += functionName;
            protectedFunctRetTyp += returnType;
            protectedFunctSpec += specifier;
        } else if (access == QLatin1String("private")) {
            privateFuncts += functionName;
            privateFunctRetTyp += returnType;
            privateFunctSpec += specifier;
        } else {
            publicFuncts += functionName;
            publicFunctRetTyp += returnType;
            publicFunctSpec += specifier;
        }
    }

    QStringList publicVars, protectedVars, privateVars;
    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("variable"));
    for (i = 0; i < (int)nl.length(); i++) {
        n = nl.item(i).toElement();
        // Because of compatibility the next lines have to be commented out.
        // Someday it should be uncommented.
        //if (n.parentNode().toElement().tagName() != QLatin1String("variables"))
        //    continue;
        QString access = n.attribute(QLatin1String("access"), QLatin1String("protected"));
        QString var = fixDeclaration(n.firstChild().toText().data().trimmed());
        if (!var.endsWith(QLatin1String(";")))
            var += QLatin1String(";");
        if (access == QLatin1String("public"))
            publicVars += var;
        else if (access == QLatin1String("private"))
            privateVars += var;
        else
            protectedVars += var;
    }

    if (!publicVars.isEmpty()) {
        for (it = publicVars.constBegin(); it != publicVars.constEnd(); ++it)
            out << indent << *it << endl;
        out << endl;
    }
    if (!publicFuncts.isEmpty())
        writeFunctionsDecl(publicFuncts, publicFunctRetTyp, publicFunctSpec);

    if (!publicSlots.isEmpty()) {
        out << "public slots:" << endl;
        if (!publicSlots.isEmpty())
            writeFunctionsDecl(publicSlots, publicSlotTypes, publicSlotSpecifier);
    }

    // find signals
    QStringList extraSignals;
    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("signal"));
    for (i = 0; i < (int) nl.length(); i++) {
        n = nl.item(i).toElement();
        if (n.parentNode().toElement().tagName() != QLatin1String("signals")
             && n.parentNode().toElement().tagName() != QLatin1String("connections"))
            continue;
        if (n.attribute(QLatin1String("language"), QLatin1String("C++")) != QLatin1String("C++"))
            continue;
        QString sigName = n.firstChild().toText().data().trimmed();
        if (sigName.endsWith(QLatin1String(";")))
            sigName = sigName.left(sigName.length() - 1);
        extraSignals += fixDeclaration(sigName);
    }

    // create signals
    if (!extraSignals.isEmpty()) {
        out << "signals:" << endl;
        for (it = extraSignals.constBegin(); it != extraSignals.constEnd(); ++it)
            out << "    void " << (*it) << ";" << endl;
        out << endl;
    }

    if (!protectedVars.isEmpty()) {
        out << "protected:" << endl;
        for (it = protectedVars.constBegin(); it != protectedVars.constEnd(); ++it)
            out << indent << *it << endl;
        out << endl;
    }

    if (!protectedFuncts.isEmpty()) {
        if (protectedVars.isEmpty())
            out << "protected:" << endl;

        writeFunctionsDecl(protectedFuncts, protectedFunctRetTyp, protectedFunctSpec);
    }

    out << "protected slots:" << endl;
    out << "    virtual void languageChange();" << endl;

    if (!protectedSlots.isEmpty()) {
        out << endl;
        writeFunctionsDecl(protectedSlots, protectedSlotTypes, protectedSlotSpecifier);
    }
    out << endl;

    // create all private stuff
    if (!privateFuncts.isEmpty() || !privateVars.isEmpty()) {
        out << "private:" << endl;
        if (!privateVars.isEmpty()) {
            for (it = privateVars.constBegin(); it != privateVars.constEnd(); ++it)
                out << indent << *it << endl;
            out << endl;
        }
        if (!privateFuncts.isEmpty())
            writeFunctionsDecl(privateFuncts, privateFunctRetTyp, privateFunctSpec);
    }

    if (!privateSlots.isEmpty()) {
        out << "private slots:" << endl;
        writeFunctionsDecl(privateSlots, privateSlotTypes, privateSlotSpecifier);
    }

    out << "};" << endl;
    for (i = 0; i < (int) namespaces.count(); i++)
        out << "}" << endl;

    out << endl;
}

void Ui3Reader::writeFunctionsDecl(const QStringList &fuLst, const QStringList &typLst, const QStringList &specLst)
{
    QStringList::ConstIterator it, it2, it3;
    for (it = fuLst.begin(), it2 = typLst.begin(), it3 = specLst.begin();
          it != fuLst.end(); ++it, ++it2, ++it3) {
        QString signature = *it;
        QString specifier;
        QString pure;
        QString type = *it2;
        if (type.isEmpty())
            type = QLatin1String("void");
        if (*it3 == QLatin1String("static")) {
            specifier = QLatin1String("static ");
        } else {
            if (*it3 != QLatin1String("non virtual") && *it3 != QLatin1String("nonVirtual"))
                specifier = QLatin1String("virtual ");
            if (*it3 == QLatin1String("pure virtual") || *it3 == QLatin1String("pureVirtual"))
                pure = QLatin1String(" = 0");
        }
        type.replace(QLatin1String(">>"), QLatin1String("> >"));
        if (!signature.contains(QLatin1String("operator")))
            signature.replace(QLatin1String(">>"), QLatin1String("> >"));

        signature = fixDeclaration(signature);
        type = fixType(type);
        out << "    " << specifier << type << " " << signature << pure << ";" << endl;
    }
    out << endl;
}

/*!
  Creates an implementation (cpp-file) for the form given in \a e.

  \sa createFormDecl(), createObjectImpl()
 */
void Ui3Reader::createFormImpl(const QDomElement &e)
{
    QDomElement n;
    QDomNodeList nl;
    int i;
    QString objClass = getClassName(e);
    if (objClass.isEmpty())
        return;
    QString objName = getObjectName(e);

    // generate local and local includes required
    QStringList globalIncludes, localIncludes;
    QStringList::Iterator it;

    QMap<QString, CustomInclude> customWidgetIncludes;

    // find additional slots and functions
    QStringList extraFuncts;
    QStringList extraFunctTyp;
    QStringList extraFunctSpecifier;

    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("slot"));
    for (i = 0; i < (int) nl.length(); i++) {
        n = nl.item(i).toElement();
        if (n.parentNode().toElement().tagName() != QLatin1String("slots")
             && n.parentNode().toElement().tagName() != QLatin1String("connections"))
            continue;
        if (n.attribute(QLatin1String("language"), QLatin1String("C++")) != QLatin1String("C++"))
            continue;
        QString functionName = n.firstChild().toText().data().trimmed();
        if (functionName.endsWith(QLatin1String(";")))
            functionName = functionName.left(functionName.length() - 1);
        extraFuncts += functionName;
        extraFunctTyp += n.attribute(QLatin1String("returnType"), QLatin1String("void"));
        extraFunctSpecifier += n.attribute(QLatin1String("specifier"), QLatin1String("virtual"));
    }

    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("function"));
    for (i = 0; i < (int) nl.length(); i++) {
        n = nl.item(i).toElement();
        if (n.parentNode().toElement().tagName() != QLatin1String("functions"))
            continue;
        if (n.attribute(QLatin1String("language"), QLatin1String("C++")) != QLatin1String("C++"))
            continue;
        QString functionName = n.firstChild().toText().data().trimmed();
        if (functionName.endsWith(QLatin1String(";")))
            functionName = functionName.left(functionName.length() - 1);
        extraFuncts += functionName;
        extraFunctTyp += n.attribute(QLatin1String("returnType"), QLatin1String("void"));
        extraFunctSpecifier += n.attribute(QLatin1String("specifier"), QLatin1String("virtual"));
    }

    // additional includes (local or global) and forward declaractions
    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("include"));
    for (i = 0; i < (int) nl.length(); i++) {
        QDomElement n2 = nl.item(i).toElement();
        QString s = n2.firstChild().toText().data();
        if (n2.attribute(QLatin1String("location")) != QLatin1String("local")) {
            if (s.right(5) == QLatin1String(".ui.h") && !QFile::exists(s))
                continue;
            if (n2.attribute(QLatin1String("impldecl"), QLatin1String("in implementation")) != QLatin1String("in implementation"))
                continue;
            globalIncludes += s;
        }
    }

    registerDatabases(e);
    dbConnections = unique(dbConnections);
    bool dbForm = false;
    if (dbForms[QLatin1String("(default)")].count())
        dbForm = true;
    bool subDbForms = false;
    for (it = dbConnections.begin(); it != dbConnections.end(); ++it) {
        if (!(*it).isEmpty()  && (*it) != QLatin1String("(default)")) {
            if (dbForms[(*it)].count()) {
                subDbForms = true;
                break;
            }
        }
    }

    // do the local includes afterwards, since global includes have priority on clashes
    for (i = 0; i < (int) nl.length(); i++) {
        QDomElement n2 = nl.item(i).toElement();
        QString s = n2.firstChild().toText().data();
        if (n2.attribute(QLatin1String("location")) == QLatin1String("local") && !globalIncludes.contains(s)) {
            if (s.right(5) == QLatin1String(".ui.h") && !QFile::exists(s))
                continue;
            if (n2.attribute(QLatin1String("impldecl"), QLatin1String("in implementation")) != QLatin1String("in implementation"))
                continue;
            localIncludes += s;
        }
    }

    // additional custom widget headers
    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("header"));
    for (i = 0; i < (int) nl.length(); i++) {
        QDomElement n2 = nl.item(i).toElement();
        QString s = n2.firstChild().toText().data();
        if (n2.attribute(QLatin1String("location")) != QLatin1String("local"))
            globalIncludes += s;
        else
            localIncludes += s;
    }

    out << "#include <qvariant.h>" << endl; // first for gcc 2.7.2

    globalIncludes = unique(globalIncludes);
    for (it = globalIncludes.begin(); it != globalIncludes.end(); ++it) {
        if (!(*it).isEmpty())
            out << "#include <" << fixHeaderName(*it) << ">" << endl;
    }

    if (externPixmaps) {
        out << "#include <qimage.h>" << endl;
        out << "#include <qpixmap.h>" << endl << endl;
    }

    /*
      Put local includes after all global includes
    */
    localIncludes = unique(localIncludes);
    for (it = localIncludes.begin(); it != localIncludes.end(); ++it) {
        if (!(*it).isEmpty() && *it != QFileInfo(fileName + QLatin1String(".h")).fileName())
            out << "#include \"" << fixHeaderName(*it) << "\"" << endl;
    }

    QString uiDotH = fileName + QLatin1String(".h");
    if (QFile::exists(uiDotH)) {
        if (!outputFileName.isEmpty())
            uiDotH = QString::fromUtf8(combinePath(uiDotH.ascii(), outputFileName.ascii()));
        out << "#include \"" << uiDotH << "\"" << endl;
        writeFunctImpl = false;
    }

    // register the object and unify its name
    objName = registerObject(objName);

    if (externPixmaps) {
        pixmapLoaderFunction = QLatin1String("QPixmap::fromMimeSource");
    }

    // constructor
    if (objClass == QLatin1String("QDialog") || objClass == QLatin1String("QWizard")) {
        out << "/*" << endl;
        out << " *  Constructs a " << nameOfClass << " as a child of 'parent', with the" << endl;
        out << " *  name 'name' and widget flags set to 'f'." << endl;
        out << " *" << endl;
        out << " *  The " << objClass.mid(1).toLower() << " will by default be modeless, unless you set 'modal' to" << endl;
        out << " *  true to construct a modal " << objClass.mid(1).toLower() << "." << endl;
        out << " */" << endl;
        out << nameOfClass << "::" << bareNameOfClass << "(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)" << endl;
        out << "    : " << objClass << "(parent, name, modal, fl)";
    } else if (objClass == QLatin1String("QWidget"))  {
        out << "/*" << endl;
        out << " *  Constructs a " << nameOfClass << " as a child of 'parent', with the" << endl;
        out << " *  name 'name' and widget flags set to 'f'." << endl;
        out << " */" << endl;
        out << nameOfClass << "::" << bareNameOfClass << "(QWidget* parent, const char* name, Qt::WindowFlags fl)" << endl;
        out << "    : " << objClass << "(parent, name, fl)";
    } else if (objClass == QLatin1String("QMainWindow") || objClass == QLatin1String("Q3MainWindow")) {
        out << "/*" << endl;
        out << " *  Constructs a " << nameOfClass << " as a child of 'parent', with the" << endl;
        out << " *  name 'name' and widget flags set to 'f'." << endl;
        out << " *" << endl;
        out << " */" << endl;
        out << nameOfClass << "::" << bareNameOfClass << "(QWidget* parent, const char* name, Qt::WindowFlags fl)" << endl;
        out << "    : " << objClass << "(parent, name, fl)";
        isMainWindow = true;
    } else {
        out << "/*" << endl;
        out << " *  Constructs a " << nameOfClass << " which is a child of 'parent', with the" << endl;
        out << " *  name 'name'.' " << endl;
        out << " */" << endl;
        out << nameOfClass << "::" << bareNameOfClass << "(QWidget* parent, const char* name)" << endl;
        out << "    : " << objClass << "(parent, name)";
    }

    out << endl;

    out << "{" << endl;

//
// setup the gui
//
    out << indent << "setupUi(this);" << endl << endl;


    if (isMainWindow)
        out << indent << "(void)statusBar();" << endl;

    // database support
    dbConnections = unique(dbConnections);
    if (dbConnections.count())
        out << endl;
    for (it = dbConnections.begin(); it != dbConnections.end(); ++it) {
        if (!(*it).isEmpty() && (*it) != QLatin1String("(default)")) {
            out << indent << (*it) << "Connection = QSqlDatabase::database(\"" <<(*it) << "\");" << endl;
        }
    }

    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("widget"));
    for (i = 1; i < (int) nl.length(); i++) { // start at 1, 0 is the toplevel widget
        n = nl.item(i).toElement();
        QString s = getClassName(n);
        if ((dbForm || subDbForms) && (s == QLatin1String("QDataBrowser") || s == QLatin1String("QDataView"))) {
            QString objName = getObjectName(n);
            QString tab = getDatabaseInfo(n, QLatin1String("table"));
            QString con = getDatabaseInfo(n, QLatin1String("connection"));
            out << indent << "QSqlForm* " << objName << "Form = new QSqlForm(this);" << endl;
            out << indent << objName << "Form->setObjectName(\"" << objName << "Form\");" << endl;
            QDomElement n2;
            for (n2 = n.firstChild().toElement(); !n2.isNull(); n2 = n2.nextSibling().toElement())
                createFormImpl(n2, objName, con, tab);
            out << indent << objName << "->setForm(" << objName << "Form);" << endl;
        }
    }

    if (extraFuncts.contains(QLatin1String("init()")))
        out << indent << "init();" << endl;

    // end of constructor
    out << "}" << endl;
    out << endl;

    // destructor
    out << "/*" << endl;
    out << " *  Destroys the object and frees any allocated resources" << endl;
    out << " */" << endl;
    out << nameOfClass << "::~" << bareNameOfClass << "()" << endl;
    out << "{" << endl;
    if (extraFuncts.contains(QLatin1String("destroy()")))
        out << indent << "destroy();" << endl;
    out << indent << "// no need to delete child widgets, Qt does it all for us" << endl;
    out << "}" << endl;
    out << endl;

    // handle application events if required
    bool needFontEventHandler = false;
    bool needSqlTableEventHandler = false;
    bool needSqlDataBrowserEventHandler = false;
    nl = e.elementsByTagName(QLatin1String("widget"));
    for (i = 0; i < (int) nl.length(); i++) {
        if (!DomTool::propertiesOfType(nl.item(i).toElement() , QLatin1String("font")).isEmpty())
            needFontEventHandler = true;
        QString s = getClassName(nl.item(i).toElement());
        if (s == QLatin1String("QDataTable") || s == QLatin1String("QDataBrowser")) {
            if (!isFrameworkCodeGenerated(nl.item(i).toElement()))
                 continue;
            if (s == QLatin1String("QDataTable"))
                needSqlTableEventHandler = true;
            if (s == QLatin1String("QDataBrowser"))
                needSqlDataBrowserEventHandler = true;
        }
        if (needFontEventHandler && needSqlTableEventHandler && needSqlDataBrowserEventHandler)
            break;
    }

    out << "/*" << endl;
    out << " *  Sets the strings of the subwidgets using the current" << endl;
    out << " *  language." << endl;
    out << " */" << endl;
    out << "void " << nameOfClass << "::languageChange()" << endl;
    out << "{" << endl;
    out << "    retranslateUi(this);" << endl;
    out << "}" << endl;
    out << endl;

    // create stubs for additional slots if necessary
    if (!extraFuncts.isEmpty() && writeFunctImpl) {
        it = extraFuncts.begin();
        QStringList::Iterator it2 = extraFunctTyp.begin();
        QStringList::Iterator it3 = extraFunctSpecifier.begin();
        while (it != extraFuncts.end()) {
            QString type = fixDeclaration(*it2);
            if (type.isEmpty())
                type = QLatin1String("void");
            type = type.simplified();
            QString fname = fixDeclaration(Parser::cleanArgs(*it));
            if (!(*it3).startsWith(QLatin1String("pure"))) { // "pure virtual" or "pureVirtual"
                out << type << " " << nameOfClass << "::" << fname << endl;
                out << "{" << endl;
                if (*it != QLatin1String("init()") && *it != QLatin1String("destroy()")) {
                    QRegExp numeric(QLatin1String("^(?:signed|unsigned|u?char|u?short|u?int"
                                     "|u?long|Q_U?INT(?:8|16|32)|Q_U?LONG|float"
                                     "|double)$"));
                    QString retVal;

                    /*
                      We return some kind of dummy value to shut the
                      compiler up.

                      1.  If the type is 'void', we return nothing.

                      2.  If the type is 'bool', we return 'false'.

                      3.  If the type is 'unsigned long' or
                          'quint16' or 'double' or similar, we
                          return '0'.

                      4.  If the type is 'Foo *', we return '0'.

                      5.  If the type is 'Foo &', we create a static
                          variable of type 'Foo' and return it.

                      6.  If the type is 'Foo', we assume there's a
                          default constructor and use it.
                    */
                    if (type != QLatin1String("void")) {
                        QStringList toks = type.split(QLatin1String(" "));
                        bool isBasicNumericType =
                                (toks.filter(numeric).count() == toks.count());

                        if (type == QLatin1String("bool")) {
                            retVal = QLatin1String("false");
                        } else if (isBasicNumericType || type.endsWith(QLatin1String("*"))) {
                            retVal = QLatin1String("0");
                        } else if (type.endsWith(QLatin1String("&"))) {
                            do {
                                type.chop(1);
                            } while (type.endsWith(QLatin1String(" ")));
                            retVal = QLatin1String("uic_temp_var");
                            out << indent << "static " << type << " " << retVal << ";" << endl;
                        } else {
                            retVal = type + QLatin1String("()");
                        }
                    }

                    out << indent << "qWarning(\"" << nameOfClass << "::" << fname << ": Not implemented yet\");" << endl;
                    if (!retVal.isEmpty())
                        out << indent << "return " << retVal << ";" << endl;
                }
                out << "}" << endl;
                out << endl;
            }
            ++it;
            ++it2;
            ++it3;
        }
    }
}


/*! Creates form support implementation code for the widgets given
  in \a e.

  Traverses recursively over all children.
 */

void Ui3Reader::createFormImpl(const QDomElement& e, const QString& form, const QString& connection, const QString& table)
{
    if (e.tagName() == QLatin1String("widget")
            && e.attribute(QLatin1String("class")) != QLatin1String("QDataTable")) {
        QString field = getDatabaseInfo(e, QLatin1String("field"));
        if (!field.isEmpty()) {
            if (isWidgetInTable(e, connection, table))
                out << indent << form << "Form->insert(" << getObjectName(e) << ", " << fixString(field) << ");" << endl;
        }
    }
    QDomElement n;
    for (n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement()) {
        createFormImpl(n, form, connection, table);
    }
}

QT_END_NAMESPACE
