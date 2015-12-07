TEMPLATE = subdirs

SUBDIRS =

unix:contains(QT_CONFIG, gstreamer): SUBDIRS *= gstreamer
mac:SUBDIRS *= qt7
win32: SUBDIRS *= ds9
