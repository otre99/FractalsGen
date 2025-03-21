#include <execution>
#include "exportdialog.h"

#include <QFileDialog>

#include "ui_exportdialog.h"

namespace {

void applyLowPassFilter(std::vector<double> &input, int R, int C) {
  constexpr double gaussianKernel5x5[5][5] = {
      {1.0 / 273, 4.0 / 273, 7.0 / 273, 4.0 / 273, 1.0 / 273},
      {4.0 / 273, 16.0 / 273, 26.0 / 273, 16.0 / 273, 4.0 / 273},
      {7.0 / 273, 26.0 / 273, 41.0 / 273, 26.0 / 273, 7.0 / 273},
      {4.0 / 273, 16.0 / 273, 26.0 / 273, 16.0 / 273, 4.0 / 273},
      {1.0 / 273, 4.0 / 273, 7.0 / 273, 4.0 / 273, 1.0 / 273}};

  std::vector<double> output(input.size());

  auto getValue = [&](int r, int c) -> double {
    // if (r < 0 || r >= R || c < 0 || c >= C) return 0;
    r = std::max(0, std::min(R - 1, r));  // Mirror border handling
    c = std::max(0, std::min(C - 1, c));
    return input[r * C + c];
  };

  std::for_each(
      std::execution::par_unseq, std::begin(output), std::end(output),
      [&](double &out) {
        int idx = &out - &output[0];  // Get index from pointer arithmetic
        int r = idx / C;
        int c = idx % C;
        double sum = 0.0;
        for (int kr = -2; kr <= 2; ++kr) {
          for (int kc = -2; kc <= 2; ++kc) {
            sum += getValue(r + kr, c + kc) * gaussianKernel5x5[kr + 1][kc + 1];
          }
        }
        out = sum;
      });
  input.swap(output);
}

std::vector<double> genRawData(const FractalParameters *params, const int H,
                               const int W, const double x0, const double x1,
                               const double y0, const double y1, bool smoth) {
  auto fractal = Fractal::Create(params);
  auto funct =
      fractal->GetCouloringFunction(params->mandelbrot, params->orbit_trap);

  std::vector<int> indexs(H);
  std::iota(indexs.begin(), indexs.end(), 0);
  const dbltype dx = (x1 - x0) / (W - 1);
  const dbltype dy = (y1 - y0) / (H - 1);
  std::vector<double> data(W * H);
  std::for_each(std::execution::par_unseq, indexs.begin(), indexs.end(),
                [&](int i) {
                  double *d = &(data[i * W]);
                  dbltype yy = y0 + i * dy;
                  for (int k = 0; k < W; ++k) {
                    d[k] = funct({x0 + k * dx, yy});
                  }
                });
  if (smoth) {
    applyLowPassFilter(data, H, W);
  }
  return std::move(data);
}

}  // namespace

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

void ExportDialog::setColorMapParameters(ColorMapper *colorMapper, bool useLog,
                                         double offset) {
  this->colorMapper = colorMapper;
  this->useLog = useLog;
  this->offset = offset;
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
  if (fname.isEmpty()) return;

  ui->pushButton->setText("Wait");

  const int W = ui->spinBoxW->value();
  const int H = ui->spinBoxH->value();
  std::vector<double> data = genRawData(params, H, W, x1(), x2(), y1(), y2(),
                                        ui->checkBoxSmoth->isChecked());

  QFile ofile(fname);
  ofile.open(QIODevice::WriteOnly);

  int header[2] = {W, H};
  ofile.write(reinterpret_cast<char *>(header), 2 * sizeof(int));
  ofile.write(reinterpret_cast<char *>(data.data()),
              sizeof(double) * data.size());
  accept();
}

void ExportDialog::on_checkBox_clicked(bool checked) {
  ui->spinBoxH->setEnabled(!checked);
}

void ExportDialog::on_pushButtonImage_clicked() {
  const QString fname = QFileDialog::getSaveFileName(this, "Save file");
  if (fname.isEmpty()) return;

  ui->pushButton->setText("Wait");

  const int W = ui->spinBoxW->value();
  const int H = ui->spinBoxH->value();
  const std::vector<double> data = genRawData(
      params, H, W, x1(), x2(), y1(), y2(), ui->checkBoxSmoth->isChecked());

  const auto &&mm =
      std::minmax_element(std::execution::par_unseq, data.begin(), data.end());
  const double minVal = *(mm.first);
  const double maxVal = *(mm.second);
  const double o = (maxVal - minVal) * offset;
  QImage image({W, H}, QImage::Format_ARGB32);
  for (int i = 0; i < H; ++i) {
    colorMapper->colorize(data.data() + i * W, minVal + o, maxVal + o,
                          reinterpret_cast<QRgb *>(image.scanLine(i)), W, 1,
                          useLog);
  }
  bool ok = image.save(fname);
  accept();
}
