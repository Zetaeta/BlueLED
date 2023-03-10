#include "daemon.h"
#include "btdbusinterface.h"
#include <QBluetoothDeviceDiscoveryAgent>
#include <QDBusConnection>
#include <QSettings>
#include <QtDebug>
#define NAME "BTLed"
Daemon::Daemon(QObject *parent)
    : QObject(parent), localDevice(new QBluetoothLocalDevice(this)),
      discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this)),
      settings(NAME, NAME) {}

// void Daemon::connect(const QString &address) {}
void Daemon::initialize() {
  connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
          this, &Daemon::deviceDiscovered);
  connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this,
          &Daemon::finishedScan);
  auto address = settings.value("address");
  if (address.isNull()) {
#ifdef DEFAULT_ADDRESS
    this->address = DEFAULT_ADDRESS;
#else
    this->address = "none";
#endif
    settings.setValue("address", this->address);
  } else {
    this->address = address.toString();
  }
  discoveryAgent->start();
}

void Daemon::deviceDiscovered(const QBluetoothDeviceInfo &info) {
  auto deviceAddress = info.address();
  qInfo() << "found device with address " << deviceAddress;
  if (!deviceAddress.toString().contains(this->address)) {
    return;
  }
  if (interface != nullptr) {
    qWarning() << "already created interface";
  }
  qDebug() << "making interface";
  interface = std::make_unique<BluetoothInterface>(info);
  qDebug() << interface.get();
  auto dbusInt = new BtDBusInterface(interface.get(), this);
  QDBusConnection::sessionBus().registerObject("/", this);
  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
    qCritical("failed to register");
  }
}

void Daemon::finishedScan() { qDebug() << "scan finished"; }
