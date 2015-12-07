TEMPLATE    = subdirs
SUBDIRS     = calendar \
              orderform \
              syntaxhighlighter

# install
target.path = $$[QT_INSTALL_EXAMPLES]/richtext
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS richtext.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/richtext
INSTALLS += target sources
