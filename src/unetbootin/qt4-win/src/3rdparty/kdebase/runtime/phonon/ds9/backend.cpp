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

#include "backend.h"
#include "backendnode.h"

#include "audiooutput.h"
#include "effect.h"
#include "mediaobject.h"
#include "videowidget.h"
#include "volumeeffect.h"

//windows specific (DirectX Media Object)
#include <dmo.h>

#include <QtCore/QSettings>
#include <QtCore/QSet>
#include <QtCore/QVariant>

#include <QtCore/QtPlugin>

QT_BEGIN_NAMESPACE

// export as Qt/KDE factory as required
#ifdef PHONON_MAKE_QT_ONLY_BACKEND
Q_EXPORT_PLUGIN2(phonon_ds9, Phonon::DS9::Backend);
#else
#include <kpluginfactory.h>
K_PLUGIN_FACTORY(DS9BackendFactory, registerPlugin<Phonon::DS9::Backend>();)
K_EXPORT_PLUGIN(DS9BackendFactory("ds9backend"))
#endif

namespace Phonon
{
    namespace DS9
    {
        //helper function
#ifndef Q_OS_WINCE
		static ComPointer<IMalloc> getIMalloc()
        {
            ComPointer<IMalloc> ret;
            ::CoGetMalloc(1, &ret);
            return ret;
        }
#endif

        bool Backend::AudioMoniker::operator==(const AudioMoniker &other)
        {
            return other->IsEqual(*this) == S_OK;
        }


        Backend::Backend(QObject *parent, const QVariantList &)
            : QObject(parent)
        {
            //registering meta types
            qRegisterMetaType<HRESULT>("HRESULT");
            qRegisterMetaType<QSet<Filter> >("QSet<Filter>");
        }

        Backend::~Backend()
        {
        }

        QObject *Backend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &args)
        {
            switch (c)
            {
            case MediaObjectClass:
                return new MediaObject(parent);
            case AudioOutputClass:
                return new AudioOutput(this, parent);
            case EffectClass:
                return new Effect(m_audioEffects[ args[0].toInt() ], parent);
            case VideoWidgetClass:
                return new VideoWidget(qobject_cast<QWidget *>(parent));
            case VolumeFaderEffectClass:
                return new VolumeEffect(parent);
            default:
                return 0;
            }
        }

        bool Backend::supportsVideo() const
        {
            return true;
        }

        QStringList Backend::availableMimeTypes() const
        {
            QSet<QString> ret;

            const QStringList locations = QStringList() << QLatin1String("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Multimedia\\mplayer2\\mime types")
                << QLatin1String("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Multimedia\\wmplayer\\mime types");

            foreach(QString s, locations) {
                QSettings settings(s, QSettings::NativeFormat);
                ret += settings.childGroups().toSet();
            }

            QStringList list = ret.toList();
            qSort(list);
            return list;
        }

        ComPointer<IMoniker> Backend::getAudioOutputMoniker(int index) const
        {
            ComPointer<IMoniker> ret;
            if (index >= 0) {
                ret = m_audioOutputs[index];
            }
            return ret;
        }


        QList<int> Backend::objectDescriptionIndexes(Phonon::ObjectDescriptionType type) const
        {
            QList<int> ret;

            switch(type)
            {
            case Phonon::AudioOutputDeviceType:
                {
#ifndef Q_OS_WINCE
					ComPointer<ICreateDevEnum> devEnum(CLSID_SystemDeviceEnum, IID_ICreateDevEnum);
					if (!devEnum) {
						return ret; //it is impossible to enumerate the devices
					}
                    ComPointer<IEnumMoniker> enumMon;
                    HRESULT hr = devEnum->CreateClassEnumerator(CLSID_AudioRendererCategory, &enumMon, 0);
                    if (FAILED(hr)) {
                        break;
                    }
                    AudioMoniker mon;

                    //let's reorder the devices so that directshound appears first
                    int nbds = 0; //number of directsound devices

                    while (S_OK == enumMon->Next(1, &mon, 0)) {
                        LPOLESTR str = 0;
                        mon->GetDisplayName(0,0,&str);
                        const QString name = QString::fromWCharArray(str);
                        getIMalloc()->Free(str);

                        int insert_pos = 0;
                        if (!m_audioOutputs.contains(mon)) {
                            insert_pos = m_audioOutputs.count();
                            m_audioOutputs.append(mon);
                        } else {
                            insert_pos = m_audioOutputs.indexOf(mon);
                        }

                        if (name.contains(QLatin1String("DirectSound"))) {
                            ret.insert(nbds++, insert_pos);
                        } else {
                            ret.append(insert_pos);
                        }
                    }
#endif
					break;
                }
            case Phonon::EffectType:
                {
                    m_audioEffects.clear();
                    ComPointer<IEnumDMO> enumDMO;
                    HRESULT hr = DMOEnum(DMOCATEGORY_AUDIO_EFFECT, DMO_ENUMF_INCLUDE_KEYED, 0, 0, 0, 0, &enumDMO);
                    if (SUCCEEDED(hr)) {
                        CLSID clsid;
                        while (S_OK == enumDMO->Next(1, &clsid, 0, 0)) {
                            ret += m_audioEffects.count();
                            m_audioEffects.append(clsid);
                        }
                    }
                    break;
                }
                break;
            default:
                break;
            }
			return ret;
        }

        QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(Phonon::ObjectDescriptionType type, int index) const
        {
            QHash<QByteArray, QVariant> ret;
            switch (type)
            {
            case Phonon::AudioOutputDeviceType:
                {
#ifndef Q_OS_WINCE
                    const AudioMoniker &mon = m_audioOutputs[index];
                    LPOLESTR str = 0;
                    HRESULT hr = mon->GetDisplayName(0,0, &str);
                    if (SUCCEEDED(hr)) {
                        QString name = QString::fromWCharArray(str); 
                        getIMalloc()->Free(str);
                        ret["name"] = name.mid(name.indexOf('\\') + 1);
					}
#endif
                }
                break;
            case Phonon::EffectType:
                {
                    WCHAR name[80]; // 80 is clearly stated in the MSDN doc
                    HRESULT hr = DMOGetName(m_audioEffects[index], name);
                    if (SUCCEEDED(hr)) {
                        ret["name"] = QString::fromWCharArray(name);
                    }
                }
                break;
            }
			return ret;
        }

        bool Backend::endConnectionChange(QSet<QObject *> objects)
        {
            //end of a transaction
            HRESULT hr = E_FAIL;
            if (!objects.isEmpty()) {
                foreach(QObject *o, objects) {
                    if (BackendNode *node = qobject_cast<BackendNode*>(o)) {
                        MediaObject *mo = node->mediaObject();
                        if (mo && m_graphState.contains(mo)) {
                            switch(m_graphState[mo])
                            {
                            case Phonon::ErrorState:
                            case Phonon::StoppedState:
                            case Phonon::LoadingState:
                                //nothing to do
                                break;
                            case Phonon::PausedState:
                                mo->pause();
                                break;
                            default:
                                mo->play();
                                break;
                            }
                            if (mo->state() != Phonon::ErrorState) {
                                hr = S_OK;
                            }
                            m_graphState.remove(mo);
                            break;
                        }
                    }
                }
            }
            return SUCCEEDED(hr);
        }


        bool Backend::startConnectionChange(QSet<QObject *> objects)
        {
            //start a transaction
            HRESULT hr = E_FAIL;

            //let's save the state of the graph (before we stop it)
            foreach(QObject *o, objects) {
                if (BackendNode *node = qobject_cast<BackendNode*>(o)) {
                    if (MediaObject *mo = node->mediaObject()) {
                        m_graphState[mo] = mo->state();
                        mo->ensureStopped(); //we have to stop the graph..
                        if (mo->state() != Phonon::ErrorState) {
                            hr = S_OK;
                        }
                        break;
                    }
                }
            }
            return SUCCEEDED(hr);
        }

        bool Backend::connectNodes(QObject *_source, QObject *_sink)
        {
            BackendNode *source = qobject_cast<BackendNode*>(_source);
            if (!source) {
                return false;
            }
            BackendNode *sink = qobject_cast<BackendNode*>(_sink);
            if (!sink) {
                return false;
            }

            //setting the graph if needed
            if ((source->mediaObject() && sink->mediaObject() && source->mediaObject() != sink->mediaObject())
                || (source->mediaObject() == 0 && sink->mediaObject() == 0)) {
                    //error: not in the same graph or no graph at all
                    return false;
            } else if (source->mediaObject() && source->mediaObject() != sink->mediaObject()) {
                //this' graph becomes the common one
                source->mediaObject()->grabNode(sink);
            } else if (source->mediaObject() == 0) {
                //sink's graph becomes the common one
                sink->mediaObject()->grabNode(source);
            }

            return source->mediaObject()->connectNodes(source, sink);
        }

        bool Backend::disconnectNodes(QObject *_source, QObject *_sink)
        {
            BackendNode *source = qobject_cast<BackendNode*>(_source);
            if (!source) {
                return false;
            }
            BackendNode *sink = qobject_cast<BackendNode*>(_sink);
            if (!sink) {
                return false;
            }

            return source->mediaObject() == 0 ||
                source->mediaObject()->disconnectNodes(source, sink);
        }
    }
}

QT_END_NAMESPACE

#include "moc_backend.cpp"
