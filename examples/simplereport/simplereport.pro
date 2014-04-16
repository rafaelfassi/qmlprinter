 TEMPLATE = app

 QT += declarative \
     printsupport

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

copyqml.commands = $(COPY_DIR) $$PWD/qml $$OUT_PWD
copyimages.commands = $(COPY_DIR) $$PWD/images $$OUT_PWD
first.depends = $(first) copyqml copyimages
export(first.depends)
export(copyqml.commands)
export(copyimages.commands)
QMAKE_EXTRA_TARGETS += first copyqml copyimages
