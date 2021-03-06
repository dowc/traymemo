/****************************************************************************
# Copyright (C) 2011 Markus Nolvi
#
# This file is part of TrayMemo.
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
# along with TrayMemo.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include <QtGui>

#include "traymemotab.h"

TrayMemoTab::TrayMemoTab(const QString &fileName, QWidget *parent = 0)
    :QTextEdit(parent), fileSaved(true), initialized(false), fileName(fileName)
{
    QObject::connect(this->document(), SIGNAL(contentsChanged()), this, SLOT(setAsNotSaved()));
    QObject::connect(this, SIGNAL(updateAsterisk()), parent, SLOT(updateAsterisk()));
    //setAcceptRichText(false);
}

void TrayMemoTab::setAsNotSaved()
{
    if (!initialized || !this->document()->isModified())
        return;

    fileSaved = false;
    emit updateAsterisk();
}

bool TrayMemoTab::isSaved() const
{
    return fileSaved;
}

void TrayMemoTab::setAsSaved()
{
    fileSaved = true;
    emit updateAsterisk();
}

void TrayMemoTab::initCompleted()
{
    initialized = true;
}

QString TrayMemoTab::getFileName()
{
    return fileName;
}
