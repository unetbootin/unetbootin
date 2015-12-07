#include <QtGui>

#include <phonon/backendcapabilities.h>
#include <phonon/phononnamespace.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>
#include <phonon/effectwidget.h>


int main(int argv, char **args)
{
    QApplication app(argv, args);
    app.setApplicationName("effectsnippets");

    QList<Phonon::EffectDescription> effects =
        Phonon::BackendCapabilities::availableAudioEffects();

    Phonon::Effect *effect = new Phonon::Effect(effects.at(3));
    
//![0]
    QList<Phonon::EffectParameter> parameters = effect->parameters();

    foreach(Phonon::EffectParameter parameter, parameters) {
        // Do something with parameter
    }
//![0]

//![1]
    Phonon::EffectWidget *effectWidget = new Phonon::EffectWidget(effect);
//![1]
    
    effectWidget->show();

    return app.exec();
}
