/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
    void raiseErrorWithLine();
};

void QHelpProjectDataPrivate::raiseErrorWithLine()
{
    raiseError(QObject::tr("Unknown token at line %1.")
                    .arg(lineNumber()));
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
                raiseError(QObject::tr("Unknown token at line %1. Expected \"QtHelpProject\"!")
                    .arg(lineNumber()));
		}
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
				raiseErrorWithLine();
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
				raiseErrorWithLine();
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
				raiseErrorWithLine();
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
				raiseErrorWithLine();
			}
		} else if (isEndElement()) {
			if (name() == QLatin1String("section")) {
                contentStack.pop();
				continue;
            } else if (name() == QLatin1String("toc") && contentStack.isEmpty()) {
				break;
			} else {
				raiseErrorWithLine();
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
				raiseErrorWithLine();
			}
		} else if (isEndElement()) {
			if (name() == QLatin1String("keyword"))
				continue;
			else if (name() == QLatin1String("keywords"))
				break;
			else
				raiseErrorWithLine();
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
				raiseErrorWithLine();
		} else if (isEndElement()) {
			if (name() == QLatin1String("file"))
				continue;
			else if (name() == QLatin1String("files"))
				break;
			else
				raiseErrorWithLine();
		}
	}
}



/*!
    \internal
    \class QHelpProjectData
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
