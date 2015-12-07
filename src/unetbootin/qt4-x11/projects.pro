#####################################################################
# Main projectfile
#####################################################################

CONFIG += ordered
TEMPLATE = subdirs

cross_compile: CONFIG += nostrip

isEmpty(QT_BUILD_PARTS) { #defaults
   QT_BUILD_PARTS = libs tools examples demos docs translations
} else { #make sure the order makes sense
   contains(QT_BUILD_PARTS, tools) {
       QT_BUILD_PARTS -= tools
       QT_BUILD_PARTS = tools $$QT_BUILD_PARTS
   }
   contains(QT_BUILD_PARTS, libs) {
       QT_BUILD_PARTS -= libs
       QT_BUILD_PARTS = libs $$QT_BUILD_PARTS
   }
   contains(QT_BUILD_PARTS, qmake) {
       QT_BUILD_PARTS -= qmake
       QT_BUILD_PARTS = qmake $$QT_BUILD_PARTS
   }
}

#process the projects
for(PROJECT, $$list($$lower($$unique(QT_BUILD_PARTS)))) {
    isEqual(PROJECT, tools) {
       SUBDIRS += tools
    } else:isEqual(PROJECT, examples) {
       SUBDIRS += examples
    } else:isEqual(PROJECT, demos) {
       SUBDIRS += demos
    } else:isEqual(PROJECT, libs) {
       include(src/src.pro)
    } else:isEqual(PROJECT, docs) {
       contains(QT_BUILD_PARTS, tools):include(doc/doc.pri)
    } else:isEqual(PROJECT, translations) {
       contains(QT_BUILD_PARTS, tools):include(translations/translations.pri)
    } else:isEqual(PROJECT, qmake) {
#      SUBDIRS += qmake
    } else {
       message(Unknown PROJECT: $$PROJECT)
    }
}

confclean.depends += clean
confclean.commands =
unix {
  confclean.commands += (cd config.tests/unix/stl && $(MAKE) distclean); \
			(cd config.tests/unix/endian && $(MAKE) distclean); \
			(cd config.tests/unix/ipv6 && $(MAKE) distclean); \
			(cd config.tests/unix/largefile && $(MAKE) distclean); \
			(cd config.tests/unix/ptrsize && $(MAKE) distclean); \
			(cd config.tests/x11/notype && $(MAKE) distclean); \
			(cd config.tests/unix/getaddrinfo && $(MAKE) distclean); \
			(cd config.tests/unix/cups && $(MAKE) distclean); \
			(cd config.tests/unix/psql && $(MAKE) distclean); \
			(cd config.tests/unix/mysql && $(MAKE) distclean); \
 	 		(cd config.tests/unix/mysql_r && $(MAKE) distclean); \
			(cd config.tests/unix/nis && $(MAKE) distclean); \
			(cd config.tests/unix/nix && $(MAKE) distclean); \
			(cd config.tests/unix/odbc && $(MAKE) distclean); \
			(cd config.tests/unix/oci && $(MAKE) distclean); \
			(cd config.tests/unix/tds && $(MAKE) distclean); \
			(cd config.tests/unix/db2 && $(MAKE) distclean); \
			(cd config.tests/unix/ibase && $(MAKE) distclean); \
			(cd config.tests/unix/ipv6ifname && $(MAKE) distclean); \
			(cd config.tests/unix/zlib && $(MAKE) distclean); \
			(cd config.tests/unix/libmng && $(MAKE) distclean); \
			(cd config.tests/unix/sqlite2 && $(MAKE) distclean); \
			(cd config.tests/unix/libjpeg && $(MAKE) distclean); \
			(cd config.tests/unix/libpng && $(MAKE) distclean); \
			(cd config.tests/x11/xcursor && $(MAKE) distclean); \
			(cd config.tests/x11/xrender && $(MAKE) distclean); \
			(cd config.tests/x11/xrandr && $(MAKE) distclean); \
			(cd config.tests/x11/xkb && $(MAKE) distclean); \
			(cd config.tests/x11/xinput && $(MAKE) distclean); \
			(cd config.tests/x11/fontconfig && $(MAKE) distclean); \
			(cd config.tests/x11/xinerama && $(MAKE) distclean); \
			(cd config.tests/x11/sm && $(MAKE) distclean); \
			(cd config.tests/x11/xshape && $(MAKE) distclean); \
			(cd config.tests/x11/opengl && $(MAKE) distclean); \
                        $(DEL_FILE) config.tests/.qmake.cache; \
			$(DEL_FILE) src/core/global/qconfig.h; \
			$(DEL_FILE) src/core/global/qconfig.cpp; \
			$(DEL_FILE) mkspecs/qconfig.pri; \
			$(DEL_FILE) .qmake.cache; \
 			(cd qmake && $(MAKE) distclean);
}
win32 {
  confclean.commands += -$(DEL_FILE) src\core\global\qconfig.h $$escape_expand(\n\t) \
			-$(DEL_FILE) src\core\global\qconfig.cpp $$escape_expand(\n\t) \
			-$(DEL_FILE) mkspecs\qconfig.pri $$escape_expand(\n\t) \
			-$(DEL_FILE) .qmake.cache $$escape_expand(\n\t) \
			(cd qmake && $(MAKE) distclean)
}
QMAKE_EXTRA_TARGETS += confclean
qmakeclean.commands += (cd qmake && $(MAKE) clean)
QMAKE_EXTRA_TARGETS += qmakeclean
CLEAN_DEPS += qmakeclean

CONFIG -= qt

### installations ####

#translations
translations.path=$$[QT_INSTALL_TRANSLATIONS]
translations.files = $$QT_SOURCE_TREE/translations/*.qm
INSTALLS += translations

#qmake
qmake.path=$$[QT_INSTALL_BINS]
win32 {
   qmake.files=$$QT_BUILD_TREE/bin/qmake.exe
} else {
   qmake.files=$$QT_BUILD_TREE/bin/qmake
}
INSTALLS += qmake

#mkspecs
mkspecs.path=$$[QT_INSTALL_DATA]/mkspecs
mkspecs.files=$$QT_BUILD_TREE/mkspecs/qconfig.pri $$QT_SOURCE_TREE/mkspecs/*
unix { 
   DEFAULT_QMAKESPEC = $$QMAKESPEC
   DEFAULT_QMAKESPEC ~= s,^.*mkspecs/,,g
   mkspecs.commands += $(DEL_FILE) $(INSTALL_ROOT)$$mkspecs.path/default; $(SYMLINK) $$DEFAULT_QMAKESPEC $(INSTALL_ROOT)$$mkspecs.path/default
}
INSTALLS += mkspecs

false:macx { #mac install location
    macdocs.files = $$htmldocs.files
    macdocs.path = /Developer/Documentation/Qt
    INSTALLS += macdocs
}

!win32:contains(QT_CONFIG, qtusagereporter) {
    usagereporter.path=$$[QT_INSTALL_BINS]
    usagereporter.files=$$QT_BUILD_TREE/bin/qtusagereporter
    INSTALLS += usagereporter
}
