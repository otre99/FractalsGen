#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

#include "colormapping.h"
class DisplayWidget;
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

 private slots:
  void UpdateAllParameters();
  void UpdateAllParametersAndRender();
  void on_comboBoxCmaps_currentTextChanged(const QString &arg1);
  void on_checkBoxLogScale_clicked(bool checked);
  void on_pushButtonResetArea_clicked();
  void on_comboBoxFamily_activated(int index);
  void on_pBSaveRawData_clicked();
  void on_horizontalSlider_valueChanged(int value);

  void on_checkBoxInvert_clicked(bool checked);

  private:
  void SetUp();
  Ui::MainWindow *ui;
  DisplayWidget *displayWidget{nullptr};
  QMap<QString, ColorMapper::GradientPreset> name2gp;
};
#endif  // MAINWINDOW_H
