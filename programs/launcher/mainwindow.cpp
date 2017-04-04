#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->launchButton, SIGNAL(clicked()), this, SLOT(onLaunchClicked()));
    connect(ui->binaryComboBox, SIGNAL(activated(int)), this, SLOT(onBinaryChanged(int)) );
    connect(ui->displayComboBox, SIGNAL(activated(int)), this, SLOT(onDisplayChanged(int)) );
    connect(ui->fullscreenCheckBox, SIGNAL(toggled(bool)), this, SLOT(onFullscreenOptionChanged(bool)));
    
    int screenCount = QApplication::desktop()->screenCount();
    int primaryScreenId = QApplication::desktop()->primaryScreen();
    for(int i = 1; i <= screenCount; i++)
    {
        if(i == primaryScreenId + 1) ui->displayComboBox->addItem(QString("Display %1 (Primary)").arg(i), QString::number(i));
        else ui->displayComboBox->addItem(QString("Display %1").arg(i), QString::number(i));
    }

    ui->displayComboBox->setCurrentIndex(primaryScreenId);
    //To reposition the window
    onDisplayChanged(primaryScreenId);

    populateBinaryComboBox( "vsxu_artiste", "VSXu Artiste" );
    populateBinaryComboBox( "vsxu_player", "VSXu Player" );
#ifndef Q_OS_WIN
    populateBinaryComboBox( "vsxu_server", "VSXu Server" );
#endif
    populateBinaryComboBox( "", "Manually specify.." );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateBinaryComboBox(QString filename, QString description)
{
    QString binaryname = filename;

    QDir d(QCoreApplication::applicationDirPath());
#ifdef Q_OS_WIN
    binaryname = binaryname + ".exe";
#endif

    if( (!filename.isEmpty()))
    {
        if(d.exists(binaryname))
            ui->binaryComboBox->addItem(description, d.absoluteFilePath(binaryname));
    }
    else
    {
            ui->binaryComboBox->addItem(description, "");
    }

}

void MainWindow::onLaunchClicked()
{
    QString binaryPath = ui->binaryComboBox->itemData(ui->binaryComboBox->currentIndex()).toString();
    if(binaryPath.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Please Select the VSXu Binary to Launch"));
        msgBox.exec();
        return;
    }

    QStringList args;
    
    args << "-d" << ui->displayComboBox->currentData().toString();

    if(ui->fullscreenCheckBox->isChecked()) args << "-f";
    else {
        args << "-s" << QString("%1x%2").arg(QString::number(ui->widthSpinBox->value())).arg(QString::number(ui->heightSpinBox->value()));
    
        args << "-p" << QString("%1,%2").arg(QString::number(ui->xSpinBox->value())).arg(QString::number(ui->ySpinBox->value()));

        if(ui->borderlessCheckBox->isChecked())
            args << "-bl";
    }
    
    qDebug() << args;

    QProcess vsxuProcess;
    vsxuProcess.startDetached(binaryPath,args);

    QApplication::exit();
}


void MainWindow::onBinaryChanged(int index)
{
    if(ui->binaryComboBox->itemData(index).toString().isEmpty())
    {
        // If the User activated the "Manually specify..." option, for which filepath is ""
        QString filepath = QFileDialog::getOpenFileName(this,
                                                        tr("Select VSXu Bianry"),
                                                        QDir::currentPath(),
                                                        "VSXu Player (vsxu_player vsxu_player.exe);;VSXu Artiste (vsxu_artiste vsxu_artiste.exe);; Any File (*)" );
        if(!filepath.isEmpty())
        {
            ui->binaryComboBox->insertItem(-1,filepath, filepath);
            ui->binaryComboBox->setCurrentIndex( ui->binaryComboBox->count() - 2 );
        }
    }
}

void MainWindow::onFullscreenOptionChanged(bool value)
{
    ui->windowPropertiesGroup->setEnabled(!value);
}

void MainWindow::onDisplayChanged(int index)
{
    QRect screenGeometry = QApplication::desktop()->screenGeometry(index);
    int screenWidth = screenGeometry.width(), screenHeight = screenGeometry.height();
    int w = 1280, h = 720;

    ui->widthSpinBox->setValue(w);
    ui->heightSpinBox->setValue(h);

    //Center the window by default
    ui->xSpinBox->setValue((screenWidth - w)/2);
    ui->ySpinBox->setValue((screenHeight - h)/2);
}
