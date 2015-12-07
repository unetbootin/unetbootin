TEMPLATE = subdirs
!wince*: SUBDIRS += updater
wince*: contains(QT_CONFIG, cetest): SUBDIRS += wince
CONFIG += ordered
