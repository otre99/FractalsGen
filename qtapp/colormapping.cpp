#include "colormapping.h"

#include <QDebug>

ColorMapper::ColorMapper()
    : mLevelCount(350), mColorInterpolation(ciRGB), mPeriodic(false),
      mColorBufferInvalidated(true) {
  mColorBuffer.fill(qRgb(0, 0, 0), mLevelCount);
}

ColorMapper::ColorMapper(GradientPreset preset)
    : mLevelCount(350), mColorInterpolation(ciRGB), mPeriodic(false),
      mColorBufferInvalidated(true) {
  mColorBuffer.fill(qRgb(0, 0, 0), mLevelCount);
  loadPreset(preset);
}

bool ColorMapper::operator==(const ColorMapper &other) const {
  return ((other.mLevelCount == this->mLevelCount) &&
          (other.mColorInterpolation == this->mColorInterpolation) &&
          (other.mPeriodic == this->mPeriodic) &&
          (other.mColorStops == this->mColorStops));
}

void ColorMapper::setLevelCount(int n) {
  if (n < 2) {
    qDebug() << Q_FUNC_INFO << "n must be greater or equal 2 but was" << n;
    n = 2;
  }
  if (n != mLevelCount) {
    mLevelCount = n;
    mColorBufferInvalidated = true;
  }
}

void ColorMapper::setColorStops(const QMap<double, QColor> &colorStops) {
  mColorStops = colorStops;
  mColorBufferInvalidated = true;
}

void ColorMapper::setColorStopAt(double position, const QColor &color) {
  mColorStops.insert(position, color);
  mColorBufferInvalidated = true;
}

void ColorMapper::setColorInterpolation(
    ColorMapper::ColorInterpolation interpolation) {
  if (interpolation != mColorInterpolation) {
    mColorInterpolation = interpolation;
    mColorBufferInvalidated = true;
  }
}

void ColorMapper::setPeriodic(bool enabled) { mPeriodic = enabled; }

void ColorMapper::colorize(const double *data, const double &lower,
                           const double &upper, QRgb *scanLine, int n,
                           int dataIndexFactor, bool logarithmic) {
  // If you change something here, make sure to also adapt color() and the other
  // colorize() overload
  if (!data) {
    qDebug() << Q_FUNC_INFO << "null pointer given as data";
    return;
  }
  if (!scanLine) {
    qDebug() << Q_FUNC_INFO << "null pointer given as scanLine";
    return;
  }
  if (mColorBufferInvalidated)
    updateColorBuffer();

  const double range_size = upper - lower;
  if (!logarithmic) {
    const double posToIndexFactor = (mLevelCount - 1) / range_size;
    if (mPeriodic) {
      for (int i = 0; i < n; ++i) {
        int index =
            (int)((data[dataIndexFactor * i] - lower) * posToIndexFactor) %
            mLevelCount;
        if (index < 0)
          index += mLevelCount;
        scanLine[i] = mColorBuffer.at(index);
      }
    } else {
      for (int i = 0; i < n; ++i) {
        int index = (data[dataIndexFactor * i] - lower) * posToIndexFactor;
        if (index < 0)
          index = 0;
        else if (index >= mLevelCount)
          index = mLevelCount - 1;
        scanLine[i] = mColorBuffer.at(index);
      }
    }
  } else // logarithmic == true
  {
    if (mPeriodic) {
      for (int i = 0; i < n; ++i) {
        int index = (int)(qLn(data[dataIndexFactor * i] / lower) /
                          qLn(upper / lower) * (mLevelCount - 1)) %
                    mLevelCount;
        if (index < 0)
          index += mLevelCount;
        scanLine[i] = mColorBuffer.at(index);
      }
    } else {
      for (int i = 0; i < n; ++i) {
        int index = qLn(data[dataIndexFactor * i] / lower) /
                    qLn(upper / lower) * (mLevelCount - 1);
        if (index < 0)
          index = 0;
        else if (index >= mLevelCount)
          index = mLevelCount - 1;
        scanLine[i] = mColorBuffer.at(index);
      }
    }
  }
}

