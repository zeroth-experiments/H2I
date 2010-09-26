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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QDialog>

class QUrl;

struct MainWindowData;
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotLoadUrl();
    void slotUpdateButtonState();
    void slotLoadPorgress(int);
    void slotLoadFinished(bool);
    void slotLoadUrl(const QUrl& url);
    void slotSaveImage();
    void slotLoadStarted();

private:
    MainWindowData* d;
};

///////////////////////////////////////////////////////////////////////////////
// ImageDailog - Image Dialog is to show the final Image.
///////////////////////////////////////////////////////////////////////////////
struct ImageDialogData;
class ImageDialog: public QDialog {
    Q_OBJECT
public:
    ImageDialog(QWidget* parent=0);
    ~ImageDialog();
    void setImage(QImage& im);
    QImage image() const;

private:
    ImageDialogData* d;
};

#endif // MAINWINDOW_H
