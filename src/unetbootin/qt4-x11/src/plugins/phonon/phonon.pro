TEMPLATE = subdirs

SUBDIRS =

unix:contains(QT_CONFIG, gstreamer): SUBDIRS *= gstreamer
mac:contains(QT_CONFIG, phonon-backend): SUBDIRS *= qt7
win32:!wince*:contains(QT_CONFIG, phonon-backend): SUBDIRS *= ds9
wince*:contains(QT_CONFIG, phonon-backend): SUBDIRS *= waveout
wince*:contains(QT_CONFIG, directshow): SUBDIRS *= ds9
