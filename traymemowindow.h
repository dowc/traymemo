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

#ifndef TRAYMEMOWINDOW_H
#define TRAYMEMOWINDOW_H

#include <QtGui>
#include <QSystemTrayIcon>
#include <QDialog>
#include <QShortcut>
#include <QxtGlobalShortcut>
#include <QtDeclarative/QDeclarativeView>
#include <QTabWidget>
#include <texttabwidget.h>

class TrayMemoWindow : public QDialog
{
    Q_OBJECT

public:
    TrayMemoWindow();

protected:
    void closeEvent(QCloseEvent *event);    

private slots:
    void setIcon(int index);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void showHideWidget();
    void openFileSaveDialog();
    void openFileOpenDialog();
    void saveTextToFile();
    void readTextFromFile();
    void closeCurrentTab();
    void closeApplication();
    void changeCurrentTab(int index);

private:
    void createTrayIcon();
    void createActions();
    void createNewTab(QString fileName);
    void createNewFile(QString fileName);
    void openFile(QString fileName);
    QString stripPathFromFileName(QString fileName);
    bool showUnsavedDialog();
    bool anyUnsavedDocuments();
    void setCurrentWindowTitle(QString fileName);

    QAction *showAction;
    QAction *aboutAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QxtGlobalShortcut* shortCutShowHide;
    //QDeclarativeView *canvas;
//    QTextEdit *canvas;
//    QTabWidget* tabs;
    QShortcut *shortCutCreateNew;
    QShortcut *shortCutSaveText;
    QShortcut *shortCutOpenExisting;
    QShortcut *shortCutCloseApp;
    QShortcut *shortCutCloseCurrentTab;
    QShortcut *shortCutCycleBetweenTabs;
    //TextTabWidget *tabWidget;
    QTabWidget *tabWidget;
    int proposedFileNameNumbers;
    QFile *currentFile;
    //QTextEdit *currentTextEdit;
    TextTabWidget *currentTextEdit;

};

#endif // TRAYMEMOWINDOW_H
