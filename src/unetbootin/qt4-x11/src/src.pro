TEMPLATE = subdirs

# this order is important
unset(SRC_SUBDIRS)
win32:SRC_SUBDIRS += src_winmain
wince*:{
  SRC_SUBDIRS += src_corelib src_xml src_gui src_sql src_network src_script src_testlib
} else {
    SRC_SUBDIRS += src_tools_bootstrap src_tools_moc src_tools_rcc src_tools_uic src_corelib src_xml src_network src_gui src_sql src_script src_testlib
    contains(QT_CONFIG, qt3support): SRC_SUBDIRS += src_qt3support
    contains(QT_CONFIG, dbus):SRC_SUBDIRS += src_dbus
    !cross_compile {
        contains(QT_CONFIG, qt3support): SRC_SUBDIRS += src_tools_uic3
    }
}
win32:!contains(QT_EDITION, OpenSource|Console): {
    SRC_SUBDIRS += src_activeqt
    !wince*: SRC_SUBDIRS += src_tools_idc
}

contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles2): SRC_SUBDIRS += src_opengl
contains(QT_CONFIG, xmlpatterns): SRC_SUBDIRS += src_xmlpatterns
contains(QT_CONFIG, phonon): SRC_SUBDIRS += src_phonon
contains(QT_CONFIG, svg): SRC_SUBDIRS += src_svg
contains(QT_CONFIG, webkit)  {
    #exists($$QT_SOURCE_TREE/src/3rdparty/webkit/JavaScriptCore/JavaScriptCore.pro): SRC_SUBDIRS += src_javascriptcore
    SRC_SUBDIRS += src_webkit
}
contains(QT_CONFIG, scripttools): SRC_SUBDIRS += src_scripttools
SRC_SUBDIRS += src_plugins

src_winmain.subdir = $$QT_SOURCE_TREE/src/winmain
src_winmain.target = sub-winmain
src_tools_bootstrap.subdir = $$QT_SOURCE_TREE/src/tools/bootstrap
src_tools_bootstrap.target = sub-tools-bootstrap
src_tools_moc.subdir = $$QT_SOURCE_TREE/src/tools/moc
src_tools_moc.target = sub-moc
src_tools_rcc.subdir = $$QT_SOURCE_TREE/src/tools/rcc
src_tools_rcc.target = sub-rcc
src_tools_uic.subdir = $$QT_SOURCE_TREE/src/tools/uic
src_tools_uic.target = sub-uic
src_corelib.subdir = $$QT_SOURCE_TREE/src/corelib
src_corelib.target = sub-corelib
src_xml.subdir = $$QT_SOURCE_TREE/src/xml
src_xml.target = sub-xml
src_xmlpatterns.subdir = $$QT_SOURCE_TREE/src/xmlpatterns
src_xmlpatterns.target = sub-xmlpatterns
src_dbus.subdir = $$QT_SOURCE_TREE/src/dbus
src_dbus.target = sub-dbus
src_gui.subdir = $$QT_SOURCE_TREE/src/gui
src_gui.target = sub-gui
src_sql.subdir = $$QT_SOURCE_TREE/src/sql
src_sql.target = sub-sql
src_network.subdir = $$QT_SOURCE_TREE/src/network
src_network.target = sub-network
src_svg.subdir = $$QT_SOURCE_TREE/src/svg
src_svg.target = sub-svg
src_script.subdir = $$QT_SOURCE_TREE/src/script
src_script.target = sub-script
src_scripttools.subdir = $$QT_SOURCE_TREE/src/scripttools
src_scripttools.target = sub-scripttools
src_opengl.subdir = $$QT_SOURCE_TREE/src/opengl
src_opengl.target = sub-opengl
src_qt3support.subdir = $$QT_SOURCE_TREE/src/qt3support
src_qt3support.target = sub-qt3support
src_phonon.subdir = $$QT_SOURCE_TREE/src/phonon
src_phonon.target = sub-phonon
src_tools_uic3.subdir = $$QT_SOURCE_TREE/src/tools/uic3
src_tools_uic3.target = sub-uic3
src_activeqt.subdir = $$QT_SOURCE_TREE/src/activeqt
src_activeqt.target = sub-activeqt
src_tools_idc.subdir = $$QT_SOURCE_TREE/src/tools/idc
src_tools_idc.target = sub-idc
src_plugins.subdir = $$QT_SOURCE_TREE/src/plugins
src_plugins.target = sub-plugins
src_testlib.subdir = $$QT_SOURCE_TREE/src/testlib
src_testlib.target = sub-testlib
src_javascriptcore.subdir = $$QT_SOURCE_TREE/src/3rdparty/webkit/JavaScriptCore
src_javascriptcore.target = sub-javascriptcore
src_webkit.subdir = $$QT_SOURCE_TREE/src/3rdparty/webkit/WebCore
src_webkit.target = sub-webkit

