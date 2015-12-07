# Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
project(phonon-qt7)
include(ConfigureChecks.cmake)

if (BUILD_PHONON_QT7)
    set(phonon_qt7_SRCS
        medianode.cpp
        backend.cpp
        videowidget.cpp
        mediaobject.cpp
        quicktimevideoplayer.cpp
	displaylinkcallback.cpp
        error.cpp
	medianodevideopart.cpp
        medianodeevent.cpp
        quicktimeaudioplayer.cpp
	audionode.cpp
        audiograph.cpp
        audiooutput.cpp
        quicktimemetadata.cpp
   	audiomixer.cpp
        audiodevice.cpp
        backendinfo.cpp
        audioconnection.cpp
   	videoframe.cpp
        audiosplitter.cpp
        mediaobjectaudionode.cpp
       )

   kde4_add_plugin(phonon_qt7 ${phonon_qt7_SRCS})
   target_link_libraries(phonon_qt7
       ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY}
       ${KDE4_PHONON_LIBS}
       "-framework QuickTime"
       "-framework OpenGL"
       "-framework AudioUnit"
       "-framework AudioToolbox"
       "-framework CoreAudio"
       "-framework Quartz"
#      "-framework AGL"
       )
    install(TARGETS phonon_qt7 DESTINATION ${PLUGIN_INSTALL_DIR})
    #install(FILES qt7.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)

endif (BUILD_PHONON_QT7)
