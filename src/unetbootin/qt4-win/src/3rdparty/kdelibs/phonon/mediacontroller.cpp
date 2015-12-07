/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "mediacontroller.h"
#include "mediaobject.h"
#include "addoninterface.h"
#include <QtCore/QList>
#include <QtCore/QVariant>
#include "frontendinterface_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

class MediaControllerPrivate : public FrontendInterfacePrivate
{
    public:
        MediaControllerPrivate(MediaObject *mp) : FrontendInterfacePrivate(mp) {}

        virtual void backendObjectChanged(QObject *);
        MediaController *q;
};

MediaController::MediaController(MediaObject *mp)
    : QObject(mp)
    , d(new MediaControllerPrivate(mp))
{
    d->q = this;
    d->_backendObjectChanged();
}

void MediaControllerPrivate::backendObjectChanged(QObject *m_backendObject)
{
    QObject::connect(m_backendObject, SIGNAL(titleChanged(int)), q, SIGNAL(titleChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(availableTitlesChanged(int)), q, SIGNAL(availableTitlesChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(chapterChanged(int)), q, SIGNAL(chapterChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(availableChaptersChanged(int)), q, SIGNAL(availableChaptersChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(angleChanged(int)), q, SIGNAL(angleChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(availableAnglesChanged(int)), q, SIGNAL(availableAnglesChanged(int)));
}

MediaController::~MediaController()
{
    delete d;
}

#define IFACE \
    AddonInterface *iface = d->iface(); \
    if (!iface) return

MediaController::Features MediaController::supportedFeatures() const
{
    if (!d || !d->media) {
        return false;
    }
    IFACE false;
    Features ret;
    if (iface->hasInterface(AddonInterface::AngleInterface)) {
        ret |= Angles;
    }
    if (iface->hasInterface(AddonInterface::ChapterInterface)) {
        ret |= Chapters;
    }
    if (iface->hasInterface(AddonInterface::TitleInterface)) {
        ret |= Titles;
    }
    return ret;
}

int MediaController::availableTitles() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::availableTitles).toInt();
}

int MediaController::currentTitle() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::title).toInt();
}

void MediaController::setCurrentTitle(int titleNumber)
{
    IFACE;
    iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::setTitle, QList<QVariant>() << QVariant(titleNumber));
}

bool MediaController::autoplayTitles() const
{
    IFACE true;
    return iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::autoplayTitles).toBool();
}

void MediaController::setAutoplayTitles(bool b)
{
    IFACE;
    iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::setAutoplayTitles, QList<QVariant>() << QVariant(b));
}

void MediaController::nextTitle()
{
    setCurrentTitle(currentTitle() + 1);
}

void MediaController::previousTitle()
{
    setCurrentTitle(currentTitle() - 1);
}

int MediaController::availableChapters() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::ChapterInterface,
            AddonInterface::availableChapters).toInt();
}

int MediaController::currentChapter() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::ChapterInterface,
            AddonInterface::chapter).toInt();
}

void MediaController::setCurrentChapter(int titleNumber)
{
    IFACE;
    iface->interfaceCall(AddonInterface::ChapterInterface,
            AddonInterface::setChapter, QList<QVariant>() << QVariant(titleNumber));
}

int MediaController::availableAngles() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::AngleInterface,
            AddonInterface::availableAngles).toInt();
}

int MediaController::currentAngle() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::AngleInterface,
            AddonInterface::angle).toInt();
}

void MediaController::setCurrentAngle(int titleNumber)
{
    IFACE;
    iface->interfaceCall(AddonInterface::AngleInterface,
            AddonInterface::setAngle, QList<QVariant>() << QVariant(titleNumber));
}

#undef IFACE

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_mediacontroller.cpp"

// vim: sw=4 sts=4 et tw=100
