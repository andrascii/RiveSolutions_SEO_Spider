include(../buildscripts/buildscripts.pri)
include(../compilercompat/compilercompat.pri)
include(../qtcompat/qtcompat.pri)

include($$COMPILERCOMPAT/defines.pri)
include($$QTCOMPAT/defines.pri)

TEMPLATE = lib
TARGET = qxtsmtp
CONFIG -= flat
CONFIG += static
QT -= gui
QT += network
DEFINES += QXT_STATIC BUILD_QXT_CORE BUILD_QXT_NETWORK
DEFINES -= QT_NO_CAST_FROM_ASCII

# treat warnings as errors
QMAKE_CXXFLAGS += /WX

CONFIG(release, debug|release) {
	DEFINES += NDEBUG
	QMAKE_LFLAGS += /DEBUG
	QMAKE_CXXFLAGS += -Zi -Ob2
}

CONFIG(debug, debug|release) {
	DEFINES += DEBUG
	QMAKE_LFLAGS += /INCREMENTAL
}

INCLUDEPATH += \
	$$COMPILERCOMPAT \
	$$PWD/include \


include(qxtsmtp.pri)

QMAKE_LIBDIR += $$DESTDIR
