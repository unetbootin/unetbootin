FIND_PACKAGE(Alsa)
ALSA_CONFIGURE_FILE(${CMAKE_CURRENT_BINARY_DIR}/config-alsa.h)

OPTION(KDE4_DISABLE_MULTIMEDIA "Disable multimedia support (default: off)" OFF)
IF (KDE4_DISABLE_MULTIMEDIA)
  MESSAGE(STATUS "Phonon multimedia support disabled by user.")
  SET(HAVE_KDE4_MULTIMEDIA 0)
ELSE (KDE4_DISABLE_MULTIMEDIA)
  MESSAGE(STATUS "Phonon multimedia support is enabled.")
  SET(HAVE_KDE4_MULTIMEDIA 1)
ENDIF (KDE4_DISABLE_MULTIMEDIA)

configure_file(config-phonon.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-phonon.h )

# Those apply to all subdirs
include_directories(${KDE4_KDECORE_INCLUDES} ${CMAKE_BINARY_DIR}/solid)

add_subdirectory( tests )
add_subdirectory(examples)
add_subdirectory(experimental)
add_subdirectory(platform_kde)
add_subdirectory(libkaudiodevicelist)
if(NOT MINGW)
  # ICE in outputdevicechoice.cpp:262
  add_subdirectory( kcm )
endif(NOT MINGW)
########### next target ###############

set(phonon_LIB_SRCS
   objectdescription.cpp
   objectdescriptionmodel.cpp
   phononnamespace.cpp
   mediasource.cpp
   abstractmediastream.cpp
   streaminterface.cpp
   mediaobject.cpp
   medianode.cpp
   path.cpp
   effectparameter.cpp
   effect.cpp
   volumefadereffect.cpp
   audiooutputadaptor.cpp
   abstractaudiooutput.cpp
   abstractaudiooutput_p.cpp
   audiooutput.cpp
   abstractvideooutput.cpp
   abstractvideooutput_p.cpp
   backendcapabilities.cpp
   globalconfig.cpp
   factory.cpp
   platform.cpp
   mediacontroller.cpp
   videowidget.cpp
   videoplayer.cpp
   seekslider.cpp
   volumeslider.cpp
   effectwidget.cpp
   iodevicestream.cpp
   )

kde4_add_library(phonon SHARED ${phonon_LIB_SRCS})
target_link_libraries(phonon ${QT_QTDBUS_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#do not use GENERIC versioning in phonon
set_target_properties(phonon PROPERTIES VERSION 4.2.0 SOVERSION 4)
install(TARGETS phonon ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES
   phonon_export.h
   objectdescription.h
   objectdescriptionmodel.h
   phononnamespace.h
   mediasource.h
   abstractmediastream.h
   streaminterface.h
   mediaobject.h
   audiooutput.h
   medianode.h
   path.h
   effectparameter.h
   effect.h
   effectinterface.h
   volumefadereffect.h
   volumefaderinterface.h
   abstractaudiooutput.h
   abstractvideooutput.h
   backendcapabilities.h
   phonondefs.h
   backendinterface.h
   mediaobjectinterface.h
   audiooutputinterface.h
   addoninterface.h
   mediacontroller.h
   videowidget.h
   videowidgetinterface.h
   videoplayer.h
   seekslider.h
   volumeslider.h
   effectwidget.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel)

install( FILES  phononbackend.desktop           DESTINATION  ${SERVICETYPES_INSTALL_DIR} )
install( FILES  org.kde.Phonon.AudioOutput.xml  DESTINATION  ${DBUS_INTERFACES_INSTALL_DIR} )
