#include "bluetoothdialog.h"
#include "ui_bluetoothdialog.h"

#include <QMenu>
#include <iostream>
#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <qlowenergycontroller.h>
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
  connect(ui->buttonBox, &QDialogButtonBox::accepted, (this),
          &BluetoothDialog::select);
  // connect(ui->turnonButton, &QAbstractButton::clicked, (this),
  // &BluetoothDialog::turnOn);
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

void BluetoothDialog::connectDevice(QBluetoothAddress address) {
  auto device = devices.find(address);
  if (device == devices.end()) {
    error("missing device with address " + address.toString());
    return;
  }
  interface = std::make_unique<BluetoothInterface>(device->second);
}

void BluetoothDialog::error(QString message) {
  cerr << message.toStdString() << std::endl;
}
void BluetoothDialog::info(QString message) {
  cout << message.toStdString() << std::endl;
}

std::unique_ptr<BluetoothInterface> &&BluetoothDialog::getInterface() {
  return std::move(interface);
}

void BluetoothDialog::select() {
  auto *item = ui->list->currentItem();
  if (item == nullptr) {
    error("no item selected");
    return;
  }
  QString label = item->text();
  const auto i = label.indexOf(' ');
  if (i == -1) {
    return;
  }
  info(label.left(i));
  QBluetoothAddress address(label.left(i));
  connectDevice(address);
}
