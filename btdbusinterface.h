#ifndef BTDBUSINTERFACE_H
#define BTDBUSINTERFACE_H

#define SERVICE_NAME "net.zetaeta.BtLed.Controller"

#include <QDBusAbstractAdaptor>
#include <QObject>
QT_FORWARD_DECLARE_CLASS(BluetoothInterface)

class BtDBusInterface : public QDBusAbstractAdaptor {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)
public:
  explicit BtDBusInterface(BluetoothInterface *bt, QObject *parent = nullptr);

public slots:
  Q_NOREPLY void powerOn();
  Q_NOREPLY void powerOff();
  Q_NOREPLY void rgb(int r, int g, int b);
  Q_NOREPLY void setMode(int modeNo, int speed = 100);

private:
  BluetoothInterface *bt;
};

#endif // BTDBUSINTERFACE_H