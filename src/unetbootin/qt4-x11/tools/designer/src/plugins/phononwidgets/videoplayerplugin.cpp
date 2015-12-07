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

#include "videoplayerplugin.h"
#include "videoplayertaskmenu.h"

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtCore/qplugin.h>
#include <phonon/videoplayer.h>

static const char *toolTipC = "Phonon Video Player";

QT_BEGIN_NAMESPACE

VideoPlayerPlugin::VideoPlayerPlugin(const QString &group, QObject *parent) :
    QObject(parent),
    m_group(group),
    m_initialized(false)
{
}

QString VideoPlayerPlugin::name() const
{
    return QLatin1String("Phonon::VideoPlayer");
}

QString VideoPlayerPlugin::group() const
{
    return m_group;
}

QString VideoPlayerPlugin::toolTip() const
{
    return QString(QLatin1String(toolTipC));
}

QString VideoPlayerPlugin::whatsThis() const
{
    return QString(QLatin1String(toolTipC));
}

QString VideoPlayerPlugin::includeFile() const
{
    return QLatin1String("<phonon/videoplayer.h>");
}

QIcon VideoPlayerPlugin::icon() const
{
    return QIcon(QLatin1String(":/trolltech/phononwidgets/images/videoplayer.png"));
}

bool VideoPlayerPlugin::isContainer() const
{
    return false;
}

QWidget *VideoPlayerPlugin::createWidget(QWidget *parent)
{
    return new Phonon::VideoPlayer(Phonon::NoCategory, parent);
}

bool VideoPlayerPlugin::isInitialized() const
{
    return m_initialized;
}

void VideoPlayerPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (m_initialized)
        return;

    QExtensionManager *mgr = core->extensionManager();
    VideoPlayerTaskMenuFactory::registerExtension(mgr, Q_TYPEID(QDesignerTaskMenuExtension));
    m_initialized = true;
}

QString VideoPlayerPlugin::domXml() const
{
    return QLatin1String("\
    <ui language=\"c++\">\
        <widget class=\"Phonon::VideoPlayer\" name=\"videoPlayer\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>300</width>\
                    <height>200</height>\
                </rect>\
            </property>\
        </widget>\
    </ui>");
}

QT_END_NAMESPACE
