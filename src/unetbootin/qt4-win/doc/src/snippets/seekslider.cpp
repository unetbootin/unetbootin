#include <QtGui>

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

int main(int argv, char **args)
{
    QApplication app(argv, args);

//![0]
    Phonon::MediaObject *moo = new Phonon::MediaObject;;
    Phonon::AudioOutput *device = new Phonon::AudioOutput;
    Phonon::createPath(moo, device);
    moo->setCurrentSource(QString("/home/gvatteka/Music/Lumme-Badloop.ogg"));

    Phonon::SeekSlider *slider = new Phonon::SeekSlider;
    slider->setMediaObject(moo);
    
    slider->show();
    moo->play();
//![0]

    app.exec();
}
