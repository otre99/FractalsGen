#include "display_widget.h"

#include <math.h>

#include <QDebug>
#include <QGesture>
#include <QKeyEvent>
#include <QPainter>

const double DefaultCenterX = -0.637011;
const double DefaultCenterY = -0.0395159;
const double DefaultScale = 0.00403897;

const double ZoomInFactor = 0.8;
const double ZoomOutFactor = 1 / ZoomInFactor;
const int ScrollStep = 20;

QPixmap DisplayWidget::ImageData::GenPixmap(ColorMapper &cmap, bool useLog,
                                            bool calc_bound, double offset) {
  if (data.empty()) return {};
  if (calc_bound) {
    minVal = std::numeric_limits<double>::max();
    maxVal = std::numeric_limits<double>::min();
    for (const auto &v : std::as_const(data)) {
      minVal = std::min(minVal, v);
      maxVal = std::max(maxVal, v);
    }
  }

  QImage image(size, QImage::Format_ARGB32);
  int dim = size.width();

  const double o = (maxVal - minVal) * offset;
  for (int i = 0; i < image.height(); ++i) {
    cmap.colorize(data.data() + i * dim, minVal + o, maxVal + o,
                  reinterpret_cast<QRgb *>(image.scanLine(i)), dim, 1, useLog);
  }
  return QPixmap::fromImage(image);
}

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent),
      centerX(DefaultCenterX),
      centerY(DefaultCenterY),
      pixmapScale(DefaultScale),
      curScale(DefaultScale) {
  connect(&thread, &RenderThread::renderedImage, this,
          &DisplayWidget::updatePixmap);
#if QT_CONFIG(cursor)
  setCursor(Qt::CrossCursor);
#endif

  setColorMap(ColorMapper::gpGrayscale);
  setLogScale(false);
  help =
      tr("Zoom with mouse wheel, +/- keys or pinch.  Scroll with arrow keys "
         "or by dragging.");
}

void DisplayWidget::Reset() {
  centerX = DefaultCenterX;
  centerY = DefaultCenterY;
  pixmapScale = DefaultScale;
  curScale = DefaultScale;
  RenderCommand();
}

void DisplayWidget::setLogScale(bool enable) {
  if (useLog == enable) return;
  useLog = enable;
  pixmap = imgData.GenPixmap(colorMapper, useLog, false, colorMapOffset);
  update();
}

void DisplayWidget::setColorMap(ColorMapper::GradientPreset p) {
  if (p == colorMapper.preset()) return;
  colorMapper = ColorMapper(p);
  colorMapper.setPeriodic(true);
  pixmap = imgData.GenPixmap(colorMapper, useLog, false, colorMapOffset);
  update();
}

void DisplayWidget::invertColorMap(){
    colorMapper = colorMapper.inverted();
    pixmap = imgData.GenPixmap(colorMapper, useLog, false, colorMapOffset);
    update();
}

void DisplayWidget::setColorMapOffset(const double &offset) {
  colorMapOffset = offset;
  pixmap = imgData.GenPixmap(colorMapper, useLog, false, colorMapOffset);
  update();
}

void DisplayWidget::paintEvent(QPaintEvent * /* event */) {
  QPainter painter(this);
  painter.fillRect(rect(), Qt::black);
  if (pixmap.isNull()) {
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap,
                     tr("Rendering initial image, please wait..."));
    return;
  }
  if (qFuzzyCompare(curScale, pixmapScale)) {
    painter.drawPixmap(pixmapOffset, pixmap);
  } else {
    auto previewPixmap = qFuzzyCompare(pixmap.devicePixelRatio(), qreal(1))
                             ? pixmap
                             : pixmap.scaled(pixmap.size(), Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
    double scaleFactor = pixmapScale / curScale;
    int newWidth = int(previewPixmap.width() * scaleFactor);
    int newHeight = int(previewPixmap.height() * scaleFactor);
    int newX = pixmapOffset.x() + (previewPixmap.width() - newWidth) / 2;
    int newY = pixmapOffset.y() + (previewPixmap.height() - newHeight) / 2;

    painter.save();
    painter.translate(newX, newY);
    painter.scale(scaleFactor, scaleFactor);
    QRectF exposed =
        painter.transform().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
    painter.drawPixmap(exposed, previewPixmap, exposed);
    painter.restore();
  }

  QFontMetrics metrics = painter.fontMetrics();
  if (!info.isEmpty()) {
    int infoWidth = metrics.horizontalAdvance(info);
    int infoHeight = metrics.height();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 127));
    infoHeight = (infoWidth / width() + 1) * (infoHeight + 5);
    painter.drawRect((width() - infoWidth) / 2 - 5, 0, infoWidth + 10,
                     infoHeight);

    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
                     info);
  }

  int helpWidth = metrics.horizontalAdvance(help);
  int helpHeight = metrics.height();

  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, 127));
  helpHeight = (helpWidth / width() + 1) * (helpHeight + 5);
  painter.drawRect((width() - helpWidth) / 2 - 5, height() - helpHeight,
                   helpWidth + 10, helpHeight);

  painter.setPen(Qt::white);
  painter.drawText(rect(),
                   Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, help);
}

