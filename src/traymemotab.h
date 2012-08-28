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

#ifndef TRAYMEMOTAB_H
#define TRAYMEMOTAB_H

#include <QtGui>
#include <QTextEdit>

class TrayMemoTab : public QTextEdit
{
    Q_OBJECT

public:
    TrayMemoTab(const QString &fileName, QWidget *parent);
    bool isSaved() const;
    void setAsSaved();
    void initCompleted();
    QString getFileName();

private slots:
    void setAsNotSaved();

signals:
    void updateAsterisk();

private:
    bool fileSaved;
    bool initialized;
    QString fileName;
};

#endif // TRAYMEMOTAB_H
