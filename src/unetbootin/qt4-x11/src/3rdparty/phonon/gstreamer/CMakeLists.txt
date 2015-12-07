# Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
# Copyright (C) 2008 Matthias Kretz <kretz@kde.org>
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2 or 3 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library.  If not, see <http://www.gnu.org/licenses/>.

project(phonon-gstreamer)
include(ConfigureChecks.cmake)

if (BUILD_PHONON_GSTREAMER)
   include_directories(${GSTREAMER_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR}
      ${LIBXML2_INCLUDE_DIR})

   set(phonon_gstreamer_SRCS
      audiooutput.cpp
      artssink.cpp
      backend.cpp
      devicemanager.cpp
      effectmanager.cpp
      gsthelper.cpp
      mediaobject.cpp
      medianode.cpp
      effect.cpp
      medianodeevent.cpp
      videowidget.cpp
      qwidgetvideosink.cpp
      streamreader.cpp
      phononsrc.cpp
      message.cpp
      audioeffect.cpp
      abstractrenderer.cpp
      x11renderer.cpp
      widgetrenderer.cpp
      glrenderer.cpp
      volumefadereffect.cpp
      )

   find_package(Alsa)
   if(ALSA_FOUND)
      add_definitions(-DUSE_ALSASINK2)
      include_directories(${ALSA_INCLUDES})
      set(phonon_gstreamer_SRCS
         ${phonon_gstreamer_SRCS}
         alsasink2.c
         )
   endif(ALSA_FOUND)

   automoc4(phonon_gstreamer phonon_gstreamer_SRCS)
   add_library(phonon_gstreamer SHARED ${phonon_gstreamer_SRCS})
   set_target_properties(phonon_gstreamer PROPERTIES PREFIX "")
   target_link_libraries(phonon_gstreamer
      ${QT_QTOPENGL_LIBRARY}
      ${PHONON_LIBS} ${OPENGL_gl_LIBRARY}
      ${GSTREAMER_LIBRARIES} ${GSTREAMER_BASE_LIBRARY} ${GSTREAMER_INTERFACE_LIBRARY}
      ${GSTREAMER_PLUGIN_VIDEO_LIBRARIES} ${GSTREAMER_PLUGIN_AUDIO_LIBRARIES}
      ${GLIB2_LIBRARIES} ${GOBJECT_LIBRARIES})
   if(ALSA_FOUND)
      target_link_libraries(phonon_gstreamer ${ASOUND_LIBRARY})
   endif(ALSA_FOUND)

   install(TARGETS phonon_gstreamer DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
   install(FILES gstreamer.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
endif (BUILD_PHONON_GSTREAMER)
