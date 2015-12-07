/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>

#include "xmlwriter.h"

QDomDocument *XmlWriter::toXml()
{
    QDomImplementation implementation;
    QDomDocumentType docType = implementation.createDocumentType(
        "scribe-document", "scribe", "www.trolltech.com/scribe");

    document = new QDomDocument(docType);

    // ### This processing instruction is required to ensure that any kind
    // of encoding is given when the document is written.
    QDomProcessingInstruction process = document->createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"utf-8\"");
    document->appendChild(process);

    QDomElement documentElement = document->createElement("document");
    document->appendChild(documentElement);

//! [0]
    QTextBlock currentBlock = textDocument->begin();

    while (currentBlock.isValid()) {
//! [0]
        QDomElement blockElement = document->createElement("block");
        document->appendChild(blockElement);

        readFragment(currentBlock, blockElement, document);

//! [1]
        processBlock(currentBlock);
//! [1]

//! [2]
        currentBlock = currentBlock.next();
    }
//! [2]

    return document;
}

void XmlWriter::readFragment(const QTextBlock &currentBlock,
                             QDomElement blockElement,
                             QDomDocument *document)
{
//! [3] //! [4]
    QTextBlock::iterator it;
    for (it = currentBlock.begin(); !(it.atEnd()); ++it) {
        QTextFragment currentFragment = it.fragment();
        if (currentFragment.isValid())
//! [3] //! [5]
            processFragment(currentFragment);
//! [4] //! [5]

        if (currentFragment.isValid()) {
            QDomElement fragmentElement = document->createElement("fragment");
            blockElement.appendChild(fragmentElement);

            fragmentElement.setAttribute("length", currentFragment.length());
            QDomText fragmentText = document->createTextNode(currentFragment.text());

            fragmentElement.appendChild(fragmentText);
//! [6]
        }
//! [7]
    }
//! [6] //! [7]
}

void XmlWriter::processBlock(const QTextBlock &currentBlock)
{
}

void XmlWriter::processFragment(const QTextFragment &currentFragment)
{
}
