#include "btdbusinterface.h"
#include "bluetoothinterface.h"

BtDBusInterface::BtDBusInterface(BluetoothInterface *bt, QObject *parent)
    : QDBusAbstractAdaptor{parent}, bt(bt) {}

void BtDBusInterface::powerOn() {
  bt->info("bus");
  bt->powerOn();
}

void BtDBusInterface::powerOff() { bt->powerOff(); }

void BtDBusInterface::rgb(int r, int g, int b) {
  if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
    bt->error("invalid color");
    return;
  }
  bt->setRgb(r, g, b);
}

void BtDBusInterface::setMode(int mode, int speed) {
  bt->setModeNo(mode, speed);
}
