#ifndef DAEMON_H
#define DAEMON_H

#include "bluetoothinterface.h"
#include <QObject>
#include <QSettings>
#include <qbluetoothlocaldevice.h>
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceDiscoveryAgent)

class Daemon : public QObject {
  Q_OBJECT
public:
  explicit Daemon(QObject *parent = nullptr);
  // void connect(const QString &address);
  void initialize();
public slots:
  void deviceDiscovered(const QBluetoothDeviceInfo &info);
  void finishedScan();

private:
  QBluetoothLocalDevice *localDevice;
  QBluetoothDeviceDiscoveryAgent *discoveryAgent;
  std::unique_ptr<BluetoothInterface> interface;
  QSettings settings;
  QString address;
};

#endif // DAEMON_H
