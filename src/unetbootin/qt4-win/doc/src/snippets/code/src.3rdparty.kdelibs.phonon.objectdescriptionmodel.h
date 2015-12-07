//! [0]

     * QComboBox *cb = new QComboBox(parentWidget);
     * ObjectDescriptionModel *model = new ObjectDescriptionModel(cb);
     * model->setModelData(BackendCapabilities::availableAudioOutputDevices());
     * cb->setModel(model);
     * cb->setCurrentIndex(0); // select first entry
     * //! [0]


//! [1]

     * int cbIndex = cb->currentIndex();
     * AudioOutputDevice selectedDevice = model->modelData(cbIndex);
     * //! [1]


