/***************************************************************************
 *   Copyright (C) 2010 by Abhishek Patil <abhishekworld@gmail.com>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "MainWindow.h"

#include <QWebView>
#include <QWebFrame>
#include <QWebSettings>
#include <QToolBar>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QAction>
#include <QFileDialog>
#include <QImage>
#include <QDesktopServices>
#include <QToolButton>
#include <QPushButton>

#define TITLE "H2I - Html"

struct MainWindowData
{
    QGridLayout* layout;
    QWebView* view;
    QToolButton* tbBack;
    QToolButton* tbFwd;
    QToolButton* tbLoad;
    QToolButton* tbSave;
    QLineEdit* urlEdit;
};

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle(TITLE);
    d = new MainWindowData;

    d->tbBack = new QToolButton;
    d->tbFwd = new QToolButton;
    d->tbLoad = new QToolButton;
    d->tbSave = new QToolButton;
    d->urlEdit = new QLineEdit;

    d->tbBack->setText("<");
    d->tbBack->setEnabled(false);

    d->tbFwd->setText(">");
    d->tbFwd->setEnabled(false);

    d->tbLoad->setText(QString::fromUtf8("↵"));
    d->tbSave->setText("Get Image!");
    d->tbSave->setEnabled(false);

    d->view = new QWebView;
    d->view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    // QWebKit Settings- Enable Plugins (like Flash), Enable Java, Enable Java Script, Enable AutoLoadImages
    // Fix-Me move this to Configuration box
    d->view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    d->view->settings()->setAttribute(QWebSettings::JavaEnabled, true);
    d->view->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    d->view->settings()->setAttribute(QWebSettings::AutoLoadImages, true);

    // Layout the widget
    d->layout = new QGridLayout(this);
    d->layout->setMargin(5);
    d->layout->setSpacing(5);

    d->layout->addWidget(d->tbBack, 0, 0, 1, 1);
    d->layout->addWidget(d->tbFwd, 0, 1, 1, 1);
    d->layout->addWidget(d->urlEdit, 0, 2, 1, 4);
    d->layout->addWidget(d->tbLoad, 0, 6, 1, 1);
    d->layout->addWidget(d->tbSave, 0, 7, 1, 1);
    d->layout->addWidget(d->view, 1, 0, 1, 8);

    // Tool-Bar connection
    connect(d->tbBack, SIGNAL(clicked()), d->view, SLOT(back()));
    connect(d->tbFwd, SIGNAL(clicked()), d->view, SLOT(forward()));
    connect(d->tbLoad, SIGNAL(clicked()), this, SLOT(slotLoadUrl()));
    connect(d->urlEdit, SIGNAL(returnPressed()), this, SLOT(slotLoadUrl()));

    // QWebView Connections
    connect(d->view->pageAction(QWebPage::Back), SIGNAL(changed()), this, SLOT(slotUpdateButtonState()));
    connect(d->view->pageAction(QWebPage::Forward), SIGNAL(changed()), this, SLOT(slotUpdateButtonState()));
    connect(d->view, SIGNAL(loadProgress(int)), this, SLOT(slotLoadPorgress(int)));
    connect(d->view, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinished(bool)));
    connect(d->view, SIGNAL(linkClicked(QUrl)), this, SLOT(slotLoadUrl(QUrl)));
    connect(d->view, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));

    // Lets do the real work ;-) Html to Image converter connection
    connect(d->tbSave, SIGNAL(clicked()), this, SLOT(slotSaveImage()));
}

MainWindow::~MainWindow()
{
    delete d;
}

void MainWindow::slotUpdateButtonState()
{
    d->tbBack->setEnabled(d->view->pageAction(QWebPage::Back)->isEnabled());
    d->tbFwd->setEnabled(d->view->pageAction(QWebPage::Forward)->isEnabled());
}

void MainWindow::slotLoadUrl()
{
    QString addressBarText = d->urlEdit->text();
    QString address;
    if(addressBarText.isEmpty())
        return;
    if(addressBarText.startsWith("http:\/\/") || addressBarText.startsWith("https:\/\/") || addressBarText.startsWith("file:\/\/\/"))
        address = addressBarText;
    else
        address = addressBarText.prepend("http://");

    QUrl url(addressBarText, QUrl::TolerantMode);
    this->slotLoadUrl(url);
}

void MainWindow::slotLoadUrl(const QUrl& url)
{
    d->urlEdit->setText(url.toString());
    d->view->load(url);
}

void MainWindow::slotLoadPorgress(int val)
{
    this->setWindowTitle(QString("Loading... (%1)% - %2")
                         .arg(val)
                         .arg(TITLE));
}

void MainWindow::slotLoadFinished(bool state)
{
    if(state)
    {
        d->tbSave->setEnabled(true);
        this->setWindowTitle(QString("%1 - %2")
                            .arg(d->view->title())
                            .arg(TITLE));
    }
}

void MainWindow::slotSaveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", QString("%1/untitled.png").arg(QDir::homePath()), "Images (*.png *.xpm *.jpg)");
    if(fileName.isEmpty())
        return;
    QSize currentSize = d->view->size();
    QSize pageSize(d->view->page()->mainFrame()->contentsSize());
    d->view->page()->setViewportSize(pageSize);
    QImage im(pageSize, QImage::Format_ARGB32);

    QPainter painter(&im);
    d->view->page()->mainFrame()->render(&painter);
    im.save(fileName);
    d->view->page()->setViewportSize(currentSize);
    QString fileUrl = "file:///" + fileName;

    //QDesktopServices::openUrl(QUrl(fileUrl));
    ImageDialog dialog;
    dialog.setWindowTitle(QString("%1 - %2").arg(fileName).arg("H2I - Image"));
    dialog.setImage(im);
    dialog.exec();
}

void MainWindow::slotLoadStarted()
{
    d->tbSave->setEnabled(false);
}

///////////////////////////////////////////////////////////////////////////////
// ImageDailog - Image Dialog is to show the final Image.
///////////////////////////////////////////////////////////////////////////////

struct ImageDialogData
{
    QImage im;
    QLabel* label;
};

ImageDialog::ImageDialog(QWidget* parent)
    :QDialog(parent)
{
    d = new ImageDialogData;
    d->label = new QLabel("No Image");
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(d->label, 0, 0);
}

ImageDialog::~ImageDialog()
{
    delete d;
}

void ImageDialog::setImage(QImage& im)
{
    d->im = im;
    d->label->setPixmap(QPixmap::fromImage(d->im));
}

QImage ImageDialog::image() const
{
    return d->im;
}
