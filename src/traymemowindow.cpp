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

#include "traymemowindow.h"

#include <QxtGlobalShortcut>
#include <QShortcut>
#include <QFileInfo>
#include <QIODevice>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

TrayMemoWindow::TrayMemoWindow()
    :proposedFileNameNumbers(0),
     currentTextEdit(NULL),
     shortCutShowHide(NULL)
{
#ifdef Q_WS_WIN
    QSettings autoStartSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    autoStartSettings.setValue("traymemo.exe", QCoreApplication::applicationFilePath().replace('/','\\'));
#endif

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::Widget);
    setFocusPolicy(Qt::NoFocus);

    disallowedShortcuts << "Ctrl+T" << "Ctrl+O" << "Ctrl+S" << "Ctrl+W" << "Ctrl+Q" << "Ctrl+Tab" << "Ctrl+Shift+Tab";

    QShortcut *shortCutCreateNew = new QShortcut(QKeySequence("Ctrl+T"), this);
    QShortcut *shortCutSaveText = new QShortcut(QKeySequence("Ctrl+S"), this);
    QShortcut *shortCutOpenExisting = new QShortcut(QKeySequence("Ctrl+O"), this);
    QShortcut *shortCutCloseApp = new QShortcut(QKeySequence("Ctrl+Q"), this);
    QShortcut *shortCutCloseCurrentTab = new QShortcut(QKeySequence("Ctrl+W"), this);
    QShortcut *shortCutCycleToNextTab = new QShortcut(QKeySequence("Ctrl+Tab"), this);
    QShortcut *shortCutCycleToPreviousTab = new QShortcut(QKeySequence("Ctrl+Shift+Tab"), this);    
    QObject::connect(shortCutCreateNew, SIGNAL(activated()), this, SLOT(openFileSaveDialog()));
    QObject::connect(shortCutSaveText, SIGNAL(activated()), this, SLOT(saveTextToFile()));
    QObject::connect(shortCutOpenExisting, SIGNAL(activated()), this, SLOT(openFileOpenDialog()));
    QObject::connect(shortCutCloseCurrentTab, SIGNAL(activated()), this, SLOT(closeCurrentTab()));
    QObject::connect(shortCutCloseApp, SIGNAL(activated()), this, SLOT(closeApplication()));
    QObject::connect(shortCutCycleToNextTab, SIGNAL(activated()), this, SLOT(moveToNextTab()));
    QObject::connect(shortCutCycleToPreviousTab, SIGNAL(activated()), this, SLOT(moveToPreviousTab()));

    tabWidget = new QTabWidget(this);

    QObject::connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentTab(int)));
    setFocusProxy(tabWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);

    createTrayIcon();

    QIcon icon(":/images/traymemo.svg");
    setWindowIcon(icon);
    setWindowTitle(tr("TrayMemo"));

    QSettings settings("Traymemo", "Traymemo");
    restoreGeometry(settings.value("geometry").toByteArray());
    assignShowHideShortCut(settings.value("showhideshortcut").toString());
    restorePreviousSessionTabs();
    QObject::connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), this,
                     SLOT(commitData(QSessionManager &)),Qt::DirectConnection);
}

TrayMemoWindow::~TrayMemoWindow()
{
    delete tabWidget;
    delete shortCutShowHide;
}

void TrayMemoWindow::commitData(QSessionManager &manager)
{
    qDebug() << "commitData called";
    if (manager.allowsInteraction())
    {
        queryForUnsavedDocuments();
        saveAppSettings();
        manager.release();
        qDebug() << "documents & settings saved";
    }
    else
    {
        saveAppSettings();
        qDebug() << "only settings saved";
    }
}

