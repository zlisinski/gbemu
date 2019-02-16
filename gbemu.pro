######################################################################
# Automatically generated by qmake (3.1) Sat Feb 2 13:41:51 2019
######################################################################

QT += core gui gamepad
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = gbemu
INCLUDEPATH += .

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -Wpedantic
CONFIG += debug

# Input
HEADERS += \
	src/AbsByteProxy.h \
	src/AbsFrameHandler.h \
	src/Buttons.h \
	src/Cpu.h \
	src/Display.h \
	src/EmulatorMgr.h \
	src/Exceptions.h \
	src/gbemu.h \
	src/Globals.h \
	src/Input.h \
	src/Interrupt.h \
	src/MemoryBankController.h \
	src/MemoryByteObserver.h \
	src/MemoryByteProxy.h \
	src/MemoryByteSubject.h \
	src/Memory.h \
	src/RegisterByteProxy.h \
	src/Serial.h \
	src/Timer.h \
	src/TimerObserver.h \
	src/TimerSubject.h \
	src/Utils.h \
	src/ui/DebugWindow.h \
	src/ui/MainWindow.h \
	src/ui/QtFrameHandler.h

SOURCES += \
	src/Buttons.cpp \
	src/Cpu.cpp \
	src/Display.cpp \
	src/EmulatorMgr.cpp \
	src/Input.cpp \
	src/Interrupt.cpp \
	src/MemoryBankController.cpp \
	src/Memory.cpp \
	src/main.cpp \
	src/Serial.cpp \
	src/Timer.cpp \
	src/Utils.cpp \
	src/ui/DebugWindow.cpp \
	src/ui/MainWindow.cpp

FORMS += \
	src/ui/DebugWindow.ui