#CONFIG += ordered
!wince*:!ordered {
   src_tools_moc.depends = src_tools_bootstrap
   src_tools_rcc.depends = src_tools_bootstrap
   src_tools_uic.depends = src_tools_bootstrap
   src_corelib.depends = src_tools_moc src_tools_rcc
   src_gui.depends = src_corelib src_tools_uic
   embedded: src_gui.depends += src_network
   src_xml.depends = src_corelib
   src_xmlpatterns.depends = src_corelib src_network
   src_dbus.depends = src_corelib src_xml
   src_svg.depends = src_xml src_gui
   src_script.depends = src_corelib
   src_scripttools.depends = src_script src_gui src_network
   src_network.depends = src_corelib
   src_opengl.depends = src_gui
   src_sql.depends = src_corelib
   src_testlib.depends = src_corelib
   src_qt3support.depends = src_gui src_xml src_network src_sql
   src_phonon.depends = src_gui
   src_tools_uic3.depends = src_qt3support src_xml
   src_tools_idc.depends = src_corelib
   src_tools_activeqt.depends = src_tools_idc src_gui
   src_plugins.depends = src_gui src_sql src_svg
   contains(QT_CONFIG, webkit)  {
      src_webkit.depends = src_gui src_sql src_network src_xml
      #exists($$QT_SOURCE_TREE/src/3rdparty/webkit/JavaScriptCore/JavaScriptCore.pro): src_webkit.depends += src_javascriptcore
   }
   contains(QT_CONFIG, qt3support): src_plugins.depends += src_qt3support
   contains(QT_CONFIG, dbus):{
      src_plugins.depends += src_dbus
      src_phonon.depends +=  src_dbus
   }
}

# This creates a sub-src rule
sub_src_target.CONFIG = recursive
sub_src_target.recurse = $$SRC_SUBDIRS
sub_src_target.target = sub-src
sub_src_target.recurse_target =
QMAKE_EXTRA_TARGETS += sub_src_target

# This gives us a top level debug/release
EXTRA_DEBUG_TARGETS =
EXTRA_RELEASE_TARGETS =
for(subname, SRC_SUBDIRS) {
   subdir = $$subname
   !isEmpty($${subname}.subdir):subdir = $$eval($${subname}.subdir)
   subpro = $$subdir/$${basename(subdir)}.pro
   !exists($$subpro):next()
   subtarget = $$replace(subdir, [^A-Za-z0-9], _)
   reg_src = $$replace(QT_SOURCE_TREE, \\\\, \\\\)
   subdir = $$replace(subdir, $$reg_src, $$QT_BUILD_TREE)
   subdir = $$replace(subdir, /, $$QMAKE_DIR_SEP)
   subdir = $$replace(subdir, \\\\, $$QMAKE_DIR_SEP)
   SUB_TEMPLATE = $$list($$fromfile($$subpro, TEMPLATE))
   !isEqual(subname, src_tools_bootstrap):if(isEqual($$SUB_TEMPLATE, lib) | isEqual($$SUB_TEMPLATE, subdirs) | isEqual(subname, src_tools_idc) | isEqual(subname, src_tools_uic3)):!separate_debug_info {
       #debug
       eval(debug-$${subtarget}.depends = $${subdir}\$${QMAKE_DIR_SEP}$(MAKEFILE) $$EXTRA_DEBUG_TARGETS)
       eval(debug-$${subtarget}.commands = (cd $$subdir && $(MAKE) -f $(MAKEFILE) debug))
       EXTRA_DEBUG_TARGETS += debug-$${subtarget}
       QMAKE_EXTRA_TARGETS += debug-$${subtarget}
       #release
       eval(release-$${subtarget}.depends = $${subdir}\$${QMAKE_DIR_SEP}$(MAKEFILE) $$EXTRA_RELEASE_TARGETS)
       eval(release-$${subtarget}.commands = (cd $$subdir && $(MAKE) -f $(MAKEFILE) release))
       EXTRA_RELEASE_TARGETS += release-$${subtarget}
       QMAKE_EXTRA_TARGETS += release-$${subtarget}
    } else { #do not have a real debug target/release
       #debug
       eval(debug-$${subtarget}.depends = $${subdir}\$${QMAKE_DIR_SEP}$(MAKEFILE) $$EXTRA_DEBUG_TARGETS)
       eval(debug-$${subtarget}.commands = (cd $$subdir && $(MAKE) -f $(MAKEFILE) first))
       EXTRA_DEBUG_TARGETS += debug-$${subtarget}
       QMAKE_EXTRA_TARGETS += debug-$${subtarget}
       #release
       eval(release-$${subtarget}.depends = $${subdir}\$${QMAKE_DIR_SEP}$(MAKEFILE) $$EXTRA_RELEASE_TARGETS)
       eval(release-$${subtarget}.commands = (cd $$subdir && $(MAKE) -f $(MAKEFILE) first))
       EXTRA_RELEASE_TARGETS += release-$${subtarget}
       QMAKE_EXTRA_TARGETS += release-$${subtarget}
   }
}
debug.depends = $$EXTRA_DEBUG_TARGETS
release.depends = $$EXTRA_RELEASE_TARGETS
QMAKE_EXTRA_TARGETS += debug release

SUBDIRS += $$SRC_SUBDIRS

