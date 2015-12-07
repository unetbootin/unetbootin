# Qt gui library, paint module

HEADERS += \
        painting/qbezier_p.h \
        painting/qbrush.h \
        painting/qcolor.h \
        painting/qcolor_p.h \
        painting/qcolormap.h \
        painting/qdrawutil.h \
        painting/qpaintdevice.h \
        painting/qpaintengine.h \
        painting/qpainter.h \
        painting/qpainter_p.h \
        painting/qpainterpath.h \
        painting/qpainterpath_p.h \
        painting/qpdf_p.h \
        painting/qpen.h \
        painting/qpolygon.h \
        painting/qpolygonclipper_p.h \
        painting/qprintengine_pdf_p.h \
        painting/qprintengine_ps_p.h \
        painting/qprinter.h \
        painting/qprinter_p.h \
        painting/qprintengine.h \
	painting/qpaintengine_preview_p.h \
        painting/qprinterinfo.h \
        painting/qregion.h \
        painting/qstroker_p.h \
        painting/qstylepainter.h \
        painting/qtessellator_p.h \
        painting/qmatrix.h \
        painting/qwmatrix.h \
        painting/qrasterizer_p.h \
        painting/qtransform.h \
        painting/qpathclipper_p.h \
	painting/qmemrotate_p.h


SOURCES += \
        painting/qbezier.cpp \
        painting/qbrush.cpp \
        painting/qcolor.cpp \
        painting/qcolor_p.cpp \
        painting/qcssutil.cpp \
        painting/qdrawutil.cpp \
        painting/qpaintengine.cpp \
        painting/qpainter.cpp \
        painting/qpainterpath.cpp \
        painting/qpdf.cpp \
        painting/qpen.cpp \
        painting/qpolygon.cpp \
        painting/qprintengine_pdf.cpp \
        painting/qprintengine_ps.cpp \
	painting/qpaintengine_preview.cpp \
        painting/qprinter.cpp \
        painting/qstroker.cpp \
        painting/qstylepainter.cpp \
        painting/qtessellator.cpp \
        painting/qregion.cpp \
        painting/qmatrix.cpp \
        painting/qrasterizer.cpp \
        painting/qtransform.cpp  \
        painting/qpathclipper.cpp \
	painting/qmemrotate.cpp

        DEFINES += QT_RASTER_IMAGEENGINE
        win32:DEFINES += QT_RASTER_PAINTENGINE
        embedded:DEFINES += QT_RASTER_PAINTENGINE
        SOURCES +=                                      \
                painting/qpaintengine_raster.cpp        \
                painting/qdrawhelper.cpp                \
                painting/qimagescale.cpp                \
                painting/qblackraster.c                 \
                painting/qgrayraster.c

        HEADERS +=                                      \
                painting/qpaintengine_raster_p.h        \
                painting/qrasterdefs_p.h                \
                painting/qgrayraster_p.h                \
                painting/qblackraster_p.h

win32 {
        HEADERS += painting/qprintengine_win_p.h

        SOURCES += \
                painting/qcolormap_win.cpp \
                painting/qpaintdevice_win.cpp \
                painting/qprintengine_win.cpp \
                painting/qprinterinfo_win.cpp \
                painting/qregion_win.cpp
        !win32-borland:!wince*:LIBS += -lmsimg32
        contains(QT_CONFIG, direct3d) {
                HEADERS += painting/qpaintengine_d3d_p.h
                SOURCES += painting/qpaintengine_d3d.cpp
                RESOURCES += painting/qpaintengine_d3d.qrc
                LIBS += -ldxguid
        }
}

wince* {
        SOURCES += painting/qregion_wince.cpp
}

unix:x11 {
        HEADERS += \
                painting/qpaintengine_x11_p.h

        SOURCES += \
                painting/qcolormap_x11.cpp \
                painting/qpaintdevice_x11.cpp \
                painting/qpaintengine_x11.cpp
}

