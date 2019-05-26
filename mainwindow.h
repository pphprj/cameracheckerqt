#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "wsdiscovery/wsdiscovery.h"
#include "device/devicemanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonSearch_clicked();
    void deviceFounded(QString xaddr);
    void deviceDescripted(DeviceDescription description);
private:
    Ui::MainWindow *ui;

    WsDiscovery* _discovery;
    DeviceManager* _manager;
};

#endif // MAINWINDOW_H
