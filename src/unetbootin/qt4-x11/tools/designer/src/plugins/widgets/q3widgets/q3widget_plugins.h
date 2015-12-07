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

#ifndef Q3WIDGET_PLUGINS_H
#define Q3WIDGET_PLUGINS_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

QT_BEGIN_NAMESPACE

class Q3ButtonGroupPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3ButtonGroupPlugin(const QIcon &icon, QObject *parent = 0);
    virtual ~Q3ButtonGroupPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

class Q3ComboBoxPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3ComboBoxPlugin(const QIcon &icon, QObject *parent = 0);
    virtual ~Q3ComboBoxPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

class Q3DateEditPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3DateEditPlugin(const QIcon &icon, QObject *parent = 0);

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;
    virtual bool isContainer() const;
    virtual QWidget *createWidget(QWidget *parent);
    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);
    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

class Q3DateTimeEditPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3DateTimeEditPlugin(const QIcon &icon, QObject *parent = 0);

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;
    virtual bool isContainer() const;
    virtual QWidget *createWidget(QWidget *parent);
    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);
    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

class Q3FramePlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3FramePlugin(const QIcon &icon, QObject *parent = 0);
    virtual ~Q3FramePlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

class Q3GroupBoxPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3GroupBoxPlugin(const QIcon &icon, QObject *parent = 0);
    virtual ~Q3GroupBoxPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

class Q3ProgressBarPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3ProgressBarPlugin(const QIcon &icon, QObject *parent = 0);

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;
    virtual bool isContainer() const;
    virtual QWidget *createWidget(QWidget *parent);
    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);
    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

class Q3TextBrowserPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3TextBrowserPlugin(const QIcon &icon, QObject *parent = 0);

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;
    virtual bool isContainer() const;
    virtual QWidget *createWidget(QWidget *parent);
    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);
    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

class Q3TimeEditPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    Q3TimeEditPlugin(const QIcon &icon, QObject *parent = 0);

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;
    virtual bool isContainer() const;
    virtual QWidget *createWidget(QWidget *parent);
    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);
    virtual QString domXml() const;

private:
    bool m_initialized;
    QIcon m_icon;
};

QT_END_NAMESPACE

#endif // Q3WIDGET_PLUGINS_H