void ColorMapper::colorize(const double *data, const unsigned char *alpha,
                           const double &lower, const double &upper,
                           QRgb *scanLine, int n, int dataIndexFactor,
                           bool logarithmic) {
  // If you change something here, make sure to also adapt color() and the other
  // colorize() overload
  if (!data) {
    qDebug() << Q_FUNC_INFO << "null pointer given as data";
    return;
  }
  if (!alpha) {
    qDebug() << Q_FUNC_INFO << "null pointer given as alpha";
    return;
  }
  if (!scanLine) {
    qDebug() << Q_FUNC_INFO << "null pointer given as scanLine";
    return;
  }
  if (mColorBufferInvalidated)
    updateColorBuffer();

  const double range_size = upper - lower;
  if (!logarithmic) {
    const double posToIndexFactor = (mLevelCount - 1) / range_size;
    if (mPeriodic) {
      for (int i = 0; i < n; ++i) {
        int index =
            (int)((data[dataIndexFactor * i] - lower) * posToIndexFactor) %
            mLevelCount;
        if (index < 0)
          index += mLevelCount;
        if (alpha[dataIndexFactor * i] == 255) {
          scanLine[i] = mColorBuffer.at(index);
        } else {
          const QRgb rgb = mColorBuffer.at(index);
          const float alphaF = alpha[dataIndexFactor * i] / 255.0f;
          scanLine[i] = qRgba(qRed(rgb) * alphaF, qGreen(rgb) * alphaF,
                              qBlue(rgb) * alphaF, qAlpha(rgb) * alphaF);
        }
      }
    } else {
      for (int i = 0; i < n; ++i) {
        int index = (data[dataIndexFactor * i] - lower) * posToIndexFactor;
        if (index < 0)
          index = 0;
        else if (index >= mLevelCount)
          index = mLevelCount - 1;
        if (alpha[dataIndexFactor * i] == 255) {
          scanLine[i] = mColorBuffer.at(index);
        } else {
          const QRgb rgb = mColorBuffer.at(index);
          const float alphaF = alpha[dataIndexFactor * i] / 255.0f;
          scanLine[i] = qRgba(qRed(rgb) * alphaF, qGreen(rgb) * alphaF,
                              qBlue(rgb) * alphaF, qAlpha(rgb) * alphaF);
        }
      }
    }
  } else // logarithmic == true
  {
    if (mPeriodic) {
      for (int i = 0; i < n; ++i) {
        int index = (int)(qLn(data[dataIndexFactor * i] / lower) /
                          qLn(upper / lower) * (mLevelCount - 1)) %
                    mLevelCount;
        if (index < 0)
          index += mLevelCount;
        if (alpha[dataIndexFactor * i] == 255) {
          scanLine[i] = mColorBuffer.at(index);
        } else {
          const QRgb rgb = mColorBuffer.at(index);
          const float alphaF = alpha[dataIndexFactor * i] / 255.0f;
          scanLine[i] = qRgba(qRed(rgb) * alphaF, qGreen(rgb) * alphaF,
                              qBlue(rgb) * alphaF, qAlpha(rgb) * alphaF);
        }
      }
    } else {
      for (int i = 0; i < n; ++i) {
        int index = qLn(data[dataIndexFactor * i] / lower) /
                    qLn(upper / lower) * (mLevelCount - 1);
        if (index < 0)
          index = 0;
        else if (index >= mLevelCount)
          index = mLevelCount - 1;
        if (alpha[dataIndexFactor * i] == 255) {
          scanLine[i] = mColorBuffer.at(index);
        } else {
          const QRgb rgb = mColorBuffer.at(index);
          const float alphaF = alpha[dataIndexFactor * i] / 255.0f;
          scanLine[i] = qRgba(qRed(rgb) * alphaF, qGreen(rgb) * alphaF,
                              qBlue(rgb) * alphaF, qAlpha(rgb) * alphaF);
        }
      }
    }
  }
}

QRgb ColorMapper::color(double position, const double &lower,
                        const double &upper, bool logarithmic) {
  // If you change something here, make sure to also adapt ::colorize()
  if (mColorBufferInvalidated)
    updateColorBuffer();
  const double range_size = upper - lower;
  int index = 0;
  if (!logarithmic)
    index = (position - lower) * (mLevelCount - 1) / range_size;
  else
    index = qLn(position / lower) / qLn(upper / lower) * (mLevelCount - 1);
  if (mPeriodic) {
    index = index % mLevelCount;
    if (index < 0)
      index += mLevelCount;
  } else {
    if (index < 0)
      index = 0;
    else if (index >= mLevelCount)
      index = mLevelCount - 1;
  }
  return mColorBuffer.at(index);
}

