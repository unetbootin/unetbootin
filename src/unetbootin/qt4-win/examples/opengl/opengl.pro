TEMPLATE      = subdirs
!wince*: {
SUBDIRS       = 2dpainting \
                grabber \
                hellogl \
                overpainting \
                pbuffers \
                pbuffers2 \
		framebufferobject \
		framebufferobject2 \
                samplebuffers \
                textures
}
wince*: SUBDIRS += hellogl_es


# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS opengl.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl
INSTALLS += target sources
