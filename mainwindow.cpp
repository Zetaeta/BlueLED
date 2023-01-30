#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "bluetoothdialog.h"
#include <QColorDialog>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->mode->setMaximum(BluetoothInterface::numModes - 1);
  ui->mode->setMinimum(0);
  ui->mode->setDisplayIntegerBase(10);
  ui->mode->setValue(0);
  connect(ui->btDialogButton, &QPushButton::clicked, this,
          &MainWindow::clickBtButton);
  connect(ui->selectColorButton, &QPushButton::clicked, this,
          &MainWindow::clickColorButton);
  connect(ui->applyBIButton, &QPushButton::clicked, this,
          &MainWindow::applyBuiltInMode);
  connect(ui->speed, &QSlider::valueChanged, this, [this](int value) {
    ui->speedDisplay->setText(QString::number(value) + " (0x" +
                              QString::number(value, 16) + ")");
  });
  QAbstractButton *musicModes[] = {ui->music1, ui->music2, ui->music3,
                                   ui->music4};
  for (int i = 0; i < 4; i++) {
    ui->musicGroup->setId(musicModes[i], i);
  }
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
  connect(ui->statusButton, &QAbstractButton::clicked, interface,
          &BluetoothInterface::status);
  connect(ui->musicMode, &QCheckBox::stateChanged, this,
          [interface, this](int state) {
            info("changing music mode state");
            if (state == Qt::Checked) {
              interface->setMusicColorMode(true);
            } else {
              interface->setMusicColorMode(false);
            }
          });
  connect(ui->applyMusic, &QPushButton::clicked, this,
          &MainWindow::applyMusicMode);
}

void MainWindow::clickColorButton() {
  auto *dialog = new QColorDialog(this);
  connect(dialog, &QColorDialog::colorSelected, interface.get(),
          &BluetoothInterface::setColor);
  connect(dialog, &QColorDialog::currentColorChanged, interface.get(),
          &BluetoothInterface::setColor);
  dialog->show();
}

void MainWindow::applyBuiltInMode() {
  int mode = ui->mode->value();
  assert((mode >= 0 && mode < BluetoothInterface::numModes));
  int speed = ui->speed->value();
  assert((speed > 0 && speed <= 255));
  if (interface == nullptr) {
    error("No interface");
    return;
  }
  interface->setModeNo(mode, speed);
}
void MainWindow::applyMusicMode() {
  auto checked = ui->musicGroup->checkedId();
  info("music mode " + QString::number(checked));
  if (checked != -1) {
    interface->setMusicMode(checked);
  }
}

void MainWindow::error(QString message) {
  std::cerr << message.toStdString() << std::endl;
}
void MainWindow::info(QString message) {
  std::cout << message.toStdString() << std::endl;
}
