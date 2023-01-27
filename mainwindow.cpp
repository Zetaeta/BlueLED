#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "bluetoothdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->inputR, &QSpinBox::valueChanged, this, &MainWindow::updateR);
  connect(ui->inputG, &QSpinBox::valueChanged, this, &MainWindow::updateG);
  connect(ui->inputB, &QSpinBox::valueChanged, this, &MainWindow::updateB);
  connect(ui->selectColorButton, &QPushButton::clicked, this,
          &MainWindow::clickButton);
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

void MainWindow::clickButton() {
  auto *dialog = new BluetoothDialog;
  dialog->setWindowTitle("bluetooth");
  dialog->show();
}