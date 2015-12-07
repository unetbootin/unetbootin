TEMPLATE = subdirs

SUBDIRS	*= accessible imageformats sqldrivers iconengines script
unix {
        contains(QT_CONFIG,iconv)|contains(QT_CONFIG,gnu-libiconv):SUBDIRS *= codecs
} else {
        SUBDIRS *= codecs
}
!embedded:SUBDIRS *= graphicssystems
embedded:SUBDIRS *=  gfxdrivers decorations mousedrivers kbddrivers
!win32:!embedded:!mac:SUBDIRS *= inputmethods
contains(QT_CONFIG, phonon): SUBDIRS *= phonon
