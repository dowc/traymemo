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

#include <QtGui>

#include "traymemowindow.h"

#include <QxtGlobalShortcut>
#include <QShortcut>
#include <QFileInfo>
#include <QIODevice>
#include <QMessageBox>

TrayMemoWindow::TrayMemoWindow()
    :proposedFileNameNumbers(0)
{
#ifndef QT_NO_DEBUG
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::Widget);
#else
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::Widget
                   | Qt::WindowStaysOnTopHint);
#endif
    setFocusPolicy(Qt::NoFocus);

    QxtGlobalShortcut *shortCutShowHide = new QxtGlobalShortcut(QKeySequence("Ctrl+E"), this);
    QShortcut *shortCutCreateNew = new QShortcut(QKeySequence("Ctrl+N"), this);
    QShortcut *shortCutSaveText = new QShortcut(QKeySequence("Ctrl+S"), this);
    QShortcut *shortCutOpenExisting = new QShortcut(QKeySequence("Ctrl+O"), this);
    QShortcut *shortCutCloseApp = new QShortcut(QKeySequence("Ctrl+Q"), this);
    QShortcut *shortCutCloseCurrentTab = new QShortcut(QKeySequence("Ctrl+W"), this);
    //QShortcut *shortCutCycleBetweenTabs = new QShortcut(QKeySequence("Ctrl+Tab"), this);
    QObject::connect(shortCutShowHide, SIGNAL(activated()), this, SLOT(showHideWidget()));
    QObject::connect(shortCutCreateNew, SIGNAL(activated()), this, SLOT(openFileSaveDialog()));
    QObject::connect(shortCutSaveText, SIGNAL(activated()), this, SLOT(saveTextToFile()));
    QObject::connect(shortCutOpenExisting, SIGNAL(activated()), this, SLOT(openFileOpenDialog()));
    QObject::connect(shortCutCloseCurrentTab, SIGNAL(activated()), this, SLOT(closeCurrentTab()));
    QObject::connect(shortCutCloseApp, SIGNAL(activated()), this, SLOT(closeApplication()));
    //QObject::connect(shortCutCycleBetweenTabs, SIGNAL(activated()), this, SLOT(changeCurrentTab(int)));

    tabWidget = new QTabWidget(this);
    currentFile = new QFile(this);
    QObject::connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentTab(int)));
    setFocusProxy(tabWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);

    createTrayIcon();

    QIcon icon(":/images/traymemo.svg");
    setWindowIcon(icon);
    setWindowTitle(tr("TrayMemo"));}

void TrayMemoWindow::closeApplication()
{
    anyUnsavedDocuments();
    QCoreApplication::quit();
}

void TrayMemoWindow::changeCurrentTab(int index)
{
    tabWidget->setCurrentIndex(index);
    setCurrentWindowTitle(tabWidget->tabToolTip(index));
    currentTextEdit = dynamic_cast<TrayMemoTab*>(tabWidget->currentWidget());
}

void TrayMemoWindow::openFileSaveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "todo.txt", tr("Text files (*.txt,*.config)"));
    if (fileName.isEmpty())
    {
        fileName = QString("untitled%1.txt").arg(proposedFileNameNumbers);
        ++proposedFileNameNumbers;
    }
    createNewTab(fileName);
}

void TrayMemoWindow::openFileOpenDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "todo.txt", tr("Text files (*.txt,*.config)"));
    if (fileName.isEmpty())
        return;
    createNewTab(fileName);
}

void TrayMemoWindow::saveTextToFile()
{
    QTemporaryFile tempFile;
    if (tempFile.open())
    {
        QTextStream os(&tempFile);
        os << currentTextEdit->toPlainText();

        const QString tempFileName = tempFile.fileName();
        tempFile.close();

        QString currentFileName = currentFile->fileName();
        if (QFile::exists(currentFileName))
        {
           if (!QFile::remove(currentFileName))
           {
               QErrorMessage errorMessage;
               errorMessage.showMessage(QString("Failed to remove current file, %1").arg(currentFile->errorString()));
               errorMessage.exec();
           }
           else
           {
               if (!QFile::copy(tempFileName, currentFileName))
               {
                   QErrorMessage errorMessage;
                   errorMessage.showMessage("Failed to replace current file!");
                   errorMessage.exec();
               }
               else
               {
                   currentTextEdit->setAsSaved();
               }
           }
        }
    }
    else
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage("Failed to create temp file!");
        errorMessage.exec();
    }
}

void TrayMemoWindow::readTextFromFile()
{
    currentTextEdit->clear();
    QTextStream is(currentFile);
    currentTextEdit->setPlainText(is.readAll());
}

void TrayMemoWindow::createNewFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage("Specified file could not be created!");
        errorMessage.exec();
    }

    currentFile->setFileName(fileName);
    currentFile->close();
}

