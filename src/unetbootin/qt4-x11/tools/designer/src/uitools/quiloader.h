/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef QUILOADER_H
#define QUILOADER_H

#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QWidget;
class QLayout;
class QAction;
class QActionGroup;
class QString;
class QIODevice;
class QDir;

class QUiLoaderPrivate;
class QUiLoader : public QObject
{
    Q_OBJECT
public:
    QUiLoader(QObject *parent = 0);
    virtual ~QUiLoader();

    QStringList pluginPaths() const;
    void clearPluginPaths();
    void addPluginPath(const QString &path);

    QWidget *load(QIODevice *device, QWidget *parentWidget = 0);
    QStringList availableWidgets() const;
    QStringList availableLayouts() const;

    virtual QWidget *createWidget(const QString &className, QWidget *parent = 0, const QString &name = QString());
    virtual QLayout *createLayout(const QString &className, QObject *parent = 0, const QString &name = QString());
    virtual QActionGroup *createActionGroup(QObject *parent = 0, const QString &name = QString());
    virtual QAction *createAction(QObject *parent = 0, const QString &name = QString());

    void setWorkingDirectory(const QDir &dir);
    QDir workingDirectory() const;

    void setScriptingEnabled(bool enabled);
    bool isScriptingEnabled() const;

    void setLanguageChangeEnabled(bool enabled);
    bool isLanguageChangeEnabled() const;

    void setTranslationEnabled(bool enabled);
    bool isTranslationEnabled() const;

private:
    QUiLoaderPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QUiLoader)
    Q_DISABLE_COPY(QUiLoader)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QUILOADER_H
