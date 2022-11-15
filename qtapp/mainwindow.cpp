#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "display_widget.h"
#include "exportdialog.h"
#include <QDebug>

template <typename Type> double GetNumber(Type *l) {
  return l->text().toDouble();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setCentralWidget(displayWidget = new DisplayWidget());
  SetUp();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::SetUp() {
  connect(ui->leCImg, &QLineEdit::editingFinished, this,
          &MainWindow::UpdateAllParametersAndRender);
  connect(ui->leCReal, &QLineEdit::editingFinished, this,
          &MainWindow::UpdateAllParametersAndRender);
  connect(ui->leQImg, &QLineEdit::editingFinished, this,
          &MainWindow::UpdateAllParametersAndRender);
  connect(ui->leQReal, &QLineEdit::editingFinished, this,
          &MainWindow::UpdateAllParametersAndRender);
  connect(ui->leOrbitReal, &QLineEdit::editingFinished, this,
          &MainWindow::UpdateAllParametersAndRender);
  connect(ui->leOrbitImg, &QLineEdit::editingFinished, this,
          &MainWindow::UpdateAllParametersAndRender);
  connect(ui->leRadius, &QLineEdit::editingFinished, this,
          &MainWindow::UpdateAllParametersAndRender);

  connect(ui->checkBoxMandelbrot, &QCheckBox::clicked, this,
          &MainWindow::UpdateAllParametersAndRender);
  connect(ui->checkBoxOrbitTrap, &QCheckBox::clicked, this,
          &MainWindow::UpdateAllParametersAndRender);

  connect(ui->spinBoxMaxIters, &QSpinBox::valueChanged, this,
          &MainWindow::UpdateAllParametersAndRender);
  connect(ui->spinBoxN, &QSpinBox::valueChanged, this,
          &MainWindow::UpdateAllParametersAndRender);

  connect(ui->comboBoxOrbitType, &QComboBox::activated, this,
          &MainWindow::UpdateAllParametersAndRender);

  name2gp["gpGrayscale"] = ColorMapper::gpGrayscale;
  name2gp["gpHot"] = ColorMapper::gpHot;
  name2gp["gpCold"] = ColorMapper::gpCold;
  name2gp["gpNight"] = ColorMapper::gpNight;
  name2gp["gpCandy"] = ColorMapper::gpCandy;
  name2gp["gpGeography"] = ColorMapper::gpGeography;
  name2gp["gpIon"] = ColorMapper::gpIon;
  name2gp["gpThermal"] = ColorMapper::gpThermal;
  name2gp["gpPolar"] = ColorMapper::gpPolar;
  name2gp["gpSpectrum"] = ColorMapper::gpSpectrum;
  name2gp["gpJet"] = ColorMapper::gpJet;
  name2gp["gpHues"] = ColorMapper::gpHues;
  UpdateAllParameters();
  on_checkBoxLogScale_clicked(ui->checkBoxLogScale->isChecked());
  on_comboBoxCmaps_currentTextChanged(ui->comboBoxCmaps->currentText());
  on_comboBoxFamily_activated(ui->comboBoxFamily->currentIndex());
}

void MainWindow::UpdateAllParametersAndRender() {
  UpdateAllParameters();
  displayWidget->RenderCommand();
}

void MainWindow::UpdateAllParameters() {
  displayWidget->fractalParams.fractal_family =
      ui->comboBoxFamily->currentIndex();
  displayWidget->fractalParams.c = {GetNumber(ui->leCReal),
                                    GetNumber(ui->leCImg)};
  displayWidget->fractalParams.q = {GetNumber(ui->leQReal),
                                    GetNumber(ui->leQImg)};
  displayWidget->fractalParams.orbit_pt = {GetNumber(ui->leOrbitReal),
                                           GetNumber(ui->leOrbitImg)};
  displayWidget->fractalParams.mandelbrot = ui->checkBoxMandelbrot->isChecked();
  displayWidget->fractalParams.orbit_trap =
      ui->checkBoxOrbitTrap->isChecked() ? 1 : 0;
  displayWidget->fractalParams.max_norm = GetNumber(ui->leRadius);
  displayWidget->fractalParams.max_iterations = ui->spinBoxMaxIters->value();
  displayWidget->fractalParams.n = ui->spinBoxN->value();
  displayWidget->fractalParams.orbit_mode =
      ui->comboBoxOrbitType->currentIndex();
}

void MainWindow::on_comboBoxCmaps_currentTextChanged(const QString &arg1) {
  displayWidget->setColorMap(name2gp[arg1]);
}

void MainWindow::on_checkBoxLogScale_clicked(bool checked) {
  displayWidget->setLogScale(checked);
}

void MainWindow::on_pushButtonResetArea_clicked() { displayWidget->Reset(); }

void MainWindow::on_comboBoxFamily_activated(int index) {
  switch (index) {
  case 0:
    ui->labelFamily->setPixmap(QPixmap(":/images/images/family00.jpg"));
    break;
  case 1:
    ui->labelFamily->setPixmap(QPixmap(":/images/images/family01.jpg"));
    break;
  case 2:
    ui->labelFamily->setPixmap(QPixmap(":/images/images/family02.jpg"));
    break;
  case 3:
    ui->labelFamily->setPixmap(QPixmap(":/images/images/family03.jpg"));
    break;
  case 4:
    ui->labelFamily->setPixmap(QPixmap(":/images/images/family04.jpg"));
    break;
  default:
    qErrnoWarning(
        "Wrong fractal family number. Expected [0,1,2,3,4], but get %d", index);
    break;
  }
  displayWidget->fractalParams.fractal_family = index;
  displayWidget->RenderCommand();
}

void MainWindow::on_pBSaveRawData_clicked() {
  ExportDialog dlg(this, &(displayWidget->fractalParams));
  dlg.setBBox(displayWidget->x1(), displayWidget->x2(), displayWidget->y1(),
              displayWidget->y2());
  dlg.setBBoxSize(displayWidget->size());
  dlg.exec();
}
