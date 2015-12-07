/*  This file is part of the KDE project.

    Copyright (C) 2007 Trolltech ASA. All rights reserved.

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 or 3 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "displaylinkcallback.h"
#include "backendheader.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

CVDisplayLinkRef DisplayLinkCallback::m_displayLink = 0;
int DisplayLinkCallback::m_refCount = 0;
CVTimeStamp DisplayLinkCallback::m_timeStamp;
bool DisplayLinkCallback::m_initialized = false;

static CVReturn displayLinkCallback(CVDisplayLinkRef /*displayLink*/,
								 const CVTimeStamp */*inNow*/,
								 const CVTimeStamp */*inOutputTime*/,
								 CVOptionFlags /*flagsIn*/,
								 CVOptionFlags */*flagsOut*/,
                                 void */*displayLinkContext*/)
{
    return kCVReturnSuccess;
}

CVTimeStamp LinkTimeProxy::getCVTimeStamp() const
{
    return m_timeStamp;
}

void LinkTimeProxy::setCVTimeStamp(const CVTimeStamp &timeStamp)
{
    m_timeStamp = timeStamp;
}

LinkTimeProxy DisplayLinkCallback::currentTime()
{
    LinkTimeProxy proxy;
    CVDisplayLinkGetCurrentTime(m_displayLink, &m_timeStamp);
    proxy.setCVTimeStamp(m_timeStamp);
    return proxy;
}

void DisplayLinkCallback::retain()
{
    ++m_refCount;
    if (m_initialized)
        return;

    // First subscriber. Initialize:
    m_initialized = true;
    memset(&m_timeStamp, 0, sizeof(m_timeStamp));
    OSStatus err = EnterMovies();
    BACKEND_ASSERT2(err == noErr, "Could not initialize QuickTime", FATAL_ERROR)
    err = CVDisplayLinkCreateWithCGDisplay(kCGDirectMainDisplay, &m_displayLink);
	BACKEND_ASSERT2(err == noErr, "Could not create display link", FATAL_ERROR)
    err = CVDisplayLinkSetCurrentCGDisplay(m_displayLink, kCGDirectMainDisplay);
	BACKEND_ASSERT2(err == noErr, "Could not set the current display for the display link", FATAL_ERROR)
        err = CVDisplayLinkSetOutputCallback(m_displayLink, displayLinkCallback, 0);
    BACKEND_ASSERT2(err == noErr, "Could not set the callback for the display link", FATAL_ERROR)
    err = CVDisplayLinkStart(m_displayLink);
	BACKEND_ASSERT2(err == noErr, "Could not start the display link", FATAL_ERROR)
}

void DisplayLinkCallback::release()
{
    BACKEND_ASSERT2(m_refCount > 0, "Release called for display link with refcount < 1", NORMAL_ERROR)
    --m_refCount;
    if (m_refCount == 0){
        CVDisplayLinkStop(m_displayLink);
        CFRelease(m_displayLink);
        m_displayLink = 0;
        ExitMovies();
        m_initialized = false;
    }
}

}} // namespace Phonon::QT7

QT_END_NAMESPACE
