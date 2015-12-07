#include <QtGui>

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/backendcapabilities.h>
#include <phonon/effect.h>
#include <phonon/effectwidget.h>


int main(int argv, char **args)
{
    QApplication app(argv, args);
    app.setApplicationName("Audio effect tester");

    Phonon::MediaObject *mediaObject = new Phonon::MediaObject;
    mediaObject->setCurrentSource(QString("/home/gvatteka/Music/Lumme-Badloop.ogg"));

    Phonon::AudioOutput *audioOutput =
        new Phonon::AudioOutput(Phonon::MusicCategory);

//! [0]
    QList<Phonon::EffectDescription> effectDescriptions =
            Phonon::BackendCapabilities::availableAudioEffects();
    Phonon::EffectDescription effectDescription = effectDescriptions.at(4);
    
    Phonon::Path path = Phonon::createPath(mediaObject, audioOutput);

//! [1]
    Phonon::Effect *effect = new Phonon::Effect(effectDescription);
    path.insertEffect(effect);
//! [0]

    Phonon::EffectWidget *effectWidget = new Phonon::EffectWidget(effect);
    effectWidget->show();
//! [1]

    mediaObject->play();
    
    effectWidget->setWindowTitle("Effect Name: " + effectDescription.name());

    app.exec();
}