void TrayMemoWindow::closeApplication()
{
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.setText("Do you really want to quit?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int result = msgBox.exec();
    if (result == QMessageBox::Ok)
    {
        queryForUnsavedDocuments();
        saveAppSettings();
        QCoreApplication::quit();
    }
}

void TrayMemoWindow::saveAppSettings()
{
    QSettings settings("Traymemo", "Traymemo");
    settings.setValue("geometry", saveGeometry());
    saveSessionTabs();
}

void TrayMemoWindow::queryForUnsavedDocuments()
{
    if (anyUnsavedDocuments())
    {
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgBox.setText("You have unsaved documents. Do you want to save them before quitting?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int result = msgBox.exec();
        if (result == QMessageBox::Ok)
        {
            SaveUnsavedDocuments();
        }
    }
}

void TrayMemoWindow::changeCurrentTab(int index)
{
    if (index < 0)
        return;

    tabWidget->setCurrentIndex(index);
    setCurrentWindowTitle(tabWidget->tabToolTip(index));
    currentTextEdit = dynamic_cast<TrayMemoTab*>(tabWidget->currentWidget());
    currentTextEdit->setFocus();
}

void TrayMemoWindow::moveToNextTab()
{
    int index = tabWidget->currentIndex();
    if ((tabWidget->count() - 1) > index)
        tabWidget->setCurrentIndex(index + 1);
    else
        tabWidget->setCurrentIndex(0);
}

void TrayMemoWindow::restorePreviousSessionTabs()
{
    QMap<int, QString> map;

    QSettings settings("Traymemo", "Traymemo");
    settings.beginGroup("savedtabs");
    QStringList keys = settings.childKeys();
    foreach (QString key, keys) {
         map[settings.value(key).toInt()] = key;
    }
    settings.endGroup();

    foreach (QString value, map.values())
        createNewTab(value);
}

void TrayMemoWindow::saveSessionTabs() const
{
    int count = tabWidget->count();
    QMap<QString, int> map;
    while(count>0)
    {
        map[tabWidget->tabToolTip(count-1)] = count-1;
        --count;
    }

    QSettings settings("Traymemo", "Traymemo");
    settings.beginGroup("savedtabs");
    settings.remove("");
    QMap<QString, int>::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
         settings.setValue(i.key(), i.value());
         ++i;
     }
    settings.endGroup();
}

void TrayMemoWindow::assignShowHideShortCut(const QString value)
{
    if (shortCutShowHide)
    {
        delete shortCutShowHide;
        shortCutShowHide = NULL;
    }

    QString shortcut = value;

    if (value.isEmpty())
        shortcut = "Ctrl+E";

    shortCutShowHide = new QxtGlobalShortcut(QKeySequence(shortcut), this);

    QSettings settings("Traymemo", "Traymemo");
    settings.setValue("showhideshortcut", shortcut);

    QObject::connect(shortCutShowHide, SIGNAL(activated()), this, SLOT(showHideWidget()));
}

void TrayMemoWindow::moveToPreviousTab()
{
    int index = tabWidget->currentIndex();
    if (index > 0)
        tabWidget->setCurrentIndex(index - 1);
    else if (index == 0)
        tabWidget->setCurrentIndex(tabWidget->count() - 1);
}

void TrayMemoWindow::openFileSaveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath() + "/memo.txt", tr("Text files (*.txt *.config)"));
    if (fileName.isEmpty())
    {
        return;
//        fileName = QString("untitled%1.txt").arg(proposedFileNameNumbers);
//        ++proposedFileNameNumbers;
    }
    createNewTab(fileName);
}

void TrayMemoWindow::openFileOpenDialog()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File(s)"), QDir::homePath(), tr("Text files (*.txt *.config)"));
    if (fileNames.isEmpty())
        return;

    foreach(QString fileName, fileNames)
        createNewTab(fileName);
}

void TrayMemoWindow::saveTextToFile()
{
    QString filename = currentTextEdit->getFileName();
    qDebug() << "Saving of file %s started" << filename;
    if (tabWidget->currentWidget() == NULL)
        return;

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QTemporaryFile tempFile;
    if (tempFile.open())
    {
        qDebug("Temp file created");
        QTextStream os(&tempFile);
        os << currentTextEdit->toPlainText();
        qDebug("Text written to stream");
        const QString tempFileName = tempFile.fileName();
        tempFile.close();

        QString currentFileName = currentTextEdit->getFileName();
        if (QFile::exists(currentFileName))
        {
           qDebug("File exists...");
           if (QFile::remove(currentFileName))
           {
               qDebug("File removed successfully...");
           }
           else
           {
               qDebug("File removal failed...");
               QErrorMessage errorMessage;
               errorMessage.showMessage("Failed to remove current file");
               errorMessage.exec();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
               return;
           }
        }

        if (QFile::copy(tempFileName, currentFileName))
        {
            currentTextEdit->setAsSaved();
            qDebug("File replaced successfully...");

        }
        else
        {
            qDebug("File replacing failed...");
            QErrorMessage errorMessage;
            errorMessage.showMessage("Failed to copy current file!");
            errorMessage.exec();
        }
    }
    else
    {
        qDebug("Temp file creation failed...");
        QErrorMessage errorMessage;
        errorMessage.showMessage("Failed to create temp file!");
        errorMessage.exec();
    }

    qDebug() << "Saving of file %s completed succesfully" << filename;
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

void TrayMemoWindow::readTextFromFile(QFile &file)
{
    QTextStream is(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    currentTextEdit->clear();
    currentTextEdit->setPlainText(is.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
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

    readTextFromFile(file);
}

void TrayMemoWindow::updateAsterisk()
{
    int index = tabWidget->currentIndex();
    QString fileName = currentTextEdit->getFileName();
    if (!currentTextEdit->isSaved())
        fileName.append("*");
    else
        fileName.remove("*");
    tabWidget->setTabText(index, stripPathFromFileName(fileName));
    setCurrentWindowTitle(fileName);
}

void TrayMemoWindow::createNewTab(QString fileName)
{
    //Check if selected file is already opened
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

    TrayMemoTab *page = new TrayMemoTab(QDir::toNativeSeparators(fileName), this);
    currentTextEdit = page;

    QFile file(fileName);
    if(file.exists())
        openFile(fileName);
    else
        createNewFile(fileName);

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
    return QFileInfo(fileName).fileName();
}

void TrayMemoWindow::closeCurrentTab()
{    
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex>=0)
    {
        //TODO refactor
        bool closingCancelled = false;
        if (!currentTextEdit->isSaved())
            closingCancelled = showUnsavedDialog(currentTextEdit->getFileName());

        if (closingCancelled)
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
            //showUnsavedDialog(currentTextEdit->getFileName());
            anySaves = true;
        }
        --count;
    }
    return anySaves;
}

void TrayMemoWindow::SaveUnsavedDocuments()
{
    int count = tabWidget->count();
    while(count > 0)
    {
        tabWidget->setCurrentIndex(count-1);
        currentTextEdit = dynamic_cast<TrayMemoTab*>(tabWidget->widget(count-1));
        if (!currentTextEdit->isSaved())
        {
            showUnsavedDialog(currentTextEdit->getFileName());
        }
        --count;
    }
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
        QSettings settings("Traymemo", "Traymemo");
        restoreGeometry(settings.value("geometry").toByteArray());

        QDialog::setVisible(true);
        QApplication::setActiveWindow(this);
        activateWindow();
        if (currentTextEdit)
            currentTextEdit->setFocus();
    }
    else
    {
        if (QApplication::activeWindow() == this)
        {
            QSettings settings("Traymemo", "Traymemo");
            settings.setValue("geometry", saveGeometry());
            QDialog::setVisible(false);
        }
        else
        {
            QApplication::setActiveWindow(this);
            activateWindow();
            if (currentTextEdit)
                currentTextEdit->setFocus();
        }
    }
}

