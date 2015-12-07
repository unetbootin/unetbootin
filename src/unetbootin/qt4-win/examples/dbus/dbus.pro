TEMPLATE = subdirs
SUBDIRS = listnames \
	  pingpong \
	  complexpingpong \
	  chat \
	  remotecontrolledcar

# install
target.path = $$[QT_INSTALL_EXAMPLES]/dbus
sources.files = *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dbus
INSTALLS += sources
