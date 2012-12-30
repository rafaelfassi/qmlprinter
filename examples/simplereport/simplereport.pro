 TEMPLATE = app

 QT += declarative

MOC_DIR = tmp
OBJECTS_DIR = tmp

 HEADERS = model.h \
    qmlprinter.h \
    ../../qmlprinterengine.h
 SOURCES = main.cpp model.cpp \
    qmlprinter.cpp \
    ../../qmlprinterengine.cpp

OTHER_FILES += \
    qml/view.qml \
    qml/Button.qml
