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
#include <QCloseEvent>

TrayMemoWindow::TrayMemoWindow()
    :proposedFileNameNumbers(0)
{
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::Widget
                   | Qt::WindowStaysOnTopHint);
    setFocusPolicy(Qt::NoFocus);

    createActions();
    createTrayIcon();

    shortCutShowHide = new QxtGlobalShortcut(QKeySequence("Ctrl+E"), this);
    shortCutCreateNew = new QShortcut(QKeySequence("Ctrl+N"), this);
    shortCutSaveText = new QShortcut(QKeySequence("Ctrl+S"), this);
    shortCutOpenExisting = new QShortcut(QKeySequence("Ctrl+O"), this);
    shortCutCloseApp = new QShortcut(QKeySequence("Ctrl+Q"), this);
    shortCutCloseCurrentTab = new QShortcut(QKeySequence("Ctrl+W"), this);
    //shortCutCycleBetweenTabs = new QShortcut(QKeySequence("Ctrl+Tab"), this);
    QObject::connect(shortCutShowHide, SIGNAL(activated()), this, SLOT(showHideWidget()));
    QObject::connect(shortCutCreateNew, SIGNAL(activated()), this, SLOT(openFileSaveDialog()));
    QObject::connect(shortCutSaveText, SIGNAL(activated()), this, SLOT(saveTextToFile()));
    QObject::connect(shortCutOpenExisting, SIGNAL(activated()), this, SLOT(openFileOpenDialog()));
    QObject::connect(shortCutCloseCurrentTab, SIGNAL(activated()), this, SLOT(closeCurrentTab()));
    QObject::connect(shortCutCloseApp, SIGNAL(activated()), this, SLOT(closeApplication()));
//    QObject::connect(shortCutCycleBetweenTabs, SIGNAL(activated()), this, SLOT(changeCurrentTab()));

    QObject::connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    tabWidget = new QTabWidget(this);
    QObject::connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentTab(int)));
    setFocusProxy(tabWidget);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);

    setIcon(1);
    trayIcon->show();

    setWindowTitle(tr("Traymemo"));
    currentFile = new QFile(this);
}

void TrayMemoWindow::closeApplication()
{
    anyUnsavedDocuments();
    QCoreApplication::quit();
}

void TrayMemoWindow::changeCurrentTab(int index)
{
    tabWidget->setCurrentIndex(index);
    setCurrentWindowTitle(tabWidget->tabToolTip(index));
    currentTextEdit = dynamic_cast<TextTabWidget*>(tabWidget->currentWidget());

    // DOES NOT WORK
//    int count = tabWidget->count();
//    if (count > 0)
//    {
//        int current = tabWidget->currentIndex();
//        if (count > 1)
//        {
//            tabWidget->sesetCurrentIndex(current + 1);
//            currentTextEdit = dynamic_cast<TextTabWidget*>(tabWidget->currentWidget());
//        }
//    }
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
        if (!currentFile->open(QIODevice::ReadWrite | QIODevice::Text))
            return;
        QString currentFileName = currentFile->fileName();
        if (QFile::exists(currentFileName))
           QFile::remove(currentFileName);

        QFile::copy(tempFileName, currentFileName);
        currentTextEdit->setAsSaved();
    }
    else
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage("Failed to save text!");
    }

    //currentFile->seek(0);

}

void TrayMemoWindow::readTextFromFile()
{
    currentTextEdit->clear();
    QTextStream is(currentFile);
    currentTextEdit->setPlainText(is.readAll());
}

void TrayMemoWindow::createNewFile(QString fileName)
{
    //currentFile->setFileName(fileName);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage("Specified file could not be created!");
    }

    currentFile = &file;
}

void TrayMemoWindow::openFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage("Specified file could not be opened!");
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

    TextTabWidget *page = new TextTabWidget(fileName, this);
//    if (fileName.isEmpty())
//        fileName = getNextFreeFileName();

    QFile file(fileName);
    if (file.exists())
    {
        currentTextEdit = page;
        openFile(fileName);

        //readTextFromFile();
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
            closeCancelled = showUnsavedDialog();

        if (closeCancelled)
            return;
        tabWidget->removeTab(currentIndex);
    }
}

bool TrayMemoWindow::showUnsavedDialog()
{
    QMessageBox msgBox;
    msgBox.setText("The document has been modified.");
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
        currentTextEdit = dynamic_cast<TextTabWidget*>(tabWidget->widget(count-1));
        if (!currentTextEdit->isSaved())
        {
            showUnsavedDialog();
            anySaves = true;
        }
        --count;
    }
    return anySaves;
}

void TrayMemoWindow::closeEvent(QCloseEvent *event)
{
//    if (trayIcon->isVisible()) {
//        hide();
//        event->ignore();
//    }
}

void TrayMemoWindow::setIcon(int index)
{
    QIcon icon(":/images/traymemo.svg");
    //QIcon icon(":/images/heart.svg");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    //trayIcon->setToolTip(iconComboBox->itemText(index));
}

void TrayMemoWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    {
        QDialog::setVisible(!isVisible());
     }
        break;    
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

void TrayMemoWindow::showMessage()
{
    QMessageBox::about(this, tr("About Traymemo"),
                             tr("<b>TrayMemo</b><br>"
                                "Version 0.5<br>"
                                "Author: Markus Nolvi<br>"
                                "E-mail: markus.nolvi@gmail.com"));
}

void TrayMemoWindow::createActions()
{
    showAction = new QAction(tr("&Open"), this);
    connect(showAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showMessage()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

}

void TrayMemoWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setToolTip("Traymemo");
    trayIcon->setContextMenu(trayIconMenu);
}