void TrayMemoWindow::openFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage("Specified file could not be opened!");
        errorMessage.exec();
    }
    QTextStream is(&file);
#ifndef QT_NO_CURSOR
     QApplication::setOverrideCursor(Qt::WaitCursor);
 #endif
     currentTextEdit->clear();
     currentTextEdit->setPlainText(is.readAll());
 #ifndef QT_NO_CURSOR
     QApplication::restoreOverrideCursor();
 #endif

    currentFile->setFileName(file.fileName());
    currentFile->close();
}

void TrayMemoWindow::createNewTab(QString fileName)
{
    int count = tabWidget->count();
    while(count > 0)
    {
        if (tabWidget->tabToolTip(count-1) == fileName)
        {
            tabWidget->setCurrentIndex(count-1);
            return;
        }
        --count;
    }

    TrayMemoTab *page = new TrayMemoTab(fileName, this);

    QFile file(fileName);
    if (file.exists())
    {
        currentTextEdit = page;
        openFile(fileName);
    }
    else
    {
        createNewFile(fileName);
        currentTextEdit = page;
    }
    QString name = stripPathFromFileName(fileName);
    int index = tabWidget->addTab(page,name);
    tabWidget->setTabToolTip(index, fileName);
    tabWidget->setCurrentWidget(currentTextEdit);
    setCurrentWindowTitle(fileName);
    currentTextEdit->initCompleted();
    currentTextEdit->setFocus();
}

void TrayMemoWindow::setCurrentWindowTitle(QString fileName)
{
    QString windowTitle = QString(fileName + " - TrayMemo");
    setWindowTitle(windowTitle);
}

QString TrayMemoWindow::stripPathFromFileName(QString fileName)
{    
    QFileInfo info(fileName);
    return info.fileName();
}

void TrayMemoWindow::closeCurrentTab()
{    
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex>=0)
    {
        //TODO refactor
        bool closeCancelled = false;
        if (!currentTextEdit->isSaved())
            closeCancelled = showUnsavedDialog(currentTextEdit->getFileName());

        if (closeCancelled)
            return;
        tabWidget->removeTab(currentIndex);
    }
}

bool TrayMemoWindow::showUnsavedDialog(QString fileName)
{
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.setText(QString("The document %1 has been modified.").arg(fileName));
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int result = msgBox.exec();
    switch (result) {
       case QMessageBox::Save:
           saveTextToFile();
           break;
       case QMessageBox::Discard:
           break;
       case QMessageBox::Cancel:
           return true;
           break;
       default:
           break;
     }
    return false;
}

bool TrayMemoWindow::anyUnsavedDocuments()
{
    bool anySaves = false;
    int count = tabWidget->count();
    while(count > 0)
    {
        tabWidget->setCurrentIndex(count-1);
        currentTextEdit = dynamic_cast<TrayMemoTab*>(tabWidget->widget(count-1));
        if (!currentTextEdit->isSaved())
        {
            showUnsavedDialog(currentTextEdit->getFileName());
            anySaves = true;
        }
        --count;
    }
    return anySaves;
}

void TrayMemoWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    {
        QDialog::setVisible(!isVisible());
        break;
    }
    default:
        break;
    }
}

void TrayMemoWindow::showHideWidget()
{    
    if (!isVisible())
    {
        QDialog::setVisible(true);
        QApplication::setActiveWindow(this);
    }
    else
    {
        QDialog::setVisible(false);
    }

}

void TrayMemoWindow::showAboutMessage()
{
    QMessageBox::about(this, tr("About Traymemo"),
                             tr("<b>TrayMemo</b><br>"
                                "Version 0.59<br>"
                                "Author: Markus Nolvi<br>"
                                "E-mail: markus.nolvi@gmail.com"));
}

void TrayMemoWindow::showCurrentShortcuts()
{
    QMessageBox::information(this, tr("Current shortcuts"),
                             tr("Application to/from tray - Ctrl+E<br>"
                                "Open new file - Ctrl+N<br>"
                                "Open existing file - Ctrl+O<br>"
                                "Close current tab - Ctrl+W<br>"
                                "Save current file - Ctrl+S<br>"));
}

void TrayMemoWindow::createTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable())
        return;

    QAction *minimizeAction = new QAction(tr("&Minimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    QAction *showShortCuts = new QAction(tr("&Schortcuts"), this);
    connect(showShortCuts, SIGNAL(triggered()), this, SLOT(showCurrentShortcuts()));

    QAction *aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutMessage()));

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(showShortCuts);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
    QObject::connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->setToolTip("Traymemo");
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon(":/images/traymemo.svg");
    trayIcon->setIcon(icon);
    trayIcon->show();
}

