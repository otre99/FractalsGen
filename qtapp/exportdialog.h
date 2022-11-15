#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include "fractals.h"
#include <QDialog>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent, FractalParameters *p);
    ~ExportDialog();
    void setBBox(const double &x1, const double &x2, const double &y1, const double &y2);
    double x1() const;
    double x2() const;
    double y1() const;
    double y2() const;

    void setBBoxSize(const QSize &size);
    QSize bboxSize() const;

private slots:
    void on_spinBoxW_valueChanged(int arg1);
    void on_pushButton_clicked();

    void on_checkBox_clicked(bool checked);

private:
    Ui::ExportDialog *ui;
    double aspectRatio{1.0};
    FractalParameters *params;
};

#endif // EXPORTDIALOG_H
