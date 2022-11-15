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
    QPixmap GenPixmap(ColorMapper &cmap, bool useLog, bool calc_bound = true) {
      if (data.empty())
        return {};
      if (calc_bound) {
        minVal = std::numeric_limits<double>::max();
        maxVal = std::numeric_limits<double>::min();
        for (const auto &v : data) {
          minVal = std::min(minVal, v);
          maxVal = std::max(maxVal, v);
        }
      }
      QImage image(size, QImage::Format_ARGB32);
      int dim = size.width();
      for (int i = 0; i < image.height(); ++i) {
        cmap.colorize(data.data() + i * dim, minVal, maxVal,
                      reinterpret_cast<QRgb *>(image.scanLine(i)), dim, 1,
                      useLog);
      }
      return QPixmap::fromImage(image);
    }

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

  void setLogScale(bool enable) {
    if (useLog == enable)
      return;
    useLog = enable;
    pixmap = imgData.GenPixmap(colorMapper, useLog, false);
    update();
  }

  void setColorMap(ColorMapper::GradientPreset p) {
    if (p == colorMapper.preset())
      return;
    colorMapper = ColorMapper(p);
    pixmap = imgData.GenPixmap(colorMapper, useLog, false);
    update();
  }

  double x1() const { return centerX-width()*curScale;}
  double y1() const { return centerY-height()*curScale;}
  double x2() const { return centerX+width()*curScale;}
  double y2() const { return centerY+height()*curScale;}


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
};
//! [0]

#endif // DISPLAY_WIDGET_H
