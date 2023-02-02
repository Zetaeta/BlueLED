#include "btdbusinterface.h"
#include "bluetoothinterface.h"
#include <QColor>

BtDBusInterface::BtDBusInterface(BluetoothInterface *bt, QObject *parent)
    : QDBusAbstractAdaptor{parent}, bt(bt) {}

void BtDBusInterface::powerOn() {
  bt->info("bus");
  bt->powerOn();
}

void BtDBusInterface::powerOff() { bt->powerOff(); }

void BtDBusInterface::rgb(int r, int g, int b) {
  auto color = QColor(r, g, b);
  if (!color.isValid()) {
    bt->error("invalid color");
    return;
  }
  bt->setColor(color);
}

void BtDBusInterface::setMode(int mode, int speed) {
  bt->setModeNo(mode, speed);
}
