#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H
#include <QDialog>
#include <qbluetoothlocaldevice.h>

namespace Ui {
class ConnectionDialog;
}
class ConnectionDialog : public QDialog {
  // Q_OBJECT
  ConnectionDialog(QWidget *parent = nullptr);

  ~ConnectionDialog();

private:
  Ui::ConnectionDialog *ui;
};

#endif
