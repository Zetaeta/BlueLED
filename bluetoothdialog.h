#ifndef BLUETOOTHDIALOG_H
#define BLUETOOTHDIALOG_H

#include <QDialog>
#include <QLowEnergyCharacteristic>
#include <map>
#include <qbluetoothlocaldevice.h>

namespace Ui {
class BluetoothDialog;
}

QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceDiscoveryAgent)
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)
QT_FORWARD_DECLARE_CLASS(QLowEnergyService)
QT_FORWARD_DECLARE_CLASS(QLowEnergyCharacteristic)

class BluetoothDialog : public QDialog {
  Q_OBJECT

public:
  explicit BluetoothDialog(QWidget *parent = nullptr);
  ~BluetoothDialog();
public slots:
  void addDevice(const QBluetoothDeviceInfo &device);
  void startScan();
  void finishedScan();
  void showPairingMenu(const QPoint &point);
  void hostModeStateChanged(QBluetoothLocalDevice::HostMode mode);
  void connectDevice(QBluetoothAddress address);
  void error(QString message);
  void info(QString message);
  void turnOn();

private:
  Ui::BluetoothDialog *ui;
  QBluetoothDeviceDiscoveryAgent *discoveryAgent;
  QBluetoothLocalDevice *localDevice;
  std::map<QBluetoothAddress, QBluetoothDeviceInfo> devices;
  QLowEnergyService *writeService;
  QLowEnergyCharacteristic writeChar;
};

#endif // BLUETOOTHDIALOG_H