void DisplayWidget::resizeEvent(QResizeEvent * /* event */) { RenderCommand(); }

void DisplayWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Plus:
      zoom(ZoomInFactor);
      break;
    case Qt::Key_Minus:
      zoom(ZoomOutFactor);
      break;
    case Qt::Key_Left:
      scroll(-ScrollStep, 0);
      break;
    case Qt::Key_Right:
      scroll(+ScrollStep, 0);
      break;
    case Qt::Key_Down:
      scroll(0, -ScrollStep);
      break;
    case Qt::Key_Up:
      scroll(0, +ScrollStep);
      break;
    case Qt::Key_Q:
      close();
      break;
    default:
      QWidget::keyPressEvent(event);
  }
}

#if QT_CONFIG(wheelevent)
void DisplayWidget::wheelEvent(QWheelEvent *event) {
  const int numDegrees = event->angleDelta().y() / 8;
  const double numSteps = numDegrees / double(15);
  zoom(pow(ZoomInFactor, numSteps));
}
#endif

void DisplayWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    lastDragPos = event->position().toPoint();
}

void DisplayWidget::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    pixmapOffset += event->position().toPoint() - lastDragPos;
    lastDragPos = event->position().toPoint();
    update();
  }
}

void DisplayWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    pixmapOffset += event->position().toPoint() - lastDragPos;
    lastDragPos = QPoint();
    const auto pixmapSize = pixmap.size();
    int deltaX = (width() - pixmapSize.width()) / 2 - pixmapOffset.x();
    int deltaY = (height() - pixmapSize.height()) / 2 - pixmapOffset.y();
    scroll(deltaX, deltaY);
  }
}

void DisplayWidget::updatePixmap(const QVector<double> &data, const QSize &size,
                                 double scaleFactor) {
  if (!lastDragPos.isNull()) return;
  imgData.data = data;
  imgData.size = size;
  pixmap = imgData.GenPixmap(colorMapper, useLog, true, colorMapOffset);
  pixmapOffset = QPoint();
  lastDragPos = QPoint();
  pixmapScale = scaleFactor;
  info.clear();
  update();
}

void DisplayWidget::zoom(double zoomFactor) {
  curScale *= zoomFactor;
  RenderCommand();
}

void DisplayWidget::RenderCommand() {
  info = "Rendering...";
  update();
  fractalParams.centerX = centerX;
  fractalParams.centerY = centerY;
  fractalParams.scale = curScale;
  fractalParams.image_size = size();
  thread.render(fractalParams);  // centerX, centerY, curScale, size(),
                                 // devicePixelRatio());
}

void DisplayWidget::scroll(int deltaX, int deltaY) {
  centerX += deltaX * curScale;
  centerY += deltaY * curScale;
  RenderCommand();
}

#ifndef QT_NO_GESTURES
bool DisplayWidget::gestureEvent(QGestureEvent *event) {
  if (auto *pinch =
          static_cast<QPinchGesture *>(event->gesture(Qt::PinchGesture))) {
    if (pinch->changeFlags().testFlag(QPinchGesture::ScaleFactorChanged))
      zoom(1.0 / pinch->scaleFactor());
    return true;
  }
  return false;
}

bool DisplayWidget::event(QEvent *event) {
  if (event->type() == QEvent::Gesture)
    return gestureEvent(static_cast<QGestureEvent *>(event));
  return QWidget::event(event);
}
#endif
