#include <QtGui>

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>

int main(int argv, char **args)
{
    QApplication app(argv, args);
    app.setApplicationName("Volume slider tester");

    Phonon::MediaObject *mediaObject = new Phonon::MediaObject;
    mediaObject->setCurrentSource(QString("/home/gvatteka/Music/Lumme-Badloop.ogg"));

//! [0]
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory);
    Phonon::createPath(mediaObject, audioOutput);

    Phonon::VolumeSlider *volumeSlider = new Phonon::VolumeSlider;
    volumeSlider->setAudioOutput(audioOutput);
//! [0]

    mediaObject->play();
    
    volumeSlider->show();

    return app.exec();
}

