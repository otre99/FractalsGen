#include <execution>
#include "exportdialog.h"
#include "ui_exportdialog.h"
#include <QFileDialog>

ExportDialog::ExportDialog(QWidget *parent, FractalParameters *p)
    : QDialog(parent), params(p), ui(new Ui::ExportDialog) {
  ui->setupUi(this);
}

void ExportDialog::setBBox(const double &x1, const double &x2, const double &y1,
                           const double &y2) {
  ui->leX1->setText(QString::number(x1));
  ui->leX2->setText(QString::number(x2));
  ui->leY1->setText(QString::number(y1));
  ui->leY2->setText(QString::number(y2));
}

double ExportDialog::x1() const { return ui->leX1->text().toDouble(); }
double ExportDialog::x2() const { return ui->leX2->text().toDouble(); }
double ExportDialog::y1() const { return ui->leY1->text().toDouble(); }
double ExportDialog::y2() const { return ui->leY2->text().toDouble(); }

void ExportDialog::setBBoxSize(const QSize &size) {
  ui->spinBoxW->setValue(size.width());
  ui->spinBoxH->setValue(size.height());
  aspectRatio = static_cast<double>(size.height()) / size.width();
}

QSize ExportDialog::bboxSize() const {
  return QSize{ui->spinBoxW->value(), ui->spinBoxH->value()};
}

ExportDialog::~ExportDialog() { delete ui; }

void ExportDialog::on_spinBoxW_valueChanged(int arg1) {
  if (ui->checkBox->isChecked()) {
    ui->spinBoxH->setValue(static_cast<int>(arg1 * aspectRatio + 0.5));
  }
}

void ExportDialog::on_pushButton_clicked() {
  const QString fname = QFileDialog::getSaveFileName(this, "Save file");
  if (fname.isEmpty())
    return;

  ui->pushButton->setText("Wait");

  QFile ofile(fname);
  ofile.open(QIODevice::WriteOnly);

  auto fractal = Fractal::Create(params);
  auto funct =
      fractal->GetCouloringFunction(params->mandelbrot, params->orbit_trap);

  const int W = ui->spinBoxW->value();
  const int H = ui->spinBoxH->value();
  std::vector<int> indexs(H);
  std::iota(indexs.begin(), indexs.end(), 0);
  const dbltype x0 = x1();
  const dbltype y0 = y1();
  const dbltype dx = (x2() - x0) / (W - 1);
  const dbltype dy = (y2() - y0) / (H - 1);
  std::vector<double> data(W * H);
  std::for_each(std::execution::par_unseq, indexs.begin(), indexs.end(),
                [&](int i) {
                  double *d = &(data[i * W]);
                  dbltype yy = y0 + i * dy;
                  for (int k = 0; k < W; ++k) {
                    d[k] = funct({x0 + k * dx, yy});
                  }
                });
  int header[2] = {W, H};
  ofile.write(reinterpret_cast<char *>(header), 2 * sizeof(int));
  ofile.write(reinterpret_cast<char *>(data.data()),
              sizeof(double) * data.size());
  accept();
}

void ExportDialog::on_checkBox_clicked(bool checked) {
  ui->spinBoxH->setEnabled(!checked);
}