void TrayMemoWindow::showAboutMessage()
{
//    QMessageBox *about = new QMessageBox(this);
//    about->setWindowTitle(tr("About Traymemo"));
//    about->setText(tr("<b>TrayMemo</b><br>"
//                      "Version 0.84<br>"
//                      "Author: Markus Nolvi<br>"
//                      "E-mail: markus.nolvi@gmail.com"));
//    about->setDefaultButton(QMessageBox::Ok);
//    about->exec();
    QMessageBox::about(this, tr("About Traymemo"),
                             tr("<b>TrayMemo</b><br>"
                                "Version 0.84<br>"
                                "Author: Markus Nolvi<br>"
                                "E-mail: markus.nolvi@gmail.com"));
}

void TrayMemoWindow::showCurrentShortcuts()
{
    QSettings settings("Traymemo", "Traymemo");
    QString currentSetting = settings.value("showhideshortcut").toString();

    QMessageBox::information(this, tr("Current shortcuts"),
                             tr("Show/hide shortcut - %1<br>"
                                "Create new file - Ctrl+T<br>"
                                "Open existing file - Ctrl+O<br>"
                                "Close current tab - Ctrl+W<br>"
                                "Save current file - Ctrl+S<br>"
                                "Move to next tab - Ctrl+Tab<br>"
                                "Move to previous tab - Ctrl+Shift+Tab<br>").arg(currentSetting));
}

void TrayMemoWindow::showChangeDialog()
{
    QSettings settings("Traymemo", "Traymemo");
    QString currentSetting = settings.value("showhideshortcut").toString();
    bool ok;
    QString shortcut = QInputDialog::getText(this, tr("Define show/hide shortcut"),
                                                   tr("Shortcut (f.ex: Ctrl+E):                                       "),
                                                   QLineEdit::Normal,
                                                   currentSetting, &ok);

    if (ok)
    {
        if (disallowedShortcuts.contains(shortcut, Qt::CaseInsensitive))
        {
            QMessageBox::information(this, tr("Error occurred"),
                                        tr("Shortcut cannot be assigned.<br>"
                                        "Selected shortcut %1 is already "
                                        "assigned to some other operation.<br>"
                                        "Please select some other shortcut.<br>").arg(shortcut));
            return;
        }

        if (!shortcut.isEmpty())
        {
            assignShowHideShortCut(shortcut);
        }
        else
        {
            QMessageBox::information(this, tr("Error occurred!"),
                                        tr("Cannot assign empty shortcut.<br>"));

            return;
        }
    }
}

void TrayMemoWindow::createTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable())
        return;

    QAction *minimizeMaximizeAction = new QAction(tr("&Minimize/Maximize"), this);
    connect(minimizeMaximizeAction, SIGNAL(triggered()), this, SLOT(showHideWidget()));

    QAction *showShortCuts = new QAction(tr("&Shortcuts"), this);
    connect(showShortCuts, SIGNAL(triggered()), this, SLOT(showCurrentShortcuts()));

    QAction *changeShowHideShortCut = new QAction(tr("&Change show/hide shortcut"), this);
    connect(changeShowHideShortCut, SIGNAL(triggered()), this, SLOT(showChangeDialog()));

    QAction *aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutMessage()));

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeMaximizeAction);
    trayIconMenu->addAction(showShortCuts);
    trayIconMenu->addAction(changeShowHideShortCut);    
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

