#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
public slots:
  void updateR(int r);
  void updateG(int g);
  void updateB(int b);
  void clickButton();

private:
  Ui::MainWindow *ui;
  int r;
  int g;
  int b;
  void updateColor();
};
#endif // MAINWINDOW_H
