# Copyright (C) 2007 Trolltech ASA. All rights reserved.
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

    include_directories($ENV{DXDSDK_DIR}/include)
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

    if (MSVC80)
	    # need to set this option for MSVC 2005+ or else
	    # QString::fromWCharArray will result in an unresolved external
	    # see http://www.qtcentre.org/forum/f-installation-5/t-wchar-to-qstring-giving-error-in-vs2005-4625.html
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
    endif (MSVC80)
    
    if (MINGW)
        add_definitions("-DUNICODE")
    endif (MINGW)
	
    kde4_add_plugin(phonon_ds9 ${phonon_ds9_SRCS})
    target_link_libraries(phonon_ds9
            ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${KDE4_PHONON_LIBS} ${KDE4_KDECORE_LIBS}
	    dxguid strmiids dmoguids msdmo ole32 oleaut32 uuid gdi32)
    install(TARGETS phonon_ds9
	    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
	    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
    install(FILES ds9.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
endif (BUILD_PHONON_DS9)