void ColorMapper::loadPreset(GradientPreset preset) {
  clearColorStops();
  switch (preset) {
  case gpGrayscale:
    setColorInterpolation(ciRGB);
    setColorStopAt(0, Qt::black);
    setColorStopAt(1, Qt::white);
    break;
  case gpHot:
    setColorInterpolation(ciRGB);
    setColorStopAt(0, QColor(50, 0, 0));
    setColorStopAt(0.2, QColor(180, 10, 0));
    setColorStopAt(0.4, QColor(245, 50, 0));
    setColorStopAt(0.6, QColor(255, 150, 10));
    setColorStopAt(0.8, QColor(255, 255, 50));
    setColorStopAt(1, QColor(255, 255, 255));
    break;
  case gpCold:
    setColorInterpolation(ciRGB);
    setColorStopAt(0, QColor(0, 0, 50));
    setColorStopAt(0.2, QColor(0, 10, 180));
    setColorStopAt(0.4, QColor(0, 50, 245));
    setColorStopAt(0.6, QColor(10, 150, 255));
    setColorStopAt(0.8, QColor(50, 255, 255));
    setColorStopAt(1, QColor(255, 255, 255));
    break;
  case gpNight:
    setColorInterpolation(ciHSV);
    setColorStopAt(0, QColor(10, 20, 30));
    setColorStopAt(1, QColor(250, 255, 250));
    break;
  case gpCandy:
    setColorInterpolation(ciHSV);
    setColorStopAt(0, QColor(0, 0, 255));
    setColorStopAt(1, QColor(255, 250, 250));
    break;
  case gpGeography:
    setColorInterpolation(ciRGB);
    setColorStopAt(0, QColor(70, 170, 210));
    setColorStopAt(0.20, QColor(90, 160, 180));
    setColorStopAt(0.25, QColor(45, 130, 175));
    setColorStopAt(0.30, QColor(100, 140, 125));
    setColorStopAt(0.5, QColor(100, 140, 100));
    setColorStopAt(0.6, QColor(130, 145, 120));
    setColorStopAt(0.7, QColor(140, 130, 120));
    setColorStopAt(0.9, QColor(180, 190, 190));
    setColorStopAt(1, QColor(210, 210, 230));
    break;
  case gpIon:
    setColorInterpolation(ciHSV);
    setColorStopAt(0, QColor(50, 10, 10));
    setColorStopAt(0.45, QColor(0, 0, 255));
    setColorStopAt(0.8, QColor(0, 255, 255));
    setColorStopAt(1, QColor(0, 255, 0));
    break;
  case gpThermal:
    setColorInterpolation(ciRGB);
    setColorStopAt(0, QColor(0, 0, 50));
    setColorStopAt(0.15, QColor(20, 0, 120));
    setColorStopAt(0.33, QColor(200, 30, 140));
    setColorStopAt(0.6, QColor(255, 100, 0));
    setColorStopAt(0.85, QColor(255, 255, 40));
    setColorStopAt(1, QColor(255, 255, 255));
    break;
  case gpPolar:
    setColorInterpolation(ciRGB);
    setColorStopAt(0, QColor(50, 255, 255));
    setColorStopAt(0.18, QColor(10, 70, 255));
    setColorStopAt(0.28, QColor(10, 10, 190));
    setColorStopAt(0.5, QColor(0, 0, 0));
    setColorStopAt(0.72, QColor(190, 10, 10));
    setColorStopAt(0.82, QColor(255, 70, 10));
    setColorStopAt(1, QColor(255, 255, 50));
    break;
  case gpSpectrum:
    setColorInterpolation(ciHSV);
    setColorStopAt(0, QColor(50, 0, 50));
    setColorStopAt(0.15, QColor(0, 0, 255));
    setColorStopAt(0.35, QColor(0, 255, 255));
    setColorStopAt(0.6, QColor(255, 255, 0));
    setColorStopAt(0.75, QColor(255, 30, 0));
    setColorStopAt(1, QColor(50, 0, 0));
    break;
  case gpJet:
    setColorInterpolation(ciRGB);
    setColorStopAt(0, QColor(0, 0, 100));
    setColorStopAt(0.15, QColor(0, 50, 255));
    setColorStopAt(0.35, QColor(0, 255, 255));
    setColorStopAt(0.65, QColor(255, 255, 0));
    setColorStopAt(0.85, QColor(255, 30, 0));
    setColorStopAt(1, QColor(100, 0, 0));
    break;
  case gpHues:
    setColorInterpolation(ciHSV);
    setColorStopAt(0, QColor(255, 0, 0));
    setColorStopAt(1.0 / 3.0, QColor(0, 0, 255));
    setColorStopAt(2.0 / 3.0, QColor(0, 255, 0));
    setColorStopAt(1, QColor(255, 0, 0));
    break;
  }
  mPreset = preset;
}

void ColorMapper::clearColorStops() {
  mColorStops.clear();
  mColorBufferInvalidated = true;
}

ColorMapper ColorMapper::inverted() const {
  ColorMapper result(*this);
  result.clearColorStops();
  for (QMap<double, QColor>::const_iterator it = mColorStops.constBegin();
       it != mColorStops.constEnd(); ++it)
    result.setColorStopAt(1.0 - it.key(), it.value());
  return result;
}

bool ColorMapper::stopsUseAlpha() const {
  for (QMap<double, QColor>::const_iterator it = mColorStops.constBegin();
       it != mColorStops.constEnd(); ++it) {
    if (it.value().alpha() < 255)
      return true;
  }
  return false;
}

