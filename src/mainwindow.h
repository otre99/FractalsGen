#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "renderthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_doubleSpinBoxMaxNorm_valueChanged(double arg1);
    void on_lineEditCx_textEdited(const QString &arg1);
    void on_lineEditCy_textEdited(const QString &arg1);
    void Render();
    void on_pushButtonRescale_clicked();
    void on_lineEditQx_textEdited(const QString &arg1);
    void on_checkBoxLog_clicked();
    void on_checkBoxPeriod_clicked();
    void on_pushButtonSaveParams_clicked();
    void on_comboBoxCMaps_activated(const QString &arg1);
    void on_comboBoxRanderModel_activated(int index);
    void on_pushButton_clicked();
    void on_horizontalSliderHi_doubleValueMoved(double val);
    void on_horizontalSliderLo_doubleValueMoved(double val);
    void on_pushButtonPNG_clicked();
    void on_checkBoxMandelbrot_clicked();
    void on_checkBoxInverted_clicked();
    void on_comboBoxCouloring_activated(int index);
    void on_lineEditOrbitX_textEdited(const QString &arg1);
    void on_lineEditOrbitY_textEdited(const QString &arg1);
    void on_comboBoxOrbitType_activated(int index);
    void on_pushButtonAdjust_clicked();
    void on_lineEditQy_textEdited(const QString &arg1);
    void on_comboBoxFamily_activated(int index);
    void on_comboBoxFamily_currentIndexChanged(int index);
    void on_spinBoxN_valueChanged(int arg1);
    void on_spinBoxMaxIters_valueChanged(int arg1);
    void on_checkBoxRegionSelection_clicked(bool checked);

private:
    void SaveRelevantParameters(QString &path);
    void LoadRelevantParameters(QString &path);
    QSize GetRenderImageSize();
    void RenderCommand(int tt);

    Ui::MainWindow *ui;
    QCustomPlot *m_custom_plot;
    QCPColorMap *m_color_map;
    void SetupColorMapPlot(QCustomPlot *customPlot);
    RenderThread rthread_;
    FractalParameters fparams_;
    void FillColormap(const QVector<float> &dd, const QRectF &area, int cols);
    void RenderedData(const QVector<float> &data, const QRectF &area, int cols);
    QCPRange last_XR_, last_YR_;
    bool update_from_saved_data_;
    QElapsedTimer elapsed_timer_;
};
#endif // MAINWINDOW_H
