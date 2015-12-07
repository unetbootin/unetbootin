TEMPLATE      = subdirs
SUBDIRS       = blockingfortuneclient \
 		broadcastreceiver \
                broadcastsender \
                chat \
		download \
		downloadmanager \
                fortuneclient \
                fortuneserver \
                ftp \
                http \
                loopback \
                threadedfortuneserver \
 		torrent

contains(QT_CONFIG, openssl):SUBDIRS += securesocketclient

# install
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS network.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/network
INSTALLS += sources
