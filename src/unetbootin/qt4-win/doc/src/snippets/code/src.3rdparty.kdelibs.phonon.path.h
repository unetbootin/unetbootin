//! [0]

MediaObject *media = new MediaObject;
AudioOutput *output = new AudioOutput(Phonon::MusicCategory);
Path path = Phonon::createPath(media, output);
Q_ASSERT(path.isValid()); // for this simple case the path should always be
                          //valid - there are unit tests to ensure it
// insert an effect
QList<EffectDescription> effectList = BackendCapabilities::availableAudioEffects();
if (!effectList.isEmpty()) {
    Effect *effect = path.insertEffect(effectList.first());
}
 * //! [0]


