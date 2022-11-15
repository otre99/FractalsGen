#ifndef COLORMAPPING_H
#define COLORMAPPING_H
#include <QColor>
#include <QMap>
#include <QRgb>
#include <QVector>

class ColorMapper {
public:
  enum ColorInterpolation { ciRGB, ciHSV };
  enum GradientPreset {
    gpGrayscale,
    gpHot,
    gpCold,
    gpNight,
    gpCandy,
    gpGeography,
    gpIon,
    gpThermal,
    gpPolar,
    gpSpectrum,
    gpJet,
    gpHues
  } mPreset;

  ColorMapper();
  ColorMapper(GradientPreset preset);
  bool operator==(const ColorMapper &other) const;
  bool operator!=(const ColorMapper &other) const { return !(*this == other); }
  GradientPreset preset() { return mPreset; }

  // getters:
  int levelCount() const { return mLevelCount; }
  QMap<double, QColor> colorStops() const { return mColorStops; }
  ColorInterpolation colorInterpolation() const { return mColorInterpolation; }
  bool periodic() const { return mPeriodic; }

  // setters:
  void setLevelCount(int n);
  void setColorStops(const QMap<double, QColor> &colorStops);
  void setColorStopAt(double position, const QColor &color);
  void setColorInterpolation(ColorInterpolation interpolation);
  void setPeriodic(bool enabled);

  // non-property methods:
  void colorize(const double *data, const double &lower, const double &upper,
                QRgb *scanLine, int n, int dataIndexFactor = 1,
                bool logarithmic = false);
  void colorize(const double *data, const unsigned char *alpha,
                const double &lower, const double &upper, QRgb *scanLine, int n,
                int dataIndexFactor = 1, bool logarithmic = false);
  QRgb color(double position, const double &lower, const double &upper,
             bool logarithmic = false);
  void loadPreset(GradientPreset preset);
  void clearColorStops();
  ColorMapper inverted() const;

protected:
  // property members:
  int mLevelCount;
  QMap<double, QColor> mColorStops;
  ColorInterpolation mColorInterpolation;
  bool mPeriodic;

  // non-property members:
  QVector<QRgb> mColorBuffer; // have colors premultiplied with alpha (for usage
                              // with QImage::Format_ARGB32_Premultiplied)
  bool mColorBufferInvalidated;

  // non-virtual methods:
  bool stopsUseAlpha() const;
  void updateColorBuffer();
};

#endif // COLORMAPPING_H
