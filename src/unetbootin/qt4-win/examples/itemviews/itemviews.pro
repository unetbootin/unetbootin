TEMPLATE      = subdirs
SUBDIRS       = addressbook \
                basicsortfiltermodel \
                chart \
                coloreditorfactory \
                customsortfiltermodel \
                dirview \
                editabletreemodel \
                pixelator \
                puzzle \
                simpledommodel \
                simpletreemodel \
                simplewidgetmapper \
                spinboxdelegate \
                stardelegate

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews
INSTALLS += sources
