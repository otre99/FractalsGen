#ifndef DISPLAY_WIDGET_H
#define DISPLAY_WIDGET_H

#include <QGestureEvent>
#include <QPixmap>
#include <QWidget>

#include "colormapping.h"
#include "renderthread.h"

class DisplayWidget : public QWidget {
  Q_OBJECT
  friend class MainWindow;
  struct ImageData {
    QVector<double> data;
    QSize size;
    double minVal, maxVal;
    QPixmap GenPixmap(ColorMapper &cmap, bool useLog, bool calc_bound = true,
                      double offset = 0.0);

  } imgData;

 public:
  DisplayWidget(QWidget *parent = nullptr);

 protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
#if QT_CONFIG(wheelevent)
  void wheelEvent(QWheelEvent *event) override;
#endif
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void Reset();

  void setLogScale(bool enable);
  void setColorMap(ColorMapper::GradientPreset p);
  void invertColorMap();
  void setColorMapOffset(const double &offset);
  double x1() const { return centerX - width() * curScale; }
  double y1() const { return centerY - height() * curScale; }
  double x2() const { return centerX + width() * curScale; }
  double y2() const { return centerY + height() * curScale; }
  ColorMapper *colorMap() { return &colorMapper; }
  bool useLogScale() const { return useLog; }
  bool colorOffset() const { return colorMapOffset; }

#ifndef QT_NO_GESTURES
  bool event(QEvent *event) override;
#endif

 private slots:
  void updatePixmap(const QVector<double> &data, const QSize &size,
                    double scaleFactor);
  void zoom(double zoomFactor);

 private:
  void scroll(int deltaX, int deltaY);
#ifndef QT_NO_GESTURES
  bool gestureEvent(QGestureEvent *event);
#endif
  void RenderCommand();
  void colorize();
  RenderThread thread;
  QPixmap pixmap;
  QPoint pixmapOffset;
  QPoint lastDragPos;
  double centerX;
  double centerY;
  double pixmapScale;
  double curScale;
  FractalParameters fractalParams;
  ColorMapper colorMapper;
  bool useLog{false};
  QString help, info;
  double colorMapOffset{0.0};
};
//! [0]

#endif  // DISPLAY_WIDGET_H
