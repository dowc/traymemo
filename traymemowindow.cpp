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

#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeView>
#include <QxtGlobalShortcut>
#include <QShortcut>

TrayMemoWindow::TrayMemoWindow()
    :proposedFileNameNumbers(0)
{
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::Widget
                   | Qt::WindowStaysOnTopHint);
    setFocusPolicy(Qt::NoFocus);

    createActions();
    createTrayIcon();

    shortCutShowHide = new QxtGlobalShortcut(QKeySequence("Ctrl+E"), this);
    shortCutCreateNew = new QShortcut(QKeySequence("Ctrl+N"), this);
    QObject::connect(shortCutShowHide, SIGNAL(activated()), this, SLOT(showHideWidget()));
    QObject::connect(shortCutCreateNew, SIGNAL(activated()), this, SLOT(openFileSaveDialog()));

    QObject::connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
//    canvas = new QDeclarativeView(this);
//    canvas->setResizeMode(QDeclarativeView::SizeRootObjectToView);
//    canvas->engine()->rootContext()->setContextObject(this);
//    canvas->engine()->rootContext()->setContextProperty("traymemowindow", this);
//    canvas->setSource(QUrl("qrc:/qml/main.qml"));
//    canvas = new QTextEdit(this);
    tabWidget = new TextTabWidget("text.txt", this);
    setFocusProxy(tabWidget);
//    canvas->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::Widget
//                           | Qt::WindowStaysOnTopHint);
//    canvas->setFocusPolicy(Qt::StrongFocus);

    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);

    setIcon(1);
    trayIcon->show();

    setWindowTitle(tr("traymemo"));
    //createNewTab("test.txt");
    openFileSaveDialog();
}

void TrayMemoWindow::openFileSaveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "todo.txt", tr("Text     files (*.txt,*.config)"));

    createNewTab(fileName);
}

void TrayMemoWindow::createNewTab(QString fileName)
{
    QTextEdit *page = new QTextEdit;
    if (fileName.isEmpty())
        fileName = getNextFreeFileName();

    QString name = stripPathFromFileName(fileName);
    int index = tabWidget->addTab(page,name);
    tabWidget->setTabToolTip(index, fileName);
    page->setFocus();
}

QString TrayMemoWindow::stripPathFromFileName(QString fileName)
{
    return fileName.remove(0, fileName.lastIndexOf("/"));
}

void TrayMemoWindow::closeTab()
{
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex>0)
        tabWidget->removeTab(currentIndex);
}

QString TrayMemoWindow::getNextFreeFileName()
{
    QString name = "text";
    ++proposedFileNameNumbers;
    name += QString::number(proposedFileNameNumbers) + ".txt";
    return name;
}

void TrayMemoWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {        
        hide();
        event->ignore();
    }
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
    //QDialog::setVisible(!isVisible());
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
    QMessageBox::information(this, tr("traymemo"),
                             tr("<b>About traymemo</b><br>"
                                "traymemo version 0.4<br>"
                                "Aauthor: Markus Nolvi"));
}

void TrayMemoWindow::createActions()
{
//    minimizeAction = new QAction(tr("Mi&nimize"), this);
//    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

//    maximizeAction = new QAction(tr("Ma&ximize"), this);
//    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

//    restoreAction = new QAction(tr("&Restore"), this);
//    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

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
    trayIcon->setContextMenu(trayIconMenu);
}