void ColorMapper::updateColorBuffer() {
  if (mColorBuffer.size() != mLevelCount)
    mColorBuffer.resize(mLevelCount);
  if (mColorStops.size() > 1) {
    double indexToPosFactor = 1.0 / (double)(mLevelCount - 1);
    const bool useAlpha = stopsUseAlpha();
    for (int i = 0; i < mLevelCount; ++i) {
      double position = i * indexToPosFactor;
      auto it = mColorStops.lowerBound(position);
      if (it == mColorStops.end()) // position is on or after last stop, use
                                   // color of last stop
      {
        if (useAlpha) {
          const QColor col = std::prev(it).value();
          const float alphaPremultiplier =
              col.alpha() /
              255.0f; // since we use QImage::Format_ARGB32_Premultiplied
          mColorBuffer[i] = qRgba(col.red() * alphaPremultiplier,
                                  col.green() * alphaPremultiplier,
                                  col.blue() * alphaPremultiplier, col.alpha());
        } else
          mColorBuffer[i] = std::prev(it).value().rgba();
      } else if (it == mColorStops.begin()) // position is on or before first
                                            // stop, use color of first stop
      {
        if (useAlpha) {
          const QColor col = it.value();
          const float alphaPremultiplier =
              col.alpha() /
              255.0f; // since we use QImage::Format_ARGB32_Premultiplied
          mColorBuffer[i] = qRgba(col.red() * alphaPremultiplier,
                                  col.green() * alphaPremultiplier,
                                  col.blue() * alphaPremultiplier, col.alpha());
        } else
          mColorBuffer[i] = it.value().rgba();
      } else // position is in between stops (or on an intermediate stop),
             // interpolate color
      {
        auto high = it;
        auto low = std::prev(it);
        double t = (position - low.key()) /
                   (high.key() - low.key()); // interpolation factor 0..1
        switch (mColorInterpolation) {
        case ciRGB: {
          if (useAlpha) {
            const int alpha =
                (1 - t) * low.value().alpha() + t * high.value().alpha();
            const float alphaPremultiplier =
                alpha /
                255.0f; // since we use QImage::Format_ARGB32_Premultiplied
            mColorBuffer[i] = qRgba(
                ((1 - t) * low.value().red() + t * high.value().red()) *
                    alphaPremultiplier,
                ((1 - t) * low.value().green() + t * high.value().green()) *
                    alphaPremultiplier,
                ((1 - t) * low.value().blue() + t * high.value().blue()) *
                    alphaPremultiplier,
                alpha);
          } else {
            mColorBuffer[i] =
                qRgb(((1 - t) * low.value().red() + t * high.value().red()),
                     ((1 - t) * low.value().green() + t * high.value().green()),
                     ((1 - t) * low.value().blue() + t * high.value().blue()));
          }
          break;
        }
        case ciHSV: {
          QColor lowHsv = low.value().toHsv();
          QColor highHsv = high.value().toHsv();
          double hue = 0;
          double hueDiff = highHsv.hueF() - lowHsv.hueF();
          if (hueDiff > 0.5)
            hue = lowHsv.hueF() - t * (1.0 - hueDiff);
          else if (hueDiff < -0.5)
            hue = lowHsv.hueF() + t * (1.0 + hueDiff);
          else
            hue = lowHsv.hueF() + t * hueDiff;
          if (hue < 0)
            hue += 1.0;
          else if (hue >= 1.0)
            hue -= 1.0;
          if (useAlpha) {
            const QRgb rgb =
                QColor::fromHsvF(
                    hue,
                    (1 - t) * lowHsv.saturationF() + t * highHsv.saturationF(),
                    (1 - t) * lowHsv.valueF() + t * highHsv.valueF())
                    .rgb();
            const float alpha =
                (1 - t) * lowHsv.alphaF() + t * highHsv.alphaF();
            mColorBuffer[i] = qRgba(qRed(rgb) * alpha, qGreen(rgb) * alpha,
                                    qBlue(rgb) * alpha, 255 * alpha);
          } else {
            mColorBuffer[i] =
                QColor::fromHsvF(
                    hue,
                    (1 - t) * lowHsv.saturationF() + t * highHsv.saturationF(),
                    (1 - t) * lowHsv.valueF() + t * highHsv.valueF())
                    .rgb();
          }
          break;
        }
        }
      }
    }
  } else if (mColorStops.size() == 1) {
    const QRgb rgb = mColorStops.constBegin().value().rgb();
    const float alpha = mColorStops.constBegin().value().alphaF();
    mColorBuffer.fill(qRgba(qRed(rgb) * alpha, qGreen(rgb) * alpha,
                            qBlue(rgb) * alpha, 255 * alpha));
  } else // mColorStops is empty, fill color buffer with black
  {
    mColorBuffer.fill(qRgb(0, 0, 0));
  }
  mColorBufferInvalidated = false;
}
