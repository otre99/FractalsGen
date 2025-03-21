#include <QDebug>
#include <functional>
#include <limits>

#include "fractals.h"

void Family00::Init(const FractalParameters &p) {
  Fractal::Init(p);
  c_.real(p.c.real());
  c_.imag(p.c.imag());
  q_ = p.q.real();
}

double Family00::CalcFinalNormMandelbrot(const cmplx &c) const {
  // qDebug() << " Family00::CalcFinalNormMandelbrot() "<<  double(c.real()) <<
  // double(c.imag());
  int iter = 1;
  dbltype x, y, dem;
  dbltype xx, yy, tmp1, tmp2, tmp0, q2;

  x = c_.real();
  y = c_.imag();
  q2 = q_ * q_;
  dbltype dist = std::numeric_limits<double>::max();
  for (; iter < max_iter_; ++iter) {
    tmp1 = x;
    tmp2 = y;
    x *= x;
    y *= y;
    dist = std::min(orbit_metric_funct_(tmp1, tmp2), dist);
    if (x + y >= th_norm_) break;

    x -= y;
    y = 2 * tmp1 * tmp2;
    xx = x * x;
    yy = y * y;
    tmp0 = (xx + yy) * q2;
    dem = tmp0 - 2 * (q_ * x) + 1;
    tmp1 = -x * tmp0 - 2 * q_ * yy + x;
    tmp2 = -y * tmp0 + 2 * q_ * x * y + y;
    x = tmp1 / dem + c.real();
    y = tmp2 / dem + c.imag();
  }
  return dist;
}

double Family00::CalcFinalNormJulia(const cmplx &z) const {
  // qDebug() << " Family00::CalcFinalNormJulia() "<<  double(z.real()) <<
  // double(z.imag());
  int iter = 0;
  dbltype x, y, dem;
  dbltype xx, yy, tmp1, tmp2, tmp0, q2;
  x = z.real();
  y = z.imag();
  q2 = q_ * q_;
  dbltype dist = std::numeric_limits<dbltype>::max();
  for (; iter < max_iter_; ++iter) {
    tmp1 = x;
    tmp2 = y;
    x *= x;
    y *= y;
    dist = std::min(orbit_metric_funct_(tmp1, tmp2), dist);
    if (x + y >= th_norm_) break;
    x -= y;
    y = 2 * tmp1 * tmp2;
    xx = x * x;
    yy = y * y;
    tmp0 = (xx + yy) * q2;
    dem = tmp0 - 2 * (q_ * x) + 1;
    tmp1 = -x * tmp0 - 2 * q_ * yy + x;
    tmp2 = -y * tmp0 + 2 * q_ * x * y + y;
    x = tmp1 / dem + c_.real();
    y = tmp2 / dem + c_.imag();
  }
  return dist;
}

//////////////////////////////////////////
double Family00::CalcEscapeJulia(const cmplx &z) const {
  // qDebug() << " Family00::CalcEscapeJulia() "<<  double(z.real()) <<
  // double(z.imag());
  int iter = 0;
  dbltype x, y, dem;
  dbltype xx, yy, tmp1, tmp2, tmp0, q2;

  x = z.real();
  y = z.imag();
  q2 = q_ * q_;
  for (;;) {
    tmp1 = x;
    tmp2 = y;
    x *= x;
    y *= y;
    if (x + y >= th_norm_ || iter >= max_iter_) break;
    x -= y;
    y = 2 * tmp1 * tmp2;
    xx = x * x;
    yy = y * y;
    tmp0 = (xx + yy) * q2;
    dem = tmp0 - 2 * (q_ * x) + 1;
    tmp1 = -x * tmp0 - 2 * q_ * yy + x;
    tmp2 = -y * tmp0 + 2 * q_ * x * y + y;
    x = tmp1 / dem + c_.real();
    y = tmp2 / dem + c_.imag();
    ++iter;
  }
  return iter;
}

double Family00::CalcEscapeMandelbrot(const cmplx &c) const {
  // qDebug() << " Family00::CalcEscapeMandelbrot() "<<  double(c.real()) <<
  // double(c.imag());
  int iter = 0;
  dbltype x, y, dem;
  dbltype xx, yy, tmp1, tmp2, tmp0, q2;

  x = 0.0;
  y = 0.0;
  q2 = q_ * q_;
  for (;;) {
    tmp1 = x;
    tmp2 = y;
    x *= x;
    y *= y;
    if (x + y >= th_norm_ || iter >= max_iter_) break;
    x -= y;
    y = 2 * tmp1 * tmp2;
    xx = x * x;
    yy = y * y;
    tmp0 = (xx + yy) * q2;
    dem = tmp0 - 2 * (q_ * x) + 1;
    tmp1 = -x * tmp0 - 2 * q_ * yy + x;
    tmp2 = -y * tmp0 + 2 * q_ * x * y + y;
    x = tmp1 / dem + c.real();
    y = tmp2 / dem + c.imag();
    ++iter;
  }
  return iter;
}
