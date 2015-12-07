//! [0]

 * VideoPlayer *player = new VideoPlayer(Phonon::VideoCategory, parentWidget);
 * connect(player, SIGNAL(finished()), player, SLOT(deleteLater()));
 * player->play(url);
 * //! [0]


//! [1]

         * audioPlayer->load(url);
         * audioPlayer->play();
         * //! [1]


