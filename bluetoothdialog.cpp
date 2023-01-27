#include "bluetoothdialog.h"
#include "ui_bluetoothdialog.h"

#include <QMenu>
#include <iostream>
#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <qlowenergycontroller.h>
#include <unistd.h>
using std::cerr;
using std::cout;

BluetoothDialog::BluetoothDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BluetoothDialog),
      localDevice(new QBluetoothLocalDevice) {
  ui->setupUi(this);
  discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
  connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
          this, &BluetoothDialog::addDevice);
  connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this,
          &BluetoothDialog::finishedScan);
  ui->list->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->list, &QWidget::customContextMenuRequested, this,
          &BluetoothDialog::showPairingMenu);
  connect(localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this,
          &BluetoothDialog::hostModeStateChanged);
  connect(ui->scanButton, &QAbstractButton::clicked, (this),
          &BluetoothDialog::startScan);
  connect(ui->turnonButton, &QAbstractButton::clicked, (this),
          &BluetoothDialog::turnOn);
  discoveryAgent->start();
}

BluetoothDialog::~BluetoothDialog() { delete ui; }

void BluetoothDialog::finishedScan() { cout << "finished scan \n"; }

void BluetoothDialog::addDevice(const QBluetoothDeviceInfo &device) {
  const QString label = device.address().toString() + ' ' + device.name();
  devices[device.address()] = device;
  const auto items = ui->list->findItems(label, Qt::MatchExactly);
  if (items.isEmpty()) {
    QListWidgetItem *item = new QListWidgetItem(label);
    ui->list->addItem(item);
  }
  if (device.coreConfigurations() &
      QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
    cout << ("Low Energy device " + label.toStdString() +
             " found. Scanning more...\n");
  }
}

void BluetoothDialog::showPairingMenu(const QPoint &point) {
  if (ui->list->count() == 0)
    return;
  QMenu menu(this);
  QAction *pairAction = menu.addAction("Pair");
  QAction *unpairAction = menu.addAction("Unpair");
  QAction *connectAction = menu.addAction("Connect");
  QAction *action = menu.exec(ui->list->viewport()->mapToGlobal(point));
  auto *currentItem = ui->list->currentItem();
  QString label = currentItem->text();
  const auto i = label.indexOf(' ');
  if (i == -1) {
    return;
  }
  info(label.left(i));
  QBluetoothAddress address(label.left(i));
  if (action == pairAction) {
    localDevice->requestPairing(address, QBluetoothLocalDevice::Paired);
  } else if (action == unpairAction) {
    localDevice->requestPairing(address, QBluetoothLocalDevice::Unpaired);
  } else if (action == connectAction) {
    connectDevice(address);
  }
}

void BluetoothDialog::hostModeStateChanged(
    QBluetoothLocalDevice::HostMode mode) {}

void BluetoothDialog::startScan() { discoveryAgent->start(); }

const QBluetoothUuid WRITE_CHAR(quint16(0xffd9));
const QBluetoothUuid STATUS_CHAR(quint16(0xffd4));

const QBluetoothUuid WRITE_SERVICE(quint16(0xffd5));
const QBluetoothUuid READ_SERVICE(quint16(0xffd0));

void BluetoothDialog::connectDevice(QBluetoothAddress address) {
  auto device = devices.find(address);
  if (device == devices.end()) {
    error("missing device with address " + address.toString());
    return;
  }
  auto controller = QLowEnergyController::createCentral(device->second);
  connect(controller, &QLowEnergyController::errorOccurred, this,
          [this](QLowEnergyController::Error err) {
            Q_UNUSED(err);
            this->error("Cannot connect to remote device.");
          });
  connect(controller, &QLowEnergyController::serviceDiscovered, (this),
          [this, controller](const QBluetoothUuid &gatt) {
            auto *service = controller->createServiceObject(gatt);
            if (service->serviceUuid() == WRITE_SERVICE) {
              info("write service");
              connect(service, &QLowEnergyService::stateChanged, this,
                      &BluetoothDialog::turnOn);
              service->discoverDetails();
              this->writeService = service;
            }

            this->info(QString("Service: ") +
                       QString::number(gatt.toUInt32(), 16));
            auto chars = service->characteristics();
            connect(service, &QLowEnergyService::errorOccurred, this,
                    [this](QLowEnergyService::ServiceError error) {
                      this->error("Service error: " + QString::number(error));
                    });

            for (auto c : chars) {
              this->info(QString("Characteristic: ") + c.uuid().toString());
              if (c.uuid() == WRITE_CHAR) {
                this->writeChar = c;
              }
            }
          });

  connect(controller, &QLowEnergyController::connected, this,
          [this, controller]() {
            info("Controller connected. Search services...");
            controller->discoverServices();
          });
  connect(controller, &QLowEnergyController::disconnected, this,
          [this]() { error("LowEnergy controller disconnected"); });
  controller->connectToDevice();
}
void BluetoothDialog::turnOn() {
  auto service = this->writeService;
  if (!service) {
    error("missing service");
    return;
  }
  this->info("service state: " + QString::number(service->state()));
  if (service->state() == QLowEnergyService::RemoteServiceDiscovered &&
      writeChar.isValid()) {
    sleep(1);
    this->info("trying to turn on");
    info("character: " + writeChar.uuid().toString());
    info("properties: 0x" + QString::number(writeChar.properties(), 16));
    const char bytes[] = {'\xCC', '\x24', '\x33'};
    auto ba = QByteArray(bytes, 3);
    service->writeCharacteristic(writeChar, ba,
                                 QLowEnergyService::WriteWithoutResponse);
    info("done");
  } else {
    error("missing service or characteristic");
  }
}

void BluetoothDialog::error(QString message) {
  cerr << message.toStdString() << std::endl;
}
void BluetoothDialog::info(QString message) {
  cout << message.toStdString() << std::endl;
}