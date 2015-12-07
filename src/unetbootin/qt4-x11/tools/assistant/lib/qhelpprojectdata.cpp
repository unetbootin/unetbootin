/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "qhelpprojectdata_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QStack>
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtXml/QXmlStreamReader>

QT_BEGIN_NAMESPACE

class QHelpProjectDataPrivate : public QXmlStreamReader
{
public:
    void readData(const QByteArray &contents);

    QString virtualFolder;
    QString namespaceName;
    QString rootPath;

    QStringList fileList;
    QList<QHelpDataCustomFilter> customFilterList;
    QList<QHelpDataFilterSection> filterSectionList;
    QMap<QString, QVariant> metaData;

    QString errorMsg;

private:
    void readProject();
    void readCustomFilter();
    void readFilterSection();
    void readTOC();
    void readKeywords();
    void readFiles();
    void raiseUnknownTokenError();
};

void QHelpProjectDataPrivate::raiseUnknownTokenError()
{
    raiseError(QObject::tr("Unknown token."));
}

void QHelpProjectDataPrivate::readData(const QByteArray &contents)
{
    addData(contents);
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("QtHelpProject")
                && attributes().value(QLatin1String("version")) == QLatin1String("1.0"))
                readProject();
            else
                raiseError(QObject::tr("Unknown token. Expected \"QtHelpProject\"!"));
        }
    }

    if (hasError()) {
        raiseError(QObject::tr("Error in line %1: %2").arg(lineNumber())
            .arg(errorString()));
    }
}

void QHelpProjectDataPrivate::readProject()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("virtualFolder")) {
                virtualFolder = readElementText();
                if (virtualFolder.contains(QLatin1String("/")))
                    raiseError(QObject::tr("A virtual folder must not contain a \'/\' character!"));
            } else if (name() == QLatin1String("namespace")) {
                namespaceName = readElementText();
                if (namespaceName.contains(QLatin1String("/")))
                    raiseError(QObject::tr("A namespace must not contain a \'/\' character!"));
            } else if (name() == QLatin1String("customFilter")) {
                readCustomFilter();
            } else if (name() == QLatin1String("filterSection")) {
                readFilterSection();
            } else if (name() == QLatin1String("metaData")) {
                QString n = attributes().value(QLatin1String("name")).toString();
                if (!metaData.contains(n))
                    metaData[n] = attributes().value(QLatin1String("value")).toString();
                else
                    metaData.insert(n, attributes().value(QLatin1String("value")).toString());
            } else {
                raiseUnknownTokenError();
            }
        } else if (isEndElement() && name() == QLatin1String("QtHelpProject")) {
            if (namespaceName.isEmpty())
                raiseError(QObject::tr("Missing namespace in QtHelpProject."));
            else if (virtualFolder.isEmpty())
                raiseError(QObject::tr("Missing virtual folder in QtHelpProject"));
            break;
        }
    }
}

void QHelpProjectDataPrivate::readCustomFilter()
{
    QHelpDataCustomFilter filter;
    filter.name = attributes().value(QLatin1String("name")).toString();
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("filterAttribute"))
                filter.filterAttributes.append(readElementText());
            else
                raiseUnknownTokenError();
        } else if (isEndElement() && name() == QLatin1String("customFilter")) {
            break;
        }
    }
    customFilterList.append(filter);
}

void QHelpProjectDataPrivate::readFilterSection()
{
    filterSectionList.append(QHelpDataFilterSection());
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("filterAttribute"))
                filterSectionList.last().addFilterAttribute(readElementText());				
            else if (name() == QLatin1String("toc"))
                readTOC();
            else if (name() == QLatin1String("keywords"))
                readKeywords();
            else if (name() == QLatin1String("files"))
                readFiles();
            else
                raiseUnknownTokenError();
        } else if (isEndElement() && name() == QLatin1String("filterSection")) {
            break;
        }
    }
}

