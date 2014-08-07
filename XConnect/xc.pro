TEMPLATE = lib
CONFIG -= qt
CONFIG += shared xplanesdk
SOURCES = *.cpp
HEADERS = *.h

INCLUDEPATH += $$TOPSRCDIR/XConnectLib
CONFIG(release, debug|release): LIBS += -L../XConnectLib/release
CONFIG(debug, debug|release): LIBS += -L../XConnectLib/debug
LIBS += -lxclib
