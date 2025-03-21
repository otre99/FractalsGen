#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

#include "fractals.h"
#include "colormapping.h"

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog {
  Q_OBJECT

 public:
  explicit ExportDialog(QWidget *parent, FractalParameters *p);
  ~ExportDialog();
  void setBBox(const double &x1, const double &x2, const double &y1,
               const double &y2);
  double x1() const;
  double x2() const;
  double y1() const;
  double y2() const;

  void setColorMapParameters(ColorMapper *colorMapper, bool useLog, double offset);
  void setBBoxSize(const QSize &size);
  QSize bboxSize() const;

 private slots:
  void on_spinBoxW_valueChanged(int arg1);
  void on_pushButton_clicked();

  void on_checkBox_clicked(bool checked);

  void on_pushButtonImage_clicked();

  private:
  Ui::ExportDialog *ui;
  double aspectRatio{1.0};
  FractalParameters *params;
  ColorMapper *colorMapper;
  bool useLog;
  double offset;
};

#endif  // EXPORTDIALOG_H