!embedded:!x11:mac {
        HEADERS += \
                painting/qpaintengine_mac_p.h \
                painting/qprintengine_mac_p.h

        SOURCES += \
                painting/qcolormap_mac.cpp \
                painting/qpaintdevice_mac.cpp \
                painting/qpaintengine_mac.cpp \
                painting/qprintengine_mac.cpp \
                painting/qprinterinfo_mac.cpp
}

unix {
        SOURCES += \
                painting/qregion_unix.cpp
}

unix:!mac {
        HEADERS += \
                painting/qprinterinfo_unix_p.h
        SOURCES += \
                painting/qprinterinfo_unix.cpp
}

win32|x11|embedded {
        SOURCES += painting/qbackingstore.cpp
}

embedded {
        contains(QT_CONFIG,qtopia) {
                DEFINES += QTOPIA_PRINTENGINE
                HEADERS += painting/qprintengine_qws_p.h
                SOURCES += painting/qprintengine_qws.cpp
        }

        SOURCES += \
                painting/qcolormap_qws.cpp \
                painting/qpaintdevice_qws.cpp
}

x11|embedded {
        contains(QT_CONFIG,qtopia) {
            DEFINES += QT_NO_CUPS QT_NO_LPR
        } else {
            SOURCES += painting/qcups.cpp
            HEADERS += painting/qcups_p.h
        }
} else {
        DEFINES += QT_NO_CUPS QT_NO_LPR
}

