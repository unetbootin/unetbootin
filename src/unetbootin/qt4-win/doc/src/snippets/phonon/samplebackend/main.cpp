//! [snippet]
QObject *Backend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &args)
{
    switch (c) {
    case MediaObjectClass:
        return new MediaObject(parent);
    case VolumeFaderEffectClass:
        return new VolumeFaderEffect(parent);
    case AudioOutputClass:
        return new AudioOutput(parent);
    case AudioDataOutputClass:
        return new AudioDataOutput(parent);
    case VisualizationClass:
        return new Visualization(parent);
    case VideoDataOutputClass:
        return new VideoDataOutput(parent);
    case EffectClass:
        return new Effect(args[0].toInt(), parent);
    case VideoWidgetClass:
        return new VideoWidget(qobject_cast<QWidget *>(parent));
    }
    return 0;
}

QSet<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const
{
    QSet<int> set;
    switch(type)
    {
    case Phonon::AudioOutputDeviceType:
        // use AudioDeviceEnumerator to list ALSA and OSS devices
        set << 10000 << 10001;
        break;
    case Phonon::AudioCaptureDeviceType:
        set << 20000 << 20001;
        break;
    case Phonon::VideoOutputDeviceType:
        break;
    case Phonon::VideoCaptureDeviceType:
        set << 30000 << 30001;
        break;
    case Phonon::VisualizationType:
    case Phonon::AudioCodecType:
    case Phonon::VideoCodecType:
    case Phonon::ContainerFormatType:
        break;
    case Phonon::EffectType:
        set << 0x7F000001;
        break;
    }
    return set;
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const
{
    QHash<QByteArray, QVariant> ret;
    switch (type) {
    case Phonon::AudioOutputDeviceType:
        switch (index) {
        case 10000:
            ret.insert("name", QLatin1String("internal Soundcard"));
            break;
        case 10001:
            ret.insert("name", QLatin1String("USB Headset"));
            ret.insert("icon", KIcon("usb-headset"));
            ret.insert("available", false);
            break;
        }
        break;
    case Phonon::AudioCaptureDeviceType:
        switch (index) {
        case 20000:
            ret.insert("name", QLatin1String("Soundcard"));
            ret.insert("description", QLatin1String("first description"));
            break;
        case 20001:
            ret.insert("name", QLatin1String("DV"));
            ret.insert("description", QLatin1String("second description"));
            break;
        }
        break;
    case Phonon::VideoOutputDeviceType:
        break;
    case Phonon::VideoCaptureDeviceType:
        switch (index) {
        case 30000:
            ret.insert("name", QLatin1String("USB Webcam"));
            ret.insert("description", QLatin1String("first description"));
            break;
        case 30001:
            ret.insert("name", QLatin1String("DV"));
            ret.insert("description", QLatin1String("second description"));
            break;
        }
        break;
    case Phonon::VisualizationType:
        break;
    case Phonon::AudioCodecType:
        break;
    case Phonon::VideoCodecType:
        break;
    case Phonon::ContainerFormatType:
        break;
    case Phonon::EffectType:
        switch (index) {
        case 0x7F000001:
            ret.insert("name", QLatin1String("Delay"));
            ret.insert("description", QLatin1String("Simple delay effect with time, feedback and level controls."));
            break;
        }
        break;
    }
    return ret;
}
//! [snippet]
