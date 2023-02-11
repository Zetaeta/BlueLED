#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bluetoothinterface.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
// QT_FORWARD_DECLARE_CLASS(BluetoothInterface)

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
public slots:
  void updateR(int r);
  void updateG(int g);
  void updateB(int b);
  void clickBtButton();
  void clickColorButton();
  void error(QString message);
  void info(QString message);
  void applyBuiltInMode();
  void applyMusicMode();
  void interfaceStatus(BluetoothInterface::ConnectionState);

private:
  Ui::MainWindow *ui;
  int r;
  int g;
  int b;
  void updateColor();
  std::unique_ptr<BluetoothInterface> interface;
  void enableControls();
};
#endif // MAINWINDOW_H
