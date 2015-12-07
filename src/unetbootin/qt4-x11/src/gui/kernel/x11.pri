x11 {
        contains(QT_CONFIG, nas): LIBS += -laudio -lXt
}
 
