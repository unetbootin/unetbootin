//! [0]

 * PushStream::PushStream(QObject *parent)
 *   : AbstractMediaStream(parent), m_timer(new QTimer(this))
 * {
 *   setStreamSize(getMediaStreamSize());
 *
 *   connect(m_timer, SIGNAL(timeout()), SLOT(moreData()));
 *   m_timer->setInterval(0);
 * }
 *
 * void PushStream::moreData()
 * {
 *   const QByteArray data = getMediaData();
 *   if (data.isEmpty()) {
 *     endOfData();
 *   } else {
 *     writeData(data);
 *   }
 * }
 *
 * void PushStream::needData()
 * {
 *   m_timer->start();
 *   moreData();
 * }
 *
 * void PushStream::enoughData()
 * {
 *   m_timer->stop();
 * }
 * //! [0]


//! [1]

 * PullStream::PullStream(QObject *parent)
 *   : AbstractMediaStream(parent)
 * {
 *   setStreamSize(getMediaStreamSize());
 * }
 *
 * void PullStream::needData()
 * {
 *   const QByteArray data = getMediaData();
 *   if (data.isEmpty()) {
 *     endOfData();
 *   } else {
 *     writeData(data);
 *   }
 * }
 * //! [1]


//! [2]

         * seekStream(0);
         * //! [2]