void QHelpProjectDataPrivate::readTOC()
{
    QStack<QHelpDataContentItem*> contentStack;
    QHelpDataContentItem *itm = 0;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("section")) {
                QString title = attributes().value(QLatin1String("title")).toString();
                QString ref = attributes().value(QLatin1String("ref")).toString();
                if (contentStack.isEmpty()) {
                    itm = new QHelpDataContentItem(0, title, ref);
                    filterSectionList.last().addContent(itm);
                } else {
                    itm = new QHelpDataContentItem(contentStack.top(), title, ref);
                }
                contentStack.push(itm);
            } else {
                raiseUnknownTokenError();
            }
        } else if (isEndElement()) {
            if (name() == QLatin1String("section")) {
                contentStack.pop();
                continue;
            } else if (name() == QLatin1String("toc") && contentStack.isEmpty()) {
                break;
            } else {
                raiseUnknownTokenError();
            }
        }
    }
}

void QHelpProjectDataPrivate::readKeywords()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("keyword")) {
                if (attributes().value(QLatin1String("ref")).toString().isEmpty()
                    || (attributes().value(QLatin1String("name")).toString().isEmpty()
                    && attributes().value(QLatin1String("id")).toString().isEmpty()))
                    raiseError(QObject::tr("Missing attribute in keyword at line %1.")
                        .arg(lineNumber()));
                filterSectionList.last().addIndex(
                    QHelpDataIndexItem(attributes().value(QLatin1String("name")).toString(),
                        attributes().value(QLatin1String("id")).toString(),
                        attributes().value(QLatin1String("ref")).toString()));
            } else {
                raiseUnknownTokenError();
            }
        } else if (isEndElement()) {
            if (name() == QLatin1String("keyword"))
                continue;
            else if (name() == QLatin1String("keywords"))
                break;
            else
                raiseUnknownTokenError();
        }
    }
}

void QHelpProjectDataPrivate::readFiles()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("file"))
                filterSectionList.last().addFile(readElementText());
            else
                raiseUnknownTokenError();
        } else if (isEndElement()) {
            if (name() == QLatin1String("file"))
                continue;
            else if (name() == QLatin1String("files"))
                break;
            else
                raiseUnknownTokenError();
        }
    }
}



/*!
    \internal
    \class QHelpProjectData
    \since 4.4
    \brief The QHelpProjectData class stores all information found
    in a Qt help project file.

    The structure is filled with data by calling readData(). The
    specified file has to have the Qt help project file format in
    order to be read successfully. Possible reading errors can be
    retrieved by calling errorMessage().
*/

/*!
    Constructs a Qt help project data structure.
*/
QHelpProjectData::QHelpProjectData()
{
    d = new QHelpProjectDataPrivate;
}

/*!
    Destroys the help project data.
*/
QHelpProjectData::~QHelpProjectData()
{
    delete d;
}

/*!
    Reads the file \a fileName and stores the help data. The file has to
    have the Qt help project file format. Returns true if the file
    was successfully read, otherwise false.

    \sa errorMessage()
*/
bool QHelpProjectData::readData(const QString &fileName)
{
    d->rootPath = QFileInfo(fileName).absolutePath();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        d->errorMsg = QObject::tr("The input file %1 could not be opened!")
            .arg(fileName);
        return false;
    }

    d->readData(file.readAll());
    return !d->hasError();
}

/*!
    Returns an error message if the reading of the Qt help project
    file failed. Otherwise, an empty QString is returned.

    \sa readData()
*/
QString QHelpProjectData::errorMessage() const
{
    if (d->hasError())
        return d->errorString();
    return d->errorMsg;
}

/*!
    \reimp
*/
QString QHelpProjectData::namespaceName() const
{
    return d->namespaceName;
}

/*!
    \reimp
*/
QString QHelpProjectData::virtualFolder() const
{
    return d->virtualFolder;
}

/*!
    \reimp
*/
QList<QHelpDataCustomFilter> QHelpProjectData::customFilters() const
{
    return d->customFilterList;
}

/*!
    \reimp
*/
QList<QHelpDataFilterSection> QHelpProjectData::filterSections() const
{
    return d->filterSectionList;
}

/*!
    \reimp
*/
QMap<QString, QVariant> QHelpProjectData::metaData() const
{
    return d->metaData;
}

/*!
    \reimp
*/
QString QHelpProjectData::rootPath() const
{
    return d->rootPath;
}

QT_END_NAMESPACE
