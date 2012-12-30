 TEMPLATE = app

 QT += declarative

 HEADERS = model.h \
    qmlprinter.h \
    ../../qmlprinterengine.h
 SOURCES = main.cpp model.cpp \
    qmlprinter.cpp \
    ../../qmlprinterengine.cpp

OTHER_FILES += \
    qml/view.qml \
    qml/Button.qml
