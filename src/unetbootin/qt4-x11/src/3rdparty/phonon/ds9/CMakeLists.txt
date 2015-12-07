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
project(phonon-ds9)
include(ConfigureChecks.cmake)

if (BUILD_PHONON_DS9)
    find_package(OPENGL REQUIRED)

    include_directories($ENV{DXDSDK_DIR}/include $ENV{DXSDK_DIR})
    set(phonon_ds9_SRCS
            abstractvideorenderer.cpp
            audiooutput.cpp
            backend.cpp
            backendnode.cpp
            effect.cpp
            fakesource.cpp
            iodevicereader.cpp
            mediagraph.cpp
            mediaobject.cpp
            videowidget.cpp
            videorenderer_vmr9.cpp
            videorenderer_soft.cpp
            volumeeffect.cpp
            qbasefilter.cpp
            qpin.cpp
            qasyncreader.cpp
            qaudiocdreader.cpp
            qmeminputpin.cpp
	)

    add_definitions(-DPHONON_MAKE_QT_ONLY_BACKEND -DUNICODE)
    automoc4_add_library(phonon_ds9 SHARED ${phonon_ds9_SRCS})
    set_target_properties(phonon_ds9 PROPERTIES PREFIX "")
    target_link_libraries(phonon_ds9
                ${PHONON_LIBS}  ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY}
                dxguid strmiids dmoguids msdmo ole32 oleaut32 uuid gdi32)
    install(TARGETS phonon_ds9
	    RUNTIME DESTINATION ${BIN_INSTALL_DIR}/phonon_backend
	    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
    install(FILES ds9.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
endif (BUILD_PHONON_DS9)
