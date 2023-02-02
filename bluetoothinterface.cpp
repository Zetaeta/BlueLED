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

void BluetoothInterface::debug(const QString &message) {
  cout << message.toStdString() << std::endl;
}

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
              connect(service, &QLowEnergyService::stateChanged, this,
                      &BluetoothInterface::writeStateChanged);
            } else if (service->serviceUuid() == READ_SERVICE) {
              this->readService = service;
              connect(service, &QLowEnergyService::stateChanged, this,
                      &BluetoothInterface::readStateChanged);
            }
            service->discoverDetails();
            info("discovering details");

            this->info(QString("Service: ") +
                       QString::number(gatt.toUInt32(), 16));
            auto chars = service->characteristics();
            connect(service, &QLowEnergyService::errorOccurred, this,
                    [this](QLowEnergyService::ServiceError error) {
                      this->error("Service error: " + QString::number(error));
                    });
            connect(service, &QLowEnergyService::characteristicChanged, this,
                    &BluetoothInterface::receiveStatus);

            for (auto c : chars) {
              this->info(QString("Characteristic: ") + c.uuid().toString());
              if (c.uuid() == WRITE_CHAR) {
                this->writeChar = c;
              } else if (c.uuid() == STATUS_CHAR) {
                info("STATUS_CHAR");
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
void BluetoothInterface::receiveStatus(
    const QLowEnergyCharacteristic &characteristic, const QByteArray &value) {
  debug("write to characteristic:" + characteristic.uuid().toString());
  if (value.size() != 12) {
    error("wrong status size");
    return;
  }
  QString lbl(
      "        magi,unv1,powr,mode,unv2,sped,red ,gren,blue,whit,unv3,magi");
  QString str("Status: ");
  for (char c : value) {
    str += "0x" + QString::number(quint8(c), 16).rightJustified(2, '0') + ",";
  }
  info(lbl);
  info(str);
  char v1 = value[1], power = value[2], mode = value[3], v2 = value[4],
       speed = value[5], r = value[6], g = value[7], b = value[8], w = value[9],
       v3 = value[10];
  if (v1 != '\xe3') {
    info("v1 different");
  }
  if (v2 != '\x20') {
    info("v2 different");
  }
  if (v3 != '\x03') {
    info("v3 different");
  }
}

void BluetoothInterface::writeStateChanged(
    QLowEnergyService::ServiceState state) {
  auto service = this->writeService;
  if (!service) {
    error("missing service");
    return;
  }
  // auto state = service->state();
  this->info("service state: " + QString::number(state));
  BluetoothInterface::ConnectionState this_state;
  if (state != QLowEnergyService::RemoteServiceDiscovered) {
    ready = false;
    info("not ready");
    if (state == QLowEnergyService::InvalidService) {
      this_state = BluetoothInterface::Error;
    } else {
      this_state = BluetoothInterface::Connecting;
    }
  } else {
    ready = true;
    this_state = BluetoothInterface::Connected;
  }
  emit stateChanged(this_state);
}
void BluetoothInterface::readStateChanged(
    QLowEnergyService::ServiceState state) {
  auto service = this->readService;
  if (!service) {
    error("missing read service");
    return;
  }
  this->info("read service state: " + QString::number(service->state()));
  if (service->state() != QLowEnergyService::RemoteServiceDiscovered) {
    info("not ready");
    return;
  }
  auto c = service->characteristic(STATUS_CHAR);
  auto cccd = c.clientCharacteristicConfiguration();
  if (!cccd.isValid()) {
    error("not valid");
    return;
  }
  service->writeDescriptor(cccd,
                           QLowEnergyCharacteristic::CCCDEnableNotification);
}
void BluetoothInterface::writeMessage(const QByteArray &bytes) {
  auto service = this->writeService;

  if (!service) {
    error("missing service");
    return;
  }
  if (service->state() == QLowEnergyService::RemoteServiceDiscovered &&
      writeChar.isValid()) {
    // sleep(1);
    debug("sending message");
    debug("character: " + writeChar.uuid().toString());
    // info("properties: 0x" + QString::number(writeChar.properties(), 16));
    service->writeCharacteristic(writeChar, bytes,
                                 QLowEnergyService::WriteWithoutResponse);
    debug("done");
  } else {
    error("missing service or characteristic");
    this->info("service state: " + QString::number(service->state()));
  }
}
void BluetoothInterface::powerOn() {

  const char bytes[] = {'\xCC', '\x23', '\x33'};
  writeMessage(bytes);
}
void BluetoothInterface::powerOff() {

  const char bytes[] = {'\xCC', '\x24', '\x33'};
  writeMessage(bytes);
}
void BluetoothInterface::setColor(const QColor &color) {

  info("Setting color: " + color.name());
  unsigned char r = color.red(), g = color.green(), b = color.blue();
  if (musicMode) {
    debug("music mode");
    const char bytes[] = {'\x78',
                          static_cast<char>(r),
                          static_cast<char>(g),
                          static_cast<char>(b),
                          '\x00',
                          '\xf0',
                          '\xee'};
    writeMessage(bytes);
    return;
  }
  const char bytes[] = {'\x56',
                        static_cast<char>(r),
                        static_cast<char>(g),
                        static_cast<char>(b),
                        '\x00',
                        '\xf0',
                        '\xaa'};
  auto qb = QByteArray(bytes, sizeof(bytes));
  writeMessage(qb);
}

void BluetoothInterface::status() {
  const char bytes[] = {'\xEF', '\x01', '\x77'};
  auto qb = QByteArray(bytes, sizeof(bytes));
  writeMessage(qb);
}

void BluetoothInterface::builtInMode(quint8 mode, quint8 speed) {
  info("built in mode 0x" + QString::number(mode, 16) + " with speed " +
       QString::number(speed));
  const char bytes[] = {'\xbb', static_cast<char>(mode),
                        static_cast<char>(speed), '\x44'};
  writeMessage(bytes);
}

void BluetoothInterface::setModeNo(quint8 index, quint8 speed) {
  assert(index < numModes);
  builtInMode(modes[index], speed);
}

void BluetoothInterface::setMusicColorMode(bool mode) { musicMode = mode; }
void BluetoothInterface::setMusicMode(int mode) {
  assert(mode >= 0 && mode < 4);
  const char bytes[] = {'\x01', '\xf0', '\x32', static_cast<char>(mode),
                        '\x00', '\x18'};
  writeMessage(bytes);
}

void BluetoothInterface::error(QString message) {
  cerr << message.toStdString() << std::endl;
}
void BluetoothInterface::info(QString message) {
  cout << message.toStdString() << std::endl;
}