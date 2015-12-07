//! [0]

     * media = new MediaObject(this);
     * connect(media, SIGNAL(finished()), SLOT(slotFinished());
     * media->setCurrentSource("/home/username/music/filename.ogg");
     * media->play();
     * //! [0]


//! [1]

     * media->setCurrentSource(":/sounds/startsound.ogg");
     * media->enqueue("/home/username/music/song.mp3");
     * media->enqueue(":/sounds/endsound.ogg");
     * //! [1]


//! [2]

     *   media->setCurrentSource(":/sounds/startsound.ogg");
     *   connect(media, SIGNAL(aboutToFinish()), SLOT(enqueueNextSource()));
     * }
     *
     * void enqueueNextSource()
     * {
     *   media->enqueue("/home/username/music/song.mp3");
     * }
     * //! [2]


//! [3]

         * int x = 200;
         * media->setTickInterval(x);
         * Q_ASSERT(x == producer->tickInterval());
         * //! [3]


//! [4]

         * int x = 200;
         * media->setTickInterval(x);
         * Q_ASSERT(x >= producer->tickInterval() &&
         *          x <= 2 * producer->tickInterval());
         * //! [4]


//! [5]

             *   connect(media, SIGNAL(hasVideoChanged(bool)), hasVideoChanged(bool));
             *   media->setCurrentSource("somevideo.avi");
             *   media->hasVideo(); // returns false;
             * }
             *
             * void hasVideoChanged(bool b)
             * {
             *   // b == true
             *   media->hasVideo(); // returns true;
             * }
             * //! [5]


//! [6]

             *   connect(media, SIGNAL(hasVideoChanged(bool)), hasVideoChanged(bool));
             *   media->setCurrentSource("somevideo.avi");
             *   media->hasVideo(); // returns false;
             * }
             *
             * void hasVideoChanged(bool b)
             * {
             *   // b == true
             *   media->hasVideo(); // returns true;
             * }
             * //! [6]


//! [7]

             * setMetaArtist (media->metaData("ARTIST"     ));
             * setMetaAlbum  (media->metaData("ALBUM"      ));
             * setMetaTitle  (media->metaData("TITLE"      ));
             * setMetaDate   (media->metaData("DATE"       ));
             * setMetaGenre  (media->metaData("GENRE"      ));
             * setMetaTrack  (media->metaData("TRACKNUMBER"));
             * setMetaComment(media->metaData("DESCRIPTION"));
             * //! [7]


//! [8]

             * QUrl url("http://www.example.com/music.ogg");
             * media->setCurrentSource(url);
             * //! [8]


//! [9]

             * progressBar->setRange(0, 100); // this is the default
             * connect(media, SIGNAL(bufferStatus(int)), progressBar, SLOT(setValue(int)));
             * //! [9]


