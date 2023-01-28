#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "bluetoothdialog.h"
#include <QColorDialog>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->inputR, &QSpinBox::valueChanged, this, &MainWindow::updateR);
  connect(ui->inputG, &QSpinBox::valueChanged, this, &MainWindow::updateG);
  connect(ui->inputB, &QSpinBox::valueChanged, this, &MainWindow::updateB);
  connect(ui->btDialogButton, &QPushButton::clicked, this,
          &MainWindow::clickBtButton);
  connect(ui->selectColorButton, &QPushButton::clicked, this,
          &MainWindow::clickColorButton);
  r = g = b = 0;
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::updateR(int r) {
  this->r = r;
  updateColor();
}

void MainWindow::updateG(int g) {
  this->g = g;
  updateColor();
}
void MainWindow::updateB(int b) {
  this->b = b;
  updateColor();
}

void MainWindow::updateColor() {
  auto button = ui->selectColorButton;
  QPalette pal = ui->selectColorButton->palette();
  pal.setColor(QPalette::Button, QColor(r, g, b));
  button->setAutoFillBackground(true);
  button->setPalette(pal);
  button->update();
}

void MainWindow::clickBtButton() {
  auto *dialog = new BluetoothDialog;
  dialog->setWindowTitle("bluetooth");
  auto result = dialog->exec();
  if (result != QDialog::Accepted) {
    return;
  }
  interface = dialog->getInterface();
  if (interface == nullptr) {
    error("No interface");
    return;
  }
  auto interface = this->interface.get();
  ui->controls->setEnabled(true);
  connect(ui->onButton, &QAbstractButton::clicked, interface,
          &BluetoothInterface::powerOn);
  connect(ui->offButton, &QAbstractButton::clicked, interface,
          &BluetoothInterface::powerOff);
}

void MainWindow::clickColorButton() {
  auto *dialog = new QColorDialog(this);
  connect(dialog, &QColorDialog::colorSelected, interface.get(),
          &BluetoothInterface::setColor);
  connect(dialog, &QColorDialog::currentColorChanged, interface.get(),
          &BluetoothInterface::setColor);
  dialog->show();
}

void MainWindow::error(QString message) {
  std::cerr << message.toStdString() << std::endl;
}
void MainWindow::info(QString message) {
  std::cout << message.toStdString() << std::endl;
}
