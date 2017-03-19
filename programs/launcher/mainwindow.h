#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onLaunchClicked();

private:
    void populateBinaryComboBox(QString filename, QString description);

private slots:
    void onBinaryChanged(int index);
    void onFullscreenOptionChanged(bool value);
    void onDisplayChanged(int index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
