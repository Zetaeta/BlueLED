#include "bluetoothinterface.h"
#include <QBluetoothDeviceInfo>
#include <QColor>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <iostream>
#include <unistd.h>
using std::cerr;
using std::cout;

const QBluetoothUuid WRITE_CHAR(quint16(0xffd9));
const QBluetoothUuid STATUS_CHAR(quint16(0xffd4));

const QBluetoothUuid WRITE_SERVICE(quint16(0xffd5));
const QBluetoothUuid READ_SERVICE(quint16(0xffd0));
BluetoothInterface::BluetoothInterface(QObject *parent) : QObject{parent} {}

BluetoothInterface::BluetoothInterface(QBluetoothDeviceInfo &device) {
  auto controller = QLowEnergyController::createCentral(device);
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
              this->writeService = service;
              // connect(service, &QLowEnergyService::stateChanged, this,
              // &BluetoothInterface::powerOn);
              info("discovering details");
              service->discoverDetails();
              info("discovering details");
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
void BluetoothInterface::writeMessage(QByteArray &bytes) {
  auto service = this->writeService;
  if (!service) {
    error("missing service");
    return;
  }
  this->info("service state: " + QString::number(service->state()));
  if (service->state() == QLowEnergyService::RemoteServiceDiscovered &&
      writeChar.isValid()) {
    // sleep(1);
    this->info("sending message");
    info("character: " + writeChar.uuid().toString());
    // info("properties: 0x" + QString::number(writeChar.properties(), 16));
    service->writeCharacteristic(writeChar, bytes,
                                 QLowEnergyService::WriteWithoutResponse);
    info("done");
  } else {
    error("missing service or characteristic");
  }
}
void BluetoothInterface::powerOn() {
  auto service = this->writeService;
  if (!service) {
    error("missing service");
    return;
  }
  this->info("service state: " + QString::number(service->state()));
  if (service->state() == QLowEnergyService::RemoteServiceDiscovered &&
      writeChar.isValid()) {
    // sleep(1);
    this->info("trying to turn on");
    info("character: " + writeChar.uuid().toString());
    info("properties: 0x" + QString::number(writeChar.properties(), 16));
    const char bytes[] = {'\xCC', '\x23', '\x33'};
    auto ba = QByteArray(bytes, 3);
    service->writeCharacteristic(writeChar, ba,
                                 QLowEnergyService::WriteWithoutResponse);
    info("done");
  } else {
    error("missing service or characteristic");
  }
}
void BluetoothInterface::powerOff() {
  auto service = this->writeService;
  if (!service) {
    error("missing service");
    return;
  }
  this->info("service state: " + QString::number(service->state()));
  if (service->state() == QLowEnergyService::RemoteServiceDiscovered &&
      writeChar.isValid()) {
    // sleep(1);
    this->info("trying to turn off");
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
void BluetoothInterface::setColor(const QColor &color) {

  info("Setting color: " + color.name());
  unsigned char r = color.red(), g = color.green(), b = color.blue();
  const char bytes[] = {'\x56', r, g, b, '\x00', '\xf0', '\xaa'};
  auto qb = QByteArray(bytes, sizeof(bytes));
  writeMessage(qb);
}
void BluetoothInterface::error(QString message) {
  cerr << message.toStdString() << std::endl;
}
void BluetoothInterface::info(QString message) {
  cout << message.toStdString() << std::endl;
}