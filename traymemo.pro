# Copyright (C) 2011 Markus Nolvi
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

INCLUDEPATH += . /usr/local/Qxt/include/QxtCore/ \
                 /usr/local/Qxt/include/QxtGui/

CONFIG  += qxt
QXT     += gui


TEMPLATE = app
TARGET = traymemo

SOURCES += main.cpp \
    traymemowindow.cpp \
    texttabwidget.cpp

HEADERS += \
    traymemowindow.h \
    texttabwidget.h


# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()
RESOURCES += \
    traymemo.qrc

OTHER_FILES += \
    images/traymemo.svg \
    qml/main.qml

#unix:!symbian {
#    #LIBS += /usr/local/Qxt/lib/libQxtCore.so
#    LIBS += /usr/local/Qxt/lib/libQxtGui.so
#    #LIBS += /usr/lib/libqxt.so
#    #LIBS += /usr/local/Qxt/lib/libQxt.so
#}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Qxt/lib/release/ -lQxtGui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Qxt/lib/debug/ -lQxtGui
else:symbian: LIBS += -lQxtGui
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Qxt/lib/ -lQxtGui

INCLUDEPATH += $$PWD/../../../../../usr/local/Qxt/include
DEPENDPATH += $$PWD/../../../../../usr/local/Qxt/include


