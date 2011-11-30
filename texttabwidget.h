/****************************************************************************
# Copyright (C) 2011 Markus Nolvi
#
# This file is part of traymemo.
# For more information, see <http://code.google.com/p/traymemo/>
#
# traymemo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# traymemo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with traymemo.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef TEXTTABWIDGET_H
#define TEXTTABWIDGET_H

#include <QtGui>
#include <QShortcut>
#include <QTabWidget>

class TextTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TextTabWidget(const QString &fileName, QWidget *parent);

private slots:
    //void showHideWidget();

private:
    QTextEdit *textEdit;
};

#endif // TEXTTABWIDGET_H
