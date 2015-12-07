/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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

#ifndef TRPREVIEWTOOL_H
#define TRPREVIEWTOOL_H

#include <QMainWindow>
#include <QWorkspace>
#include <QHash>
#include <QComboBox>
#include <QUiLoader>
#include <QTranslator>
#include <QLayout>

#include "ui_trpreviewtool.h"

QT_BEGIN_NAMESPACE

class FormHolder : public QWidget
{
    Q_OBJECT

public:
    FormHolder(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    bool loadFormFile(const QString& path);
    void retranslate();
    QString formFilePath();
    virtual QSize sizeHint() const;

private:
    static QUiLoader* uiLoader;
    QString formPath;
    QWidget* form;
    QHBoxLayout* layout;
    QSize m_sizeHint;
};

class QStandardItemModel;

class TrPreviewTool : public QMainWindow
{
    Q_OBJECT

public:
    TrPreviewTool(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~TrPreviewTool();
    void addFormFile(const QString &path);
    bool loadTranslation(const QString &path, const QString &displayName = QString());
    void addTranslator(QTranslator *translator, const QString &displayName);
    void cascade();

public slots:
    void openForm();
    void loadTranslation();
    void translationSelected(int idx);
    void showAboutBox();
    void on_viewForms_doubleClicked(const QModelIndex &);
    void reloadTranslations();

private:
    virtual bool event(QEvent *e);  // override from QWidget
    void addTranslator(QTranslator *translator, const QString &path, const QString &displayName);
    FormHolder* createFormFromFile(const QString& path);
    void recreateForms();
    void showWarning(const QString& warning);
    Ui::TrPreviewToolClass ui;
    QWorkspace* workspace;
    QComboBox* trCombo;
    QTranslator* currentTr;
    QHash<QString,QTranslator*> trDict;
    QStandardItemModel *m_uiFilesModel;
};

QT_END_NAMESPACE

#endif // TRPREVIEWTOOL_H
