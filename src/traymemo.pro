# Copyright (C) 2012 Markus Nolvi
#
# This file is part of TrayMemo.
# For more information, see <http://code.google.com/p/traymemo/>
#
# TrayMemo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# TrayMemo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TrayMemo.  If not, see <http://www.gnu.org/licenses/>.

QT       += gui

CONFIG  += qxt
QXT     += gui

TEMPLATE = app
TARGET = traymemo

#QMAKE_LFLAGS += -static-libgcc

SOURCES += main.cpp \
    traymemowindow.cpp \
    traymemotab.cpp

HEADERS += \
    traymemowindow.h \
    traymemotab.h

RESOURCES += \
    traymemo.qrc

OTHER_FILES += \
    images/traymemo.svg

#debug:DEFINES+= _DEBUG

unix:!symbian {
    LIBS += -L/usr/local/Qxt/lib/ -lQxtGui
    #INCLUDEPATH += /usr/local/Qxt/include/QxtGui/
}

win32:CONFIG(release, debug|release): {
    RC_FILE = traymemo.rc
    INCLUDEPATH += -LC:/Qxt/include
    LIBS += -LC:/Qxt/lib/libQxtCore.dll.a
    LIBS += -LC:/Qxt/lib/libQxtGui.dll.a
#    LIBS += -libQxtGui.dll.a
}
else:win32:CONFIG(debug, debug|release): {
    INCLUDEPATH += -LC:/Qxt/include
    LIBS += -LC:/Qxt/lib/libQxtCored.dll.a
    LIBS += -LC:/Qxt/lib/libQxtGuid.dll.a
}


