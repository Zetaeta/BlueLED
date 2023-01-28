#ifndef BLUETOOTHINTERFACE_H
#define BLUETOOTHINTERFACE_H

#include <QLowEnergyCharacteristic>
#include <QObject>
QT_FORWARD_DECLARE_CLASS(QLowEnergyService)
QT_FORWARD_DECLARE_CLASS(QLowEnergyController)
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)

class BluetoothInterface : public QObject {
  Q_OBJECT
public:
  explicit BluetoothInterface(QObject *parent = nullptr);

  BluetoothInterface(QBluetoothDeviceInfo &info);

  void error(QString message);
  void info(QString message);

  // signals:
public slots:
  void setColor(const QColor &color);
  void powerOn();
  void powerOff();

private:
  QLowEnergyService *writeService;
  QLowEnergyController *controller;
  QLowEnergyCharacteristic writeChar;
  void writeMessage(QByteArray &bytes);
};

#endif // BLUETOOTHINTERFACE_H
