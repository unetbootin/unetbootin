/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDomDocument>
#include <QMainWindow>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QComboBox;
class QFile;
class QGroupBox;
class QLabel;
class QListWidget;
class QSqlRelationalTableModel;
class QTableView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &artistTable, const QString &albumTable,
               QFile *albumDetails, QWidget *parent = 0);

private slots:
    void about();
    void addAlbum();
    void changeArtist(int row);
    void deleteAlbum();
    void showAlbumDetails(QModelIndex index);
    void showArtistProfile(QModelIndex index);
    void updateHeader(QModelIndex, int, int);

private:
    void adjustHeader();
    QGroupBox *createAlbumGroupBox();
    QGroupBox *createArtistGroupBox();
    QGroupBox *createDetailsGroupBox();
    void createMenuBar();
    void decreaseAlbumCount(QModelIndex artistIndex);
    void getTrackList(QDomNode album);
    QModelIndex indexOfArtist(const QString &artist);
    void readAlbumData();
    void removeAlbumFromDatabase(QModelIndex album);
    void removeAlbumFromFile(int id);
    void showImageLabel();

    QTableView *albumView;
    QComboBox *artistView;
    QListWidget *trackList;

    QLabel *iconLabel;
    QLabel *imageLabel;
    QLabel *profileLabel;
    QLabel *titleLabel;

    QDomDocument albumData;
    QFile *file;
    QSqlRelationalTableModel *model;
};

#endif