mac {

} else:if(mmx|3dnow|sse|sse2|iwmmxt) {
    HEADERS += painting/qdrawhelper_x86_p.h \
               painting/qdrawhelper_mmx_p.h \
               painting/qdrawhelper_sse_p.h
    mmx {
	DEFINES += QT_HAVE_MMX
	MMX_SOURCES += painting/qdrawhelper_mmx.cpp
    }
    3dnow {
	DEFINES += QT_HAVE_3DNOW
	MMX3DNOW_SOURCES += painting/qdrawhelper_mmx3dnow.cpp
	sse {
	    SSE3DNOW_SOURCES += painting/qdrawhelper_sse3dnow.cpp
	}
    }
    sse {
	DEFINES += QT_HAVE_SSE
	SSE_SOURCES += painting/qdrawhelper_sse.cpp

        DEFINES += QT_HAVE_MMXEXT
    }
    sse2 {
	DEFINES += QT_HAVE_SSE2
	SSE2_SOURCES += painting/qdrawhelper_sse2.cpp
    }
    iwmmxt {
	DEFINES += QT_HAVE_IWMMXT
	IWMMXT_SOURCES += painting/qdrawhelper_iwmmxt.cpp
    }

    win32-g++|!win32:!*-icc* {
        mmx {
            mmx_compiler.commands = $$QMAKE_CXX -c -Winline
            mmx_compiler.commands += -mmmx
            mmx_compiler.commands += $(CXXFLAGS) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
            mmx_compiler.dependency_type = TYPE_C
            mmx_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
            mmx_compiler.input = MMX_SOURCES
            mmx_compiler.variable_out = OBJECTS
            mmx_compiler.name = compiling[mmx] ${QMAKE_FILE_IN}
            silent:mmx_compiler.commands = @echo compiling[mmx] ${QMAKE_FILE_IN} && $$mmx_compiler.commands
            QMAKE_EXTRA_COMPILERS += mmx_compiler
        }
        3dnow {
            mmx3dnow_compiler.commands = $$QMAKE_CXX -c -Winline
            mmx3dnow_compiler.commands += -m3dnow -mmmx
            mmx3dnow_compiler.commands += $(CXXFLAGS) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
            mmx3dnow_compiler.dependency_type = TYPE_C
            mmx3dnow_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
            mmx3dnow_compiler.input = MMX3DNOW_SOURCES
            mmx3dnow_compiler.variable_out = OBJECTS
            mmx3dnow_compiler.name = compiling[mmx3dnow] ${QMAKE_FILE_IN}
            silent:mmx3dnow_compiler.commands = @echo compiling[mmx3dnow] ${QMAKE_FILE_IN} && $$mmx3dnow_compiler.commands
            QMAKE_EXTRA_COMPILERS += mmx3dnow_compiler
	    sse {
                sse3dnow_compiler.commands = $$QMAKE_CXX -c -Winline
                sse3dnow_compiler.commands += -m3dnow -msse
                sse3dnow_compiler.commands += $(CXXFLAGS) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
                sse3dnow_compiler.dependency_type = TYPE_C
                sse3dnow_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
                sse3dnow_compiler.input = SSE3DNOW_SOURCES
                sse3dnow_compiler.variable_out = OBJECTS
                sse3dnow_compiler.name = compiling[sse3dnow] ${QMAKE_FILE_IN}
                silent:sse3dnow_compiler.commands = @echo compiling[sse3dnow] ${QMAKE_FILE_IN} && $$sse3dnow_compiler.commands
                QMAKE_EXTRA_COMPILERS += sse3dnow_compiler
	    }
        }
        sse {
            sse_compiler.commands = $$QMAKE_CXX -c -Winline
            sse_compiler.commands += -msse
            sse_compiler.commands += $(CXXFLAGS) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
            sse_compiler.dependency_type = TYPE_C
            sse_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
            sse_compiler.input = SSE_SOURCES
            sse_compiler.variable_out = OBJECTS
            sse_compiler.name = compiling[sse] ${QMAKE_FILE_IN}
            silent:sse_compiler.commands = @echo compiling[sse] ${QMAKE_FILE_IN} && $$sse_compiler.commands
            QMAKE_EXTRA_COMPILERS += sse_compiler
        }
        sse2 {
            sse2_compiler.commands = $$QMAKE_CXX -c -Winline
            sse2_compiler.commands += -msse2
            sse2_compiler.commands += $(CXXFLAGS) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
            sse2_compiler.dependency_type = TYPE_C
            sse2_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
            sse2_compiler.input = SSE2_SOURCES
            sse2_compiler.variable_out = OBJECTS
            sse2_compiler.name = compiling[sse2] ${QMAKE_FILE_IN}
            silent:sse2_compiler.commands = @echo compiling[sse2] ${QMAKE_FILE_IN} && $$sse2_compiler.commands
            QMAKE_EXTRA_COMPILERS += sse2_compiler
        }
        iwmmxt {
            iwmmxt_compiler.commands = $$QMAKE_CXX -c -Winline
            iwmmxt_compiler.commands += -mcpu=iwmmxt
            iwmmxt_compiler.commands += $(CXXFLAGS) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
            iwmmxt_compiler.dependency_type = TYPE_C
            iwmmxt_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
            iwmmxt_compiler.input = IWMMXT_SOURCES
            iwmmxt_compiler.variable_out = OBJECTS
            iwmmxt_compiler.name = compiling[iwmmxt] ${QMAKE_FILE_IN}
            silent:iwmmxt_compiler.commands = @echo compiling[iwmmxt] ${QMAKE_FILE_IN} && $$iwmmxt_compiler.commands
            QMAKE_EXTRA_COMPILERS += iwmmxt_compiler
        }
    } else {
        mmx: SOURCES += $$MMX_SOURCES
        3dnow: SOURCES += $$MMX3DNOW_SOURCES
        3dnow:sse: SOURCES += $$SSE3DNOW_SOURCES
        sse: SOURCES += $$SSE_SOURCES
        sse2: SOURCES += $$SSE2_SOURCES
        iwmmxt: SOURCES += $$IWMMXT_SOURCES
    }
}
win32|x11|embedded {
        HEADERS += painting/qwindowsurface_p.h \
                   painting/qwindowsurface_raster_p.h
        SOURCES += painting/qwindowsurface.cpp \
                   painting/qwindowsurface_raster.cpp

        x11 {
                HEADERS += painting/qwindowsurface_x11_p.h
                SOURCES += painting/qwindowsurface_x11.cpp
        }

        embedded {
                HEADERS += painting/qwindowsurface_qws_p.h
                SOURCES += painting/qwindowsurface_qws.cpp
        }

        win32:contains(QT_CONFIG, direct3d) {
                HEADERS += painting/qwindowsurface_d3d_p.h
                SOURCES += painting/qwindowsurface_d3d.cpp
        }
}

