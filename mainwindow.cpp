#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _discovery = new WsDiscovery();

    _manager = new DeviceManager("admin", "mc1409");
    connect(_discovery, SIGNAL(deviceFound(QString)), _manager, SLOT(deviceFounded(QString)));
    connect(_manager, SIGNAL(deviceDescript(DeviceDescription)), this, SLOT(deviceDescripted(DeviceDescription)));

}

MainWindow::~MainWindow()
{
    delete _discovery;
    delete _manager;
    delete ui;
}

void MainWindow::on_pushButtonSearch_clicked()
{
    _discovery->start();
    _discovery->sendXaddr();
}

void MainWindow::deviceFounded(QString xaddr)
{

}

void MainWindow::deviceDescripted(DeviceDescription description)
{
    ui->listWidgetCameras->addItem(description.toString());
}
