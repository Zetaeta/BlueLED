#ifndef BLUETOOTHINTERFACE_H
#define BLUETOOTHINTERFACE_H

#include <QByteArray>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyService>
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
  void debug(const QString &message);
  bool isReady();
  static constexpr unsigned char modes[] = {37, 38, 39, 40, 41, 42, 43, 44,
                                            45, 46, 47, 48, 49, 50, 51, 52,
                                            53, 54, 55, 56, 97, 98, 99};
  static constexpr size_t numModes = sizeof(modes);
  enum ConnectionState { Connected = 0, Connecting = 1, Error = 2 };

signals:
  void stateChanged(ConnectionState state);
public slots:
  void setColor(const QColor &color);
  void powerOn();
  void powerOff();
  void status();
  void writeStateChanged(QLowEnergyService::ServiceState state);
  void readStateChanged(QLowEnergyService::ServiceState state);
  void receiveStatus(const QLowEnergyCharacteristic &characteristic,
                     const QByteArray &newValue);
  void builtInMode(quint8 mode, quint8 speed);
  void setModeNo(quint8 index, quint8 speed);
  void setMusicColorMode(bool mode);
  void setMusicMode(int mode);

private:
  bool ready = false;
  bool musicMode = false;
  QLowEnergyService *writeService;
  QLowEnergyService *readService;
  QLowEnergyController *controller;
  QLowEnergyCharacteristic writeChar;
  void writeMessage(const QByteArray &bytes);
  template <size_t N> inline void writeMessage(const char (&bytes)[N]) {
    auto qb = QByteArray(bytes, N);
    writeMessage(qb);
  }
};

#endif // BLUETOOTHINTERFACE_H
