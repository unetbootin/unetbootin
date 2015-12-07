TEMPLATE = subdirs
CONFIG += ordered

REQUIRES = !CONFIG(static,shared|static)
contains(QT_CONFIG, qt3support): SUBDIRS += widgets
win32:!contains(QT_EDITION, OpenSource):SUBDIRS += activeqt
# contains(QT_CONFIG, opengl): SUBDIRS += tools/view3d
contains(QT_CONFIG, webkit): SUBDIRS += qwebview
contains(QT_CONFIG, phonon): SUBDIRS += phononwidgets
