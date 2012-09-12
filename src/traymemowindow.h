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

#ifndef TRAYMEMOWINDOW_H
#define TRAYMEMOWINDOW_H

#include <QtGui>
#include <QSystemTrayIcon>
#include <QDialog>
#include <QShortcut>
#include <QxtGlobalShortcut>
#include <QtDeclarative/QDeclarativeView>
#include <QTabWidget>
#include <traymemotab.h>

class TrayMemoWindow : public QDialog
{
    Q_OBJECT

public:
    TrayMemoWindow();
    ~TrayMemoWindow();

private slots:    
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showAboutMessage();
    void showHideWidget();
    void openFileSaveDialog();
    void openFileOpenDialog();
    void saveTextToFile();    
    void closeCurrentTab();
    void closeApplication();
    void changeCurrentTab(int index);
    void showCurrentShortcuts();
    void moveToNextTab();
    void moveToPreviousTab();
    void showChangeDialog();
    void updateAsterisk();

private:
    void readTextFromFile(QFile &file);
    void createTrayIcon();
    void createNewTab(QString fileName);
    void createNewFile(QString fileName);
    void openFile(QString fileName);
    QString stripPathFromFileName(QString fileName);
    bool showUnsavedDialog(QString fileName);
    bool anyUnsavedDocuments();
    void setCurrentWindowTitle(QString fileName);
    void assignShowHideShortCut(const QString value);

    QTabWidget *tabWidget;
    int proposedFileNameNumbers;
    QFile *currentFile;    
    TrayMemoTab *currentTextEdit;
    QxtGlobalShortcut *shortCutShowHide;
    QStringList disallowedShortcuts;

};

#endif // TRAYMEMOWINDOW_H
