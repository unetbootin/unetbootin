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

#include "phrasemodel.h"

QT_BEGIN_NAMESPACE

void PhraseModel::removePhrases()
{
    int r = plist.count();
    if (r > 0) {
        plist.clear();
        reset();
    }
}

Phrase *PhraseModel::phrase(const QModelIndex &index) const
{
    return plist.at(index.row());
}

void PhraseModel::setPhrase(const QModelIndex &indx, Phrase *ph)
{
    int r = indx.row();

    plist[r] = ph;

    // update item in view
    const QModelIndex &si = index(r, 0);
    const QModelIndex &ei = index(r, 2);
    emit dataChanged(si, ei);
}

QModelIndex PhraseModel::addPhrase(Phrase *p)
{
    int r = plist.count();

    plist.append(p);

    // update phrases as we add them
    beginInsertRows(QModelIndex(), r, r);
    QModelIndex i = index(r, 0);
    endInsertRows();
    return i;
}

void PhraseModel::removePhrase(const QModelIndex &index)
{
    int r = index.row();
    beginRemoveRows(QModelIndex(), r, r);
    plist.removeAt(r);
    endRemoveRows();
}

QModelIndex PhraseModel::index(Phrase * const phr) const
{
    int row;
    if ((row = plist.indexOf(phr)) == -1)
        return QModelIndex();

    return index(row, 0);
}

int PhraseModel::rowCount(const QModelIndex &) const
{
    return plist.count();
}

int PhraseModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant PhraseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        switch(section) {
        case 0:
            return tr("Source phrase");
        case 1:
            return tr("Translation");
        case 2:
            return tr("Definition");
        }
    }

    return QVariant();
}

Qt::ItemFlags PhraseModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    // Edit is allowed for source & translation if item is from phrasebook
    if (plist.at(index.row())->phraseBook()
        && (index.column() != 2))
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool PhraseModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    int row = index.row();
    int column = index.column();

    if (!index.isValid() || row >= plist.count() || role != Qt::EditRole)
        return false;

    Phrase *phrase = plist.at(row);

    switch (column) {
    case 0:
        phrase->setSource(value.toString());
        break;
    case 1:
        phrase->setTarget(value.toString());
        break;
    case 2:
        phrase->setDefinition(value.toString());
        break;
    default:
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

QVariant PhraseModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if (row >= plist.count() || !index.isValid())
        return QVariant();

    Phrase *phrase = plist.at(row);

    if (role == Qt::DisplayRole || (role == Qt::ToolTipRole && column != 2)) {
        switch (column) {
        case 0: // source phrase
            return phrase->source().simplified();
        case 1: // translation
            return phrase->target().simplified();
        case 2: // definition
            return phrase->definition();
        }
    }
    else if (role == Qt::EditRole && column != 2) {
        switch (column) {
        case 0: // source phrase
            return phrase->source();
        case 1: // translation
            return phrase->target();
        }
    }

    return QVariant();
}

QT_END_NAMESPACE
